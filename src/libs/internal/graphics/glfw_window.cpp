#include <graphics/glfw_window.h>
#include <cstdio>

namespace internal::graphics {
  static glfw_window_properties_t glfw_window_properties = {
    glfw_window_properties.window_display_width = 800,
    glfw_window_properties.window_display_height = 600
  };
  static const char* glsl_version;
  static const char* sscv_window_name_str = 
    "SSCV Local ImGui Client";
  static void glfw_error_callback(int err, const char* desc) { 
    fprintf(stderr, "[GLFW] Error %d: %s\n", err, desc); 
  }
}

internal::graphics::glfw_window_properties_t 
internal::graphics::glfw_window_export_properties(void) {
  return internal::graphics::glfw_window_properties;
}

const char* internal::graphics::glsl_version_get(void) {
  return glsl_version;
}

GLFWwindow* internal::graphics::glfw_window_create(void) {
  glfwSetErrorCallback(internal::graphics::glfw_error_callback);
  if (!glfwInit()) { return NULL; }
#if defined(IMGUI_IMPL_OPENGL_ES2)
  glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
  GLFWwindow* window = glfwCreateWindow(
    glfw_window_properties.window_display_width,
    glfw_window_properties.window_display_height,
    internal::graphics::sscv_window_name_str, 
    NULL, NULL
  );
  if (window == NULL) { return NULL; }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  return window;
}

void internal::graphics::glfw_window_destroy(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void internal::graphics::glfw_window_refresh(GLFWwindow *window, ImVec4 color) {
  glfwGetFramebufferSize(window, 
    &glfw_window_properties.window_display_width,
    &glfw_window_properties.window_display_height
  );
  glViewport(
    0, 0, 
    glfw_window_properties.window_display_width,
    glfw_window_properties.window_display_height
  );
  glClearColor(
    color.x * color.w,
    color.y * color.w,
    color.z * color.w,
    color.w
  );
  glClear(GL_COLOR_BUFFER_BIT);
}
