#include "display.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "array.h"
#include "config.h"
#include "light.h"
#include "upng.h"

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
size_t num_triangles_to_render = 0;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };
mat4_t proj_matrix;

bool is_running = false;
uint64_t previous_frame_time = 0;

bool setup() {
    color_buffer = (uint32_t*)calloc(win_width * win_height, sizeof(uint32_t));;
    if (!color_buffer) {
        fprintf(stderr, "<!> Could not allocate the color buffer.\n");
        return false;
    }

    z_buffer = (float*)calloc(win_width * win_height, sizeof(float));
    if (!z_buffer) {
        fprintf(stderr, "<!> Could not allocate the z-buffer.\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        win_width,
        win_height
    );
    if (!color_buffer_texture) {
        fprintf(stderr, "<!> Could not allocate the color buffer texture.\n");
        return false;
    }

    float fov = M_PI / 3.0f;
    float aspect = (float)win_height / (float)win_width;
    float znear = 0.1f;
    float zfar = 100.0f;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // Load texture (manually)
    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;

    // Load the mesh
    //load_cube_mesh_data();
    //load_obj_file_data("./assets/models/f22.obj");
    //load_obj_file_data("./assets/models/cube.obj");
    //load_png_texture_data("./assets/models/cube.png");

    load_next_obj_file_data();
    return true;
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        is_running = false;
        return;
    }

    if (event.type != SDL_KEYDOWN) return;

    // * Pressing “1” toggle vertex point drawing
    if (event.key.keysym.sym == SDLK_1) {
        toggle_vertex_point();
    }

    // * Pressing “2” toggle wireframe drawing
    if (event.key.keysym.sym == SDLK_2) {
        toggle_wireframe();
    }

    // * Pressing “3” toggle solid rendering
    if (event.key.keysym.sym == SDLK_3) {
        toggle_solid();
    }

    // * Pressing “3” toggle solid rendering
    if (event.key.keysym.sym == SDLK_4) {
        toggle_textured();
    }

    // * Pressing “4” toggle back-face culling
    if (event.key.keysym.sym == SDLK_c) {
        toggle_backface_culling();
    }

    // * Pressing “4” toggle back-face culling
    if (event.key.keysym.sym == SDLK_RIGHT) {
        mesh.rotation.z += 0.01;
    }
    if (event.key.keysym.sym == SDLK_LEFT) {
        mesh.rotation.z -= 0.01;
    }
    if (event.key.keysym.sym == SDLK_UP) {
        mesh.rotation.x += 0.01;
    }
    if (event.key.keysym.sym == SDLK_DOWN) {
        mesh.rotation.x -= 0.01;
    }
    if (event.key.keysym.sym == SDLK_a) {
        mesh.rotation.y += 0.01;
    }
    if (event.key.keysym.sym == SDLK_d) {
        mesh.rotation.y -= 0.01;
    }
    if (event.key.keysym.sym == SDLK_k) {
        load_next_obj_file_data();
    }

}

