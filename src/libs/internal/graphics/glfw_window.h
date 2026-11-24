#ifndef INTERNAL_H_GRAPHICS_GLFW_WIN
#define INTERNAL_H_GRAPHICS_GLFW_WIN
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace internal::graphics {
  struct glfw_window_properties_t {
    int window_display_width;
    int window_display_height;
  };
  glfw_window_properties_t
  glfw_window_export_properties(void);

  const char* glsl_version_get(void);
  GLFWwindow* glfw_window_create(void);

  void glfw_window_destroy(GLFWwindow* window);
  void glfw_window_refresh(GLFWwindow* window, ImVec4 color);
}

#endif
