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
    .rotation = { .x = 0 , .y = 0 , .z= 0 }
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
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },

    // right
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },

    // back
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },

    // left
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },

    // top
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },

    // bottom
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 },
};

void load_cube_mesh_data() {
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
            face_t face = {};
            int _;
            sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &face.a, &_, &_, &face.b, &_, &_, &face.c, &_, &_);
            array_push(mesh.faces, face);
            continue;
        }
    }
}