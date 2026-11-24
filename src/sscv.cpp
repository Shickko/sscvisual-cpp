#include <graphics/glfw_window.h>
#include <visual/home_profile.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>
#ifdef IMGUI_IMPL_OPENGL_ES2
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>

using namespace internal::visual;

int main(int argc, char* argv[]) {
  GLFWwindow* window = internal::graphics::glfw_window_create();
  if (window == NULL)
    return 1;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(internal::graphics::glsl_version_get());
  
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    /*[[ Main Block Start ]]*/
    sscv_create_home_profile();
    /*[[ Main Block End ]]*/

    ImGui::Render();
    internal::graphics::glfw_window_refresh(window, ImVec4());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  internal::graphics::glfw_window_destroy(window);
  std::cout << "[[SSCV session end >> shutdown called" << std::endl;
  return 0;
}
