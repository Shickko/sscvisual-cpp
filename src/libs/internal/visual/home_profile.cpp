#include <visual/home_profile.h>
#include <components/steamcurl.h>
#include <graphics/glfw_window.h>
#include <graphics/render.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <iostream>
#include <string>
#include <format>
#include <ctime>

static internal::components::steamcli_t 
steamcli(); // currently hardcoded,
            // will be reading from
            // conf in the future.

static bool window_state { true };

struct usr_vars {
  bool isLoaded { false };
  std::string usr_stat { "" };
  internal::graphics::image usr_img {};
};
usr_vars loadvar(internal::components::steamdt_player_summaries &usr_data) {
  usr_vars vars;
  if (usr_data.communityvisibilitystate != -1) {
    switch (usr_data.personastate) {
      default : { vars.usr_stat = "[UNKNOWN]"; break; }
      case (0): { vars.usr_stat = "[Offline]"; break; }
      case (1): { vars.usr_stat = "[Online]";  break; }
      case (2): { vars.usr_stat = "[Busy]";    break; }
      case (3): { vars.usr_stat = "[Away]";    break; }
      case (4): { vars.usr_stat = "[Snooze]";  break; }
    };
    vars.usr_img.create_image(std::format(".cache/{}.jpg", usr_data.steamid).c_str(), 32, 32);
    // will also be migrated later
    vars.isLoaded = true;
  } else {
    vars.isLoaded = false;
  }
  return vars;
}

static void sscv_fetch_user(internal::components::steamdt_player_summaries &usr_data, usr_vars &vars) {
  if (vars.isLoaded == false) { return; }
  if (ImGui::BeginTable("User Profile", 2)) {
    ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Avatar", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(usr_data.personaname.c_str()); ImGui::SameLine(); ImGui::Text(vars.usr_stat.c_str());
    ImGui::Text("Time Of Creation: \n%s", std::ctime(&usr_data.timecreated));
    if (usr_data.lastlogoff != -1) {
      ImGui::Text("Last logoff: \n%s", std::ctime(&usr_data.lastlogoff));
    }
    ImGui::TableSetColumnIndex(1);
    if (vars.usr_img.isLoaded) {
      ImGui::Image(
        (ImTextureID)(intptr_t)vars.usr_img.texture, ImVec2(
        (float)(vars.usr_img.width), (float)(vars.usr_img.height))
      );
    }
    ImGui::EndTable();
  }
}

static void sscv_create_home_components_multi(void) {
  static time_t tNow;
  static bool isLoaded { false };
  static char user_uidstr[99] = "User IDs";
  static std::vector<usr_vars> usr_vars_arr {};
  static internal::components::steamdt_player_summaries_arr usr_data_arr {};
  static internal::components::steamdt_player_friends usr_friends {};

  ImGui::SetNextItemWidth(-FLT_MIN); 
  ImGui::InputText("##", user_uidstr, IM_COUNTOF(user_uidstr));

  auto fetch_players_lambda = [&](const char* dtstr) {
    usr_data_arr.clear();
    usr_vars_arr.clear();
    usr_data_arr = steamcli.players_summaries(dtstr);
    for (int i = 0; i < usr_data_arr.size(); i ++) {
      usr_vars_arr.push_back(loadvar(usr_data_arr[i]));
    }
    isLoaded = true;
  };

  if (isLoaded) {
    for (int i = 0; i < usr_data_arr.size(); i ++) {
      sscv_fetch_user(usr_data_arr[i], usr_vars_arr[i]);
    }
  }
  if (ImGui::Button("[DEBUG] User Summaries - Fetch Multi", ImVec2(-FLT_MIN, 0.00f))) {
    fetch_players_lambda(user_uidstr);
    /*
    usr_data_arr.clear();
    usr_vars_arr.clear();
    usr_data_arr = steamcli.players_summaries(user_uidstr);
    for (int i = 0; i < usr_data_arr.size(); i ++) {
      usr_vars_arr.push_back(loadvar(usr_data_arr[i]));
    }
    isLoaded = true; */
  }
  if (ImGui::Button("[DEBUG] User Friends - Fetch", ImVec2(-FLT_MIN, 0.00f))) {
    std::string user_friendlist {};
    usr_friends.clear();
    usr_friends = steamcli.player_friendlist(user_uidstr);
    if (usr_friends.size() > 0) {
      for (int i = 0; i < usr_friends.size(); i ++) {
        user_friendlist.append(usr_friends[i].steamid);
        if (i != usr_friends.size() - 1) {
          user_friendlist.push_back(',');
        }
      }
      fetch_players_lambda(user_friendlist.c_str());
    }
  }
}

static void sscv_create_home_components(void) {
  static time_t tNow;
  static char user_uidstr[65] = "User ID";
  static int user_fetch_state { 0 };
  static std::string userstat {""};
  static internal::graphics::image usrimg;
  static internal::components::steamdt_player_summaries user_data;

  ImGui::PushFont(NULL, 30.0f); 

  ImGui::SetNextItemWidth(-FLT_MIN); 
  ImGui::InputText("##", user_uidstr, IM_COUNTOF(user_uidstr));
  
  if (user_fetch_state == -1) {
    ImGui::Text("[FETCH FAILURE]");
  }

  if (user_fetch_state == 1) {
    if (ImGui::BeginTable("User Profile", 2)) {
      ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Avatar", ImGuiTableColumnFlags_WidthFixed);
      tNow = time(0);
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(std::ctime(&tNow));
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(user_data.personaname.c_str()); ImGui::SameLine(); ImGui::Text(userstat.c_str());
      ImGui::Text("Time Of Creation: \n%s", std::ctime(&user_data.timecreated));
      ImGui::TableSetColumnIndex(1);
      if (usrimg.isLoaded) {
        ImGui::Image(
          (ImTextureID)(intptr_t)usrimg.texture, ImVec2(
          (float)(usrimg.width), (float)(usrimg.height))
        );
      }
      ImGui::EndTable();
    }
  }

  if (ImGui::Button("[DEBUG] User Summaries - Fetch", ImVec2(-FLT_MIN, 0.00f))) {
    user_data = steamcli.player_summaries(user_uidstr);
    if (user_data.communityvisibilitystate != -1) {
      switch (user_data.personastate) {
        default : { userstat = "[UNKNOWN]"; break; }
        case (0): { userstat = "[Offline]"; break; }
        case (1): { userstat = "[Online]";  break; }
        case (2): { userstat = "[Busy]";    break; }
        case (3): { userstat = "[Away]";    break; }
        case (4): { userstat = "[Snooze]";  break; }
      };
      usrimg.create_image("/home/Syuu/Downloads/usravatar.jpg", 32, 32);
      user_fetch_state = 1;
    } else {
      user_fetch_state = -1;
    }
  }
  ImGui::PopFont();
}

void internal::visual::sscv_create_home_profile(void) {
  internal::graphics::glfw_window_properties_t glfw_window_properties = 
  internal::graphics::glfw_window_export_properties();
  ImGui::SetNextWindowSize(ImVec2(
    glfw_window_properties.window_display_width,
    glfw_window_properties.window_display_height
  ));
  ImGui::SetNextWindowPos(ImVec2(0,0));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  if (window_state) {
    ImGui::Begin("SSCVMain", nullptr,
      ImGuiWindowFlags_NoMove     |
      ImGuiWindowFlags_NoResize   |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoBringToFrontOnFocus
    );
    sscv_create_home_components_multi();
    ImGui::PopStyleVar(2);
    ImGui::End();
  }
}
