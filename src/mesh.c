#include "mesh.h"
#include "array.h"
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
    { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .color = 0xFFFF0000 },
    { .a = 1, .b = 3, .c = 4, .color = 0xFFFF0000 },

    // right
    { .a = 4, .b = 3, .c = 5, .color = 0xFF00FF00 },
    { .a = 4, .b = 5, .c = 6, .color = 0xFF00FF00 },

    // back
    { .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF },
    { .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF },

    // left
    { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00 },
    { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00 },

    // top
    { .a = 2, .b = 7, .c = 5, .color = 0xFF00FFFF },
    { .a = 2, .b = 5, .c = 3, .color = 0xFF00FFFF },

    // bottom
    { .a = 6, .b = 8, .c = 1, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .color = 0xFFFFFFFF },
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
    char buffer[buffer_length];
    size_t read_len;
    while (fgets(buffer, buffer_length, fd)) {

        // Vertices
        if (strncmp(buffer, "v ", 2) == 0) {
            vec3_t vertex = {};
            sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
            continue;
        }

        // Faces
        if (strncmp(buffer, "f ", 2) == 0) {
            face_t face = {
                .color = 0xFFEEEEEE
            };
            int _;
            sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &face.a, &_, &_, &face.b, &_, &_, &face.c, &_, &_);
            array_push(mesh.faces, face);
            continue;
        }
    }
}

const char* model_paths[] = {
    "colored_cube",
    "crab",
    "cube",
    "drone",
    "efa",
    "f117",
    "f22",
    "flat_vase",
    //"minicooper",
    "quad",
    "runway",
    "smooth_vase",
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
    ++current_model_index;
}
