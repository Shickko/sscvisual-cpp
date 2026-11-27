#ifndef INTERNAL_H_COMP_STEAMCURL
#define INTERNAL_H_COMP_STEAMCURL
#include <string>
#include <vector>
#include <ctime>

namespace internal::components {
  struct steamdt_player_summaries {
    std::string steamid;
    std::string primaryclanid;
    std::string loccountrycode;
    std::string personaname;
    std::string avatarfull;
    int    communityvisibilitystate { 0 };
    int    personastate { 0 };
    time_t lastlogoff { 0 };
    time_t timecreated { 0 };
  };
  struct steamdt_player_friend {
    std::string steamid;
    std::string relationship;
    long friend_since { 0 };
  };

  typedef std::vector<steamdt_player_summaries> 
    steamdt_player_summaries_arr;
  typedef std::vector<steamdt_player_friend>    
    steamdt_player_friends;

  class steamcli_t {
    private:
      std::string authkey_;
      std::string authid_;
    public:
      steamcli_t();
      steamcli_t(std::string authkey, std::string authid);
      steamdt_player_summaries player_summaries(const char* steamid);
      steamdt_player_summaries_arr players_summaries(const char* steamids);
      steamdt_player_friends player_friendlist(const char* steamid);

      std::string player_vanityurl(const char* vanity);

      std::string get_authkey();
      std::string get_authid();
      void set(std::string authkey, std::string authid);
  };
}
#endif
