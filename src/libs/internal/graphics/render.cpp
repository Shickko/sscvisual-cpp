#define STB_IMAGE_IMPLEMENTATION
#include <graphics/render.h>
#include <stb_image.h>
#include <imgui.h>
#include <cstdio>

static bool image_loadm(internal::graphics::image* img, void* data, size_t size) {
  int w = 0, h = 0;
  unsigned char* pixels = stbi_load_from_memory(
    (const unsigned char*)data, (int)size, &w, &h, NULL, 4
  );
  if (pixels == NULL) { return false; }

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  stbi_image_free(pixels);

  img->texture = tex;
  img->width = w;
  img->height = h;
  img->isLoaded = true;
  return true;
}

static bool image_loadf(internal::graphics::image* img, const char* path, int width, int height) {
  if (img == NULL) { return false; }

  img->width = width;
  img->height = height;

  FILE* f = fopen(path, "rb");
  if (f == NULL) { 
    return false; 
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (size == -1) { 
    fclose(f); 
    return false; 
  }
  fseek(f, 0, SEEK_SET);
  void* buf = ImGui::MemAlloc((size_t)size);
  if (buf == NULL) {
    fclose(f);
    return false;
  }

  size_t read_bytes = fread(buf, 1, (size_t)size, f);
  fclose(f);

  if (read_bytes != (size_t)size) {
    ImGui::MemFree(buf);
    return false;
  }


  bool ok = image_loadm(img, buf, (size_t)size);
  ImGui::MemFree(buf);
  return ok;
}

bool internal::graphics::image::create_image(const char* path, int width, int height) {
  return image_loadf(this, path, width, height);
}
