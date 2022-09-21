#include "SDL.h"
#include "display.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

bool is_running = false;
uint64_t previous_frame_time = 0;

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
    
    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();
    
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;


    // Loop all triangle faces of the mesh
    for (size_t i = 0; i < N_MESH_FACES; ++i) {
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];


        triangle_t projected_triangle;
        // Loop all three vertices of this current face and apply transformations
        for (size_t j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_t_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_t_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_t_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate the vertex away from the camera
            transformed_vertex.z -= camera_position.z;


            // Project the current vertex
            vec2_t projected_point = project(transformed_vertex);

            // Scale and translate the projected points to the middle of the screen
            projected_point.x += win_width / 2.0,
            projected_point.y += win_height / 2.0,

            // Store it
            projected_triangle.points[j] = projected_point;
        }

        // Store the projectd triangle in the array of triangles to render
        triangles_to_render[i] = projected_triangle;
    }
}

void render() {
    //draw_grid(100, 100);

    for (int i = 0; i < N_MESH_FACES; ++i) {
        triangle_t triangles = triangles_to_render[i];
        for (int j = 0; j < 3; ++j) {
            vec2_t point = triangles.points[j];
            draw_rect(
                point.x-1,
                point.y-1,
                3,
                3,
                0xFFFFFF00
            );
        }
        draw_triangle(
            triangles.points[0].x,
            triangles.points[0].y,
            triangles.points[1].x,
            triangles.points[1].y,
            triangles.points[2].x,
            triangles.points[2].y,
            0xFFFF00FF
        );

        // draw_line(100, 200, 300,  50, 0xFF22FF22);
        // draw_line(300,  50, 400, 600, 0xFF22FF22);
        // draw_line(400, 600, 100, 200, 0xFF22FF22);
        // draw_line(500, 500, 100, 400, 0xFF22FF22);
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