void update() {

    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    num_triangles_to_render = 0;

    //mesh.rotation.x += 0.01;
    mesh.rotation.y = fmod(mesh.rotation.y + 0.01, M_PI_2 * 4.0f);
    mesh.rotation.z = fmod(mesh.rotation.z + 0.01, M_PI_2 * 4.0f);
    mesh.translation.x = (cos(SDL_GetTicks() / 1000.0f) * 3.0f);
    mesh.translation.z = (sin(SDL_GetTicks() / 1000.0f) * 3.0f) + 10.0f;
    mesh.rotation.z += 0.01;

    //mesh.scale.x = (sin(SDL_GetTicks() / 1000.0f) + 1.0f) / 2.0f;
    //mesh.scale.y = mesh.scale.x;
    //mesh.scale.z = mesh.scale.x;

    //mesh.translation.x += 0.03;
    //if (mesh.translation.x >= 5) {
    //    mesh.translation.x = -5;
    //}
    //mesh.translation.z = 5.0;

    // Create a scale matrix
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    // Create a translation matrix
    mat4_t translation_matrix = mat4_make_translation(
        mesh.translation.x,
        mesh.translation.y,
        mesh.translation.z
    );

    // Create a rotation matrix
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // Loop all triangle faces of the mesh
    for (size_t i = 0; i < array_length(mesh.faces); ++i) {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3];
        // Loop all three vertices of this current face and apply transformations
        for (size_t j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Creating a world matrix (combined transformation matrix)
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Multiply the vertex with the world matrix to calculate the final position in the world/scene
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Store the transformed vertex in the transformed_vertices array
            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling
        vec3_t vec_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
        vec3_t vec_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
        vec3_t vec_c = vec3_from_vec4(transformed_vertices[2]); /* B---C */

        vec3_t vec_ab = vec3_sub(vec_b, vec_a);
        vec3_t vec_ac = vec3_sub(vec_c, vec_a);
        vec3_normalize(&vec_ab);
        vec3_normalize(&vec_ac);

        // Calculate the face normal (according to left handed coordinate system)
        vec3_t face_normal = vec3_cross(vec_ab, vec_ac);
        vec3_normalize(&face_normal);

        // Find the vectpr between a point in the triangle and the camera origin
        vec3_t camera_ray = vec3_sub(camera_position, vec_a);

        // Calculate the dot product of the camera and the camera ray
        float dot_of_the_face_normal_and_the_camera_ray = vec3_dot(face_normal, camera_ray);
        float light_intensity = -vec3_dot(face_normal, sun_light.direction);

        if (is_backface_culling_enabled()) {
            if (dot_of_the_face_normal_and_the_camera_ray < 0) {
                continue;
            }
        }

        // Projection
        vec4_t projected_points[3];
        for (size_t j = 0; j < 3; j++) {
            // Project the current vertex
            projected_points[j] = mat4_mul_vec4_project(proj_matrix,  transformed_vertices[j]);

            // Scale and translate the projected points to the middle of the screen
            projected_points[j].x *= win_width / 2.0f;
            projected_points[j].y *= win_height / 2.0f;

            // Flip the object y to correct the orientation of the object according to screen space coordinates (screen y grows from up to bottom)
            projected_points[j].y *= -1;

            projected_points[j].x += win_width / 2.0f;
            projected_points[j].y += win_height / 2.0f;
        }

        triangle_t projected_triangle = {
            .points = {
                projected_points[0],
                projected_points[1],
                projected_points[2],
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = update_color_intensity(mesh_face.color, light_intensity),
            .light_intensity = light_intensity,
            // .avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0f,
        };

        // Store the projectd triangle in the array of triangles to render
        if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
            triangles_to_render[num_triangles_to_render] = projected_triangle;
            num_triangles_to_render += 1;
        }
    }

    // Sort the triangles to render by their avg_depth (Bubble sort!!! / Painter algorithm)
    // int num_triangles = array_length(triangles_to_render);
    // for (int i = 0; i < num_triangles; ++i) {
    //     for (int j = 0; j < num_triangles; ++j) {
    //         if (triangles_to_render[i].avg_depth > triangles_to_render[j].avg_depth) {
    //             triangle_t temp = triangles_to_render[i];
    //             triangles_to_render[i] = triangles_to_render[j];
    //             triangles_to_render[j] = temp;
    //         }
    //     }
    // }
}

void render() {
    // draw_grid(100, 100);

    for (int i = 0; i < num_triangles_to_render; ++i) {
        triangle_t triangle = triangles_to_render[i];
        if (is_solid_enabled() && !is_textured_enabled()) {
            // draw_filled_triangle(
            //     triangle.points[0].x, triangle.points[0].y,
            //     triangle.points[1].x, triangle.points[1].y,
            //     triangle.points[2].x, triangle.points[2].y,
            //     triangle.color
            // );
            // draw_filled_triangle_with_z_buffer_hacky(
            //     triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
            //     triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
            //     triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
            //     triangle.color,
            //     triangle.light_intensity
            // );
            draw_filled_triangle_with_z(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color,
                triangle.light_intensity
            );
        }

        if (is_textured_enabled()) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture,
                triangle.light_intensity
            );
        }

        if (is_vertex_point_enabled()) {
            for (int j = 0; j < 3; ++j) {
                vec4_t point = triangle.points[j];
                draw_rect(
                    point.x-2,
                    point.y-1,
                    3,
                    3,
                    0xFFFFFF00
                );
            }
        }
        if (is_wireframe_enabled()) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFFFFFFFF
            );
        }

    }

    // draw_line(100, 200, 300,  50, 0xFF22FF22);
    // draw_line(300,  50, 400, 600, 0xFF22FF22);
    // draw_line(400, 600, 100, 200, 0xFF22FF22);
    // draw_line(500, 500, 100, 400, 0xFF22FF22);
    // draw_filled_triangle(300, 100, 50, 400, 500, 600, 0xFFEEEEEE);

    // draw_line(win_width / 2.0, win_height / 2.0, win_width / 2.0, 0, 0xFF00FF00);
    // draw_line(win_width / 2.0, win_height / 2.0, win_width, win_height / 2, 0xFFFF0000);

    num_triangles_to_render = 0;

    render_color_buffer();
    clear_color_buffer(0xFF111111);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

void free_resources() {
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(z_buffer);
    free(color_buffer);
    upng_free(png_texture);
}

int main(int argc, char **argv) {
    const char* welcome_msg = "App started\n";
    const char* goodby_msg = "App ended\n";
    printf("%s\n", welcome_msg);
    is_running = initialize_window();

    if (!is_running) {
        destroy_window();
        return EXIT_FAILURE;
    }

    if (!setup()) {
        destroy_window();
        return EXIT_FAILURE;
    }

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    printf("%s\n", goodby_msg);
    return EXIT_SUCCESS;
}

