#include "SDL.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;

int win_width = 800;
int win_height = 600;
int WINDOW_INDEX = 1;

bool initialize_window() {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL could not be initialized!\n");
        return false;
    }

    // Get full-screen dimensions
    // SDL_DisplayMode display_mode;
    // if (SDL_GetCurrentDisplayMode(WINDOW_INDEX, &display_mode) == 0) {
    //     width = display_mode.w;
    //     height = display_mode.h;
    //     fprintf(stderr, "Resolution obtained: %dx%d.", width, height);
    // } else {
    //     fprintf(stderr, "WARNING: Could not obtain display resolution %dx%d will be used as resolution.", width, height);
    // }

    // Initialize SDL window
    window = SDL_CreateWindow(
        "Pikuma 3D",
        SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_INDEX),
        SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_INDEX),
        win_width,
        win_height,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "<!> Could not create window.\n");
        return false;
    }

    // Initialize SDL Renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "<!> Could not create renderer.\n");
        return false;
    }

    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x >= win_width || y >= win_height) {
        fprintf(stderr, "Can not set pixel at (x=%d, y=%d)\n", x, y);
        return;
    }
    color_buffer[y * win_width + x] = color;
}

void draw_grid(int cell_width, int cell_height) {
    if (color_buffer == NULL) {
        return;
    }
    for (size_t i = 0; i < win_width * win_height; ++i) {
        int x = i % win_width;
        int y = i / win_width;
        if (x % cell_width == 0 || y % cell_height == 0) {
            uint32_t a = (0xFF000000);
            uint32_t r = (uint32_t)(0x000000FF * ((x / (float)win_width))) << 16;
            uint32_t g = (uint32_t)(0x000000FF * (y / (float)win_height)) << 8;
            uint32_t b = (0x00000000);
            color_buffer[i] = a | r | g | b;
        }
    } 
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    if (color_buffer == NULL) {
        return;
    }

    int w = (int)fmin(x+width, win_width);
    int h = (int)fmin(y+height, win_height);

    for (int u = x; u < w; ++u) {
        for (int v = y; v < h; ++v) {
            color_buffer[v * win_width + u] = color;
        }
    }
}

void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(win_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    if (color_buffer == NULL) {
        return;
    }
    for (size_t i = 0; i < win_width * win_height; ++i) {
        color_buffer[i] = color;

        int x = i % win_width;
        int y = i / win_width;
        if (x % 10 == 0 || y % 10 == 0) {
            color_buffer[i] = 0xFF333333;
        }
    } 
}

void cleanup() {
    SDL_DestroyTexture(color_buffer_texture);
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}