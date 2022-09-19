#include "SDL.h"
#include "display.h"
#include "vector.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define N_POINTS 9 * 9 * 9
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];
float fov_factor = 640;
vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

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
        return;
    }


}

vec2_t project(vec3_t point) {
    return (vec2_t){
        (fov_factor * point.x) / point.z,
        (fov_factor * point.y) / point.z
    };
}

void update() {
    for (int i = 0; i < N_POINTS; ++i) {
        vec3_t point = cube_points[i];
        point.z -= camera_position.z;
        projected_points[i] = project(point);
    }
}

void render() {
    draw_grid(100, 100);

    for (int i = 0; i < N_POINTS; ++i) {
        vec2_t pp = projected_points[i];
        draw_rect(pp.x + win_width / 2.0, pp.y + win_height / 2.0, 4, 4, 0xFFFFFF00);
    }

    render_color_buffer();
    clear_color_buffer(0xFF7F3535);

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