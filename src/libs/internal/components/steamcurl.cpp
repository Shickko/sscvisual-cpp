#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <components/steamcurl.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace internal::components;

static std::string base_url = "https://api.steampowered.com";
static json steamcurl_fetch(std::string host, std::string path) {
  httplib::Client cli(host);
  cli.set_max_timeout(1200);
  auto res = cli.Get(path);
  json data = json::object();
  if (!res) {
    std::cout << "[[ERR] Failed to fetch: Internet Failure." << std::endl;
  } else if (res->status == 200) {
    data = json::parse(res->body, nullptr, false);
    if (data.is_discarded()) {
      std::cout << "[ERR] Failed to parse json." << std::endl;
      data = json::object();
    }
  } else {
    std::cout << "[ERR] Failed to fetch: " << res->status << std::endl;
  }
  return data;
}

static void steamavatar_write(std::string url, std::string path) {
  size_t scheme = url.find("://");
  size_t start  = (scheme != std::string::npos) ? scheme + 3 : 0;
  size_t slash  = url.find('/', start);
  std::string host   = (slash != std::string::npos) ? url.substr(0, slash) : url;
  std::string target = (slash != std::string::npos) ? url.substr(slash) : std::string("/");

  httplib::Client cli(host);
  cli.set_follow_location(true);
  auto res = cli.Get(target);
  if (res && res->status == 200) {
    std::ofstream out(path, std::ios::binary);
    if (out.is_open()) {
      out.write(res->body.data(), res->body.size());
      out.close();
    }
  }
}

static std::string steamid_check(steamcli_t* cli, const char* id) {
  if (cli == nullptr || id == nullptr) { return ""; }
  std::string out;
  char* end = nullptr;
  std::strtoll(id, &end, 10);
  if (end == id || *end != '\0') {
    out = cli->player_vanityurl(id);
  } else {
    out = id;
  }
  return out;
}

static steamdt_player_summaries steamdt_player_summaries_set(json& j) {
  steamdt_player_summaries r{};
  if (!j.is_object() || j.empty()) {
    r.communityvisibilitystate = -1;
    return r;
  }
  r.communityvisibilitystate = j.value("communityvisibilitystate", -1);
  if (r.communityvisibilitystate == 3) {   // 3 == public profile
    r.steamid        = j.value("steamid", std::string("unknown"));
    r.personastate   = j.value("personastate", -1);
    r.lastlogoff     = j.value("lastlogoff", static_cast<time_t>(-1));
    r.timecreated    = j.value("timecreated", static_cast<time_t>(-1));
    r.primaryclanid  = j.value("primaryclanid", std::string(""));
    r.loccountrycode = j.value("loccountrycode", std::string(""));
    r.personaname    = j.value("personaname", std::string(""));
    r.avatarfull     = j.value("avatarfull", std::string(""));
    if (!r.avatarfull.empty()) {
      steamavatar_write(r.avatarfull,
        std::format(".cache/{}.jpg", r.steamid));
    }
  }
  return r;
}

static steamdt_player_friend steamdt_player_friend_set(json& j) {
  steamdt_player_friend r{};
  if (j.is_object() && !j.empty()) {
    r.steamid       = j.value("steamid", std::string("unknown"));
    r.relationship  = j.value("relationship", std::string("unknown"));
    r.friend_since  = j.value("friend_since", -1L);
  }
  return r;
}

steamcli_t::steamcli_t(std::string authkey, std::string authid)
  : authkey_(authkey), authid_(authid) {}

std::string steamcli_t::get_authkey() { return authkey_; }
std::string steamcli_t::get_authid()  { return authid_; }

std::string steamcli_t::player_vanityurl(const char* vanity) {
  if (vanity == nullptr) { return ""; }
  std::string url = std::format(
    "/ISteamUser/ResolveVanityURL/v0001/?key={}&vanityurl={}",
    authkey_, vanity
  );
  json j = steamcurl_fetch(base_url, url);
  if (!j.contains("response")) { return ""; }
  json resp = j["response"];
  if (!resp.is_object() || !resp.contains("success")) { return ""; }
  if (resp["success"].get<int>() == 1 && resp.contains("steamid")) {
    return resp["steamid"].get<std::string>();
  }
  return "";
}

steamdt_player_summaries steamcli_t::player_summaries(const char* steamid) {
  std::string sid = steamid_check(this, steamid);
  std::string url = std::format(
    "/ISteamUser/GetPlayerSummaries/v0002/?key={}&steamids={}",
    authkey_, sid.c_str()
  );
  json j = steamcurl_fetch(base_url, url);
  json first;
  if (j.contains("response") && j["response"].contains("players")
      && j["response"]["players"].is_array() && !j["response"]["players"].empty()) {
    first = j["response"]["players"][0];
  }
  return steamdt_player_summaries_set(first);
}

steamdt_player_summaries_arr steamcli_t::players_summaries(const char* steamids) {
  steamdt_player_summaries_arr result;
  std::string input(steamids ? steamids : "");
  std::string ids, token;
  size_t count = 0;

  auto flush = [&]() {
    if (token.empty()) { return; }
    std::string id = steamid_check(this, token.c_str());
    if (!id.empty()) {
      if (!ids.empty()) { ids += ','; }
      ids += id;
    }
    token.clear();
  };

  for (char c : input) {
    if (count > 100) { break; }
    if (c == ' ' || c == ',' || c == ';') {
      count++;
      flush();
    } else {
      token.push_back(c);
    }
  }
  flush();
  if (ids.empty()) { return result; }

  std::string url = std::format(
    "/ISteamUser/GetPlayerSummaries/v0002/?key={}&steamids={}",
    authkey_, ids.c_str()
  );
  json j = steamcurl_fetch(base_url, url);
  json players = json::array();
  if (j.contains("response") && j["response"].contains("players")) {
    players = j["response"]["players"];
  }
  for (size_t i = 0; i < players.size(); i++) {
    std::cout << std::format("Fetching User - {}/{}", i + 1, players.size()) << std::endl;
    result.push_back(steamdt_player_summaries_set(players[i]));
  }
  return result;
}

steamdt_player_friends steamcli_t::player_friendlist(const char* steamid) {
  steamdt_player_friends result;
  std::string sid = steamid_check(this, steamid);
  std::string url = std::format(
    "/ISteamUser/GetFriendList/v0001/?key={}&steamid={}",
    authkey_, sid.c_str()
  );
  json j = steamcurl_fetch(base_url, url);
  json friends = json::array();
  if (j.contains("friendslist") && j["friendslist"].contains("friends")) {
    friends = j["friendslist"]["friends"];
  }
  if (friends.size() > 0) {
    std::cout << "Size: " << friends.size() << std::endl;
    for (size_t i = 0; i < friends.size(); i++) {
      std::cout << std::format("Fetching User - {}/{}", i + 1, friends.size()) << std::endl;
      result.push_back(steamdt_player_friend_set(friends[i]));
    }
  }
  return result;
}

