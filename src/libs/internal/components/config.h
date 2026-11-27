#ifndef INTERNAL_H_COMP_CONFIG
#define INTERNAL_H_COMP_CONFIG
#define SSCVCONF_SUCCESS (0)
#define SSCVCONF_FAILURE (1)
#include <string>
#include <map>

namespace internal::components::conf {
  typedef std::map<std::string, std::string>
    sscv_config_map_t;
  typedef enum {
    READ_ALL,   READ,
    WRITE_ALL,  WRITE,
  } sscv_conf_man_mode;
  std::string sscv_config_read(std::string key);
  int sscv_config_man(
      sscv_conf_man_mode mode,
      std::string key = std::string(), 
      std::string val = std::string());
  int sscv_config_init(void); 
}
#endif
