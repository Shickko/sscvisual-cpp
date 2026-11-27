#include <components/steamcurl.h>
#include <components/config.h>
#include <graphics/render.h>
#include <visual/profile.h>
#include <imgui.h>
#include <format>
#include <string>
#include <iostream>

namespace internal::visual::profile {
  static internal::components::steamcli_t steamcli;
  static internal::components::steamdt_player_summaries user_data;
  static internal::graphics::image                      user_image;
  static std::string                                    user_stat;
  static bool initialized { false };
  static bool fetched     { false };
}

static bool profile_fetch(void) {
  internal::visual::profile::steamcli.set(
    internal::components::conf::sscv_config_read("usr_authkey"),
    internal::components::conf::sscv_config_read("usr_authid")
  );
  std::cout << std::format("trying to fetch with key {} for user={}\n",
    internal::visual::profile::steamcli.get_authkey(),
    internal::visual::profile::steamcli.get_authid()
  );

  internal::visual::profile::user_data = internal::visual::profile::steamcli.player_summaries(
  internal::visual::profile::steamcli.get_authid().c_str());
  if (internal::visual::profile::user_data.communityvisibilitystate != -1) {
    switch (internal::visual::profile::user_data.personastate) {
      default : { internal::visual::profile::user_stat = "[UNKNOWN]"; break; }
      case (0): { internal::visual::profile::user_stat = "[Offline]"; break; }
      case (1): { internal::visual::profile::user_stat = "[Online]";  break; }
      case (2): { internal::visual::profile::user_stat = "[Busy]";    break; }
      case (3): { internal::visual::profile::user_stat = "[Away]";    break; }
      case (4): { internal::visual::profile::user_stat = "[Snooze]";  break; }
    };
    internal::visual::profile::user_image.create_image(
      std::format(
        ".cache/{}.jpg", 
        internal::visual::profile::user_data.steamid
      ).c_str(), 
      32, 32
    );
    return true;
  }
  return false;
}

void internal::visual::profile::render(void) {
  if (!initialized) { 
    fetched = profile_fetch(); 
    initialized = true;
  }
  if (!fetched) {
    ImGui::Text("Fetch Failure...");
  }

  if (fetched) {
    if (ImGui::BeginTable("User Profile", 2)) {
      ImGui::TableSetupColumn("Info",   ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Avatar", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(user_data.personaname.c_str()); ImGui::SameLine(); ImGui::Text(user_stat.c_str());
      ImGui::Text("Time of Creation: \n%s", std::ctime(&user_data.timecreated));
      if (user_data.lastlogoff != -1) {
        ImGui::Text("Last Log Off: \n%s", std::ctime(&user_data.lastlogoff));
      }
      ImGui::TableSetColumnIndex(1);
      if (user_image.isLoaded) {
        ImGui::Image(
          (ImTextureID)(intptr_t)user_image.texture, ImVec2(
          (float)(user_image.width),
          (float)(user_image.height))
        );
      }
      ImGui::TableNextRow();
      ImGui::EndTable();
    }
  }
  
}
