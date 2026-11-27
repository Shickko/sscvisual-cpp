#include <graphics/glfw_window.h>
#include <visual/sscv_main.h>
#include <visual/profile.h>
#include <imgui.h>
#include <iostream>
#include <string>
#include <filesystem>

static const char* win_title  { "SSCVInject" };

static void sscv_main_draw(const char* winstr) {
  ImGui::Begin(winstr, nullptr,
    ImGuiWindowFlags_NoMove     |
    ImGuiWindowFlags_NoResize   |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus
  );
  internal::visual::profile::render();
  ImGui::End();
}

void internal::visual::sscv_main_init(void) {
  internal::graphics::glfw_window_properties_t
  glfw_window_properties =
    internal::graphics::glfw_window_export_properties();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.00f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.00f);
  ImGui::SetNextWindowSize(ImVec2(
    glfw_window_properties.window_display_width,
    glfw_window_properties.window_display_height
  ));
  ImGui::SetNextWindowPos(ImVec2(0, 0));

  sscv_main_draw(win_title);

  ImGui::PopStyleVar(2);

}
