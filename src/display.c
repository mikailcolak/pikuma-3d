#include <SDL.h>

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
float* z_buffer = NULL;

const int win_width = 1280;
const int win_height = 720;
int WINDOW_INDEX = 0;

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
    if (x < 0 || y < 0 || x >= win_width || y >= win_height) {
        //fprintf(stderr, "Can not set pixel at (x=%d, y=%d)\n", x, y);
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
            draw_pixel(u, v, color);
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    // Find how much we should increment in both x and y each step
    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; ++i) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
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
    for (size_t x = 0; x < win_width; ++x) {
        for (size_t y = 0; y < win_height; ++y) {
            size_t i = y * win_width + x;
            color_buffer[i] = color;
            if (x % 10 == 0 || y % 10 == 0) {
                color_buffer[i] = 0xFF333333;
            }
        }
    } 
}

void clear_z_buffer() {
    if (z_buffer == NULL) {
        return;
    }
    for (size_t i = 0; i < win_width * win_height; ++i) {
        z_buffer[i] = 1.0;
    } 
}

void destroy_window() {
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
