#include <components/config.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
#include <vector>

namespace fs = std::filesystem;
namespace internal::components::conf {
  static const char* env_conf = std::getenv("XDG_CONFIG_HOME");
  static const char* env_home = std::getenv("HOME");
  static sscv_config_map_t sscv_config_map {
    { "usr_authkey", "" },
    { "usr_authid",  "" }
  };
  static fs::path path_sscv;
}

std::string internal::components::conf::sscv_config_read(std::string key) {
  std::string exception_str {};
  if (key.empty()) { return exception_str; }
  return (sscv_config_man(READ, key) == SSCVCONF_SUCCESS &&
          sscv_config_map.find(key) != sscv_config_map.end()
  ) ? sscv_config_map[key] : exception_str;
}

int internal::components::conf::sscv_config_man(
    internal::components::conf::sscv_conf_man_mode mode,
    std::string key,
    std::string val) {
  if (!fs::is_regular_file(path_sscv/"sscv.conf") && (mode == READ || mode == READ_ALL)) 
  { return SSCVCONF_FAILURE; }
  std::fstream fp(
    path_sscv/"sscv.conf", 
    (mode == WRITE || mode == WRITE_ALL)
    ? (std::ios::out | std::ios::trunc)
    : (std::ios::in)
  );
  if (!fp.is_open()) { return SSCVCONF_FAILURE; }
  
  switch (mode) {
    case READ: case READ_ALL:  {
                  std::vector<char> filtered = {
                    '#', '\t', ' '
                  };
                  std::string l;
                  while (std::getline(fp, l)) {
                    auto pos = l.find(':');
                    if (pos == std::string::npos)
                      continue;
                    std::string parsed_key = l.substr(0, pos);
                    std::string parsed_val = l.substr(pos + 1);
                    if (!parsed_val.empty() && parsed_val[0] == ' ') {
                      for (char ch : filtered) {
                        if  (parsed_val[0] == ch) {
                          parsed_val.erase(0, 1);
                        }
                      }
                    }
                    if (mode == READ_ALL)
                      sscv_config_map[parsed_key] = parsed_val;
                    if (mode == READ && parsed_key == key) {
                      sscv_config_map[parsed_key] = parsed_val;
                      break;
                    }
                  }
                  break;
                }
    case WRITE: case WRITE_ALL: {
                  if (mode == WRITE)
                    sscv_config_map[key] = val.empty() ? "" : val;
                  for (const auto& [k, v] : sscv_config_map) {
                    fp << std::format("{}: {}\n", k, v);
                  }
                  break;
                }
    default:    {
                  fp.close();
                  return SSCVCONF_FAILURE;
                }
  }

  fp.close();
  return SSCVCONF_SUCCESS;
}

int internal::components::conf::sscv_config_init() {
#ifndef _WIN32
  if ((env_conf == nullptr || env_conf[0] == '\0') && 
     (env_home == nullptr || env_home[0] == '\0')) {
    std::cout << "[[SSCV_ERR >> dir HOME unset in env" << std::endl;
    return SSCVCONF_FAILURE;
  }
  path_sscv = (env_conf == nullptr || env_conf[0] == '\0')
    ? fs::path(env_home)/".config"/"sscvisual"
    : fs::path(env_conf)/"sscvisual";
  fs::create_directories(path_sscv);

  if (fs::is_regular_file(path_sscv/"sscv.conf")) { 
    return sscv_config_man(READ_ALL); 
  }

#else

#endif
  std::cout << "[[SSCV >> generating new configuration file" << std::endl;
  return sscv_config_man(WRITE_ALL);
}
