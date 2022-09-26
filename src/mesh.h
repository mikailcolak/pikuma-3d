#ifndef PK_MESH_H
#define PK_MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face

typedef struct {
    vec3_t* vertices;
    face_t* faces;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

extern mesh_t mesh;

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];
void load_cube_mesh_data();
void load_obj_file_data(const char* path);

extern const char* model_paths[];
extern int current_model_index;
void load_next_obj_file_data();
#endif // PK_MESH_H
