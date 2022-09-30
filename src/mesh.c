#include "mesh.h"
#include "array.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { .x = 0 , .y = 0 , .z = 0 },
    .scale = { .x = 1, .y = 1, .z = 1 },
    .translation = { .x = 0, .y = 0, .z = 0 },
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }, // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF }
};

void load_cube_mesh_data() {
    array_free(mesh.vertices); mesh.vertices = NULL;
    array_free(mesh.faces); mesh.faces = NULL;

    for (int i = 0; i < N_CUBE_VERTICES; ++i) {
        array_push(mesh.vertices, cube_vertices[i]);
    }
    for (int i = 0; i < N_CUBE_FACES; ++i) {
        array_push(mesh.faces, cube_faces[i]);
    }
}

void load_obj_file_data(const char* path) {
    FILE* fd;
    fd = fopen(path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Could not open the file '%s'", path);
    }

    array_free(mesh.vertices); mesh.vertices = NULL;
    array_free(mesh.faces); mesh.faces = NULL;

    const size_t buffer_length = 1024;
    char buffer[1024];
    size_t read_len;

    tex2_t* tex_coords = NULL;
    while (fgets(buffer, buffer_length, fd)) {

        // Vertices
        if (strncmp(buffer, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
            continue;
        }

        // Texture coordinates
        if (strncmp(buffer, "vt ", 3) == 0) {
            tex2_t tex_coord = { .u = 0, .v = 0 };
            sscanf(buffer, "vt %f %f", &tex_coord.u, &tex_coord.v);
            array_push(tex_coords, tex_coord);
            continue;
        }

        // Faces
        if (strncmp(buffer, "f ", 2) == 0) {
            int _;
            int face_index_a, face_index_b, face_index_c;
            int tex_index_a, tex_index_b, tex_index_c;
            sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                &face_index_a, &tex_index_a, &_,
                &face_index_b, &tex_index_b, &_,
                &face_index_c, &tex_index_c, &_
            );
            face_t face = {
                .color = 0xFFEEEEEE,
            
                .a = face_index_a - 1,
                .b = face_index_b - 1,
                .c = face_index_c - 1,

                .a_uv = tex_coords[tex_index_a - 1],
                .b_uv = tex_coords[tex_index_b - 1],
                .c_uv = tex_coords[tex_index_c - 1],
            };
            array_push(mesh.faces, face);
            continue;
        }
    }
    array_free(tex_coords);
}

const char* model_paths[] = {
    "cube",
    "crab",
    "drone",
    "efa",
    "f117",
    "f22",
    "flat_vase",
    // "minicooper",
    "quad",
    //"runway",
    //"smooth_vase",
    "sphere",
    NULL
};
int  current_model_index = 0;
void load_next_obj_file_data() {
    if (model_paths[current_model_index] == NULL) {
        current_model_index = 0;
    }
    char file_path[1024];
    sprintf(file_path, "./assets/models/%s.obj", model_paths[current_model_index]);
    fprintf(stdout, "Model loading from: %s\n", file_path);
    load_obj_file_data(file_path);

    sprintf(file_path, "./assets/models/%s.png", model_paths[current_model_index]);
    load_png_texture_data(file_path);
    ++current_model_index;
}
