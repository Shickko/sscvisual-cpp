#ifndef INTERNAL_H_GRAPHICS_RENDER
#define INTERNAL_H_GRAPHICS_RENDER
#include <GLFW/glfw3.h>

namespace internal::graphics {
  struct image {
    unsigned int texture { 0 };
    bool isLoaded { false };
    int width { 0 };
    int height { 0 };

    bool create_image(const char* path, int width, int height);
  };
}
#endif
