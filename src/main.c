#include "SDL.h"
#include "display.h"
#include "vector.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define N_POINTS 9 * 9 * 9
vec3_t cube_points[N_POINTS];

bool is_running = false;

bool setup() {
    color_buffer = (uint32_t*)calloc(win_width * win_height, sizeof(uint32_t));;
    if (!color_buffer) {
        fprintf(stderr, "<!> Could not allocate the color buffer.\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        win_width,
        win_height
    );
    if (!color_buffer_texture) {
        fprintf(stderr, "<!> Could not allocate the color buffer texture.\n");
        return false;
    }

    // Start loading my array of vectors
    // From -1 to 1 (in this 9x9x9 cube)
    int point_i = -1;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                cube_points[++point_i] = (vec3_t){ .x = x, .y = y, .z = z };
            }
        }
    }

    return true;
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        is_running = false;
    }
}

void update() {

}

void render() {
    SDL_SetRenderDrawColor(renderer, 127, 40, 40, 255);
    SDL_RenderClear(renderer);

    render_color_buffer();
    clear_color_buffer(0xFF7F3535);
    draw_grid(100, 100);
    draw_rect(700, 500, 50, 50, 0xFF00F7F7);
    draw_pixel(0, 0, 0xffffffff);
    draw_pixel(win_width-1, 0, 0xffffffff);
    draw_pixel(win_width-1, win_height-1, 0xffffffff);
    draw_pixel(0, win_height-1, 0xffffffff);
    draw_pixel(win_height, win_height, 0xffffffff);


    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
    const char* welcome_msg = "App started\n";
    const char* goodby_msg = "App ended\n";
    printf("%s\n", welcome_msg);
    is_running = initialize_window();

    if (!is_running) {
        cleanup();
        return EXIT_FAILURE;
    }

    if (!setup()) {
        cleanup();
        return EXIT_FAILURE;
    }

    while (is_running) {
        process_input();
        update();
        render();
    }

    cleanup();
    printf("%s\n", goodby_msg);
    return EXIT_SUCCESS;
}