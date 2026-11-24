#include <utils/config.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <format>

namespace fs = std::filesystem;

int internal::utils::sscv_config_init() {
#ifdef _WIN32
  return SSCVCONF_FAILURE; // Yet not worked 
#else
  std::cout << std::getenv("XDG_CONFIG_HOME") << std::endl;

  const char* dir_home = std::getenv("HOME");
  if (!dir_home) {
    std::cerr << "[[SSCV_ERR >> DIR >> HOME unset in env" << std::endl;
    return SSCVCONF_FAILURE;
  }

  const fs::path dir_sscv = fs::path(dir_home)/".config"/"sscvisual";
  const fs::path dir_conf = dir_sscv/"sscv.conf";
  std::error_code err;

  fs::create_directories(dir_sscv, err);
  if (err) {
    std::cerr << "[[SSCV_ERR >> DIR >> "
              << dir_sscv << " >> "
              << err.message()
              << std::endl;
    return SSCVCONF_FAILURE;
  }
  if (fs::is_regular_file(dir_conf, err)) {
    return SSCVCONF_SUCCESS;
  }
  
  if (std::ofstream outf(dir_conf); !outf) {
    std::cerr << "[[SSCV_ERR >> FILE >> Unable to create "
              << dir_conf
              << std::endl;
    return SSCVCONF_FAILURE;
  } else {
  /* Create Here */
  }
  return SSCVCONF_SUCCESS;
#endif
}

