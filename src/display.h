#ifndef PK_DISPLAY_H
#define PK_DISPLAY_H

#include "SDL.h"

#include <stdbool.h>
#include <stdint.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;
extern uint32_t* color_buffer;

extern int win_width;
extern int win_height;
extern int WINDOW_INDEX;

bool initialize_window();
void draw_pixel(int x, int y, uint32_t color);
void draw_grid(int cell_width, int cell_height);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer();
void clear_color_buffer(uint32_t color);
void cleanup();

#endif // PK_DISPLAY_H