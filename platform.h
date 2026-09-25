#pragma once

#include "include/SDL2/SDL.h"
// #include <SDL2/SDL.h>
#include "include/glad/glad.h"
#include <cstdint>

class Platform {
public:
  Platform(char const *title, int window_width, int window_height,
           int texture_width, int texture_height);
  ~Platform();
  void Update(void const *buffer, int pitch);
  bool ProcessInput(uint8_t *keys);

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_GLContext gl_context;
  GLuint framebuffer_texture;
};
