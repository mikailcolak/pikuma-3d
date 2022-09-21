#ifndef PIKUMA_VECTOR_H
#define PIKUMA_VECTOR_H

// Vector 2D functions
typedef struct { float x, y; } vec2_t;
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t v0, vec2_t v1);
vec2_t vec2_sub(vec2_t v0, vec2_t v1);
vec2_t vec2_mul(vec2_t v, float s);
vec2_t vec2_div(vec2_t v, float s);
float vec2_dot(vec2_t v0, vec2_t v1);

// Vector 3D
typedef struct { float x, y, z; } vec3_t;
float vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t v0, vec3_t v1);
vec3_t vec3_sub(vec3_t v0, vec3_t v1);
vec3_t vec3_mul(vec3_t v, float s);
vec3_t vec3_div(vec3_t v, float s);
vec3_t vec3_cross(vec3_t v0, vec3_t v1);
float vec3_dot(vec3_t v0, vec3_t v1);

vec3_t vec3_t_rotate_x(vec3_t v, float angle);
vec3_t vec3_t_rotate_y(vec3_t v, float angle);
vec3_t vec3_t_rotate_z(vec3_t v, float angle);

#endif //PIKUMA_VECTOR_H