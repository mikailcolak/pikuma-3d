#ifndef PIKUMA_VECTOR_H
#define PIKUMA_VECTOR_H

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

vec3_t vec3_t_rotate_x(vec3_t v, float angle);
vec3_t vec3_t_rotate_y(vec3_t v, float angle);
vec3_t vec3_t_rotate_z(vec3_t v, float angle);

#endif //PIKUMA_VECTOR_H