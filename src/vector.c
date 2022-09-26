#include "vector.h"

#include <math.h>

// Vector 2D
float vec2_length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t v0, vec2_t v1) {
    return (vec2_t) {
        v0.x + v1.x,
        v0.y + v1.y,
    };
}

vec2_t vec2_sub(vec2_t v0, vec2_t v1) {
    return (vec2_t) {
        v0.x - v1.x,
        v0.y - v1.y,
    };
}

vec2_t vec2_mul(vec2_t v, float s) {
    return (vec2_t) {
        v.x * s,
        v.y * s,
    };
}

vec2_t vec2_div(vec2_t v, float s) {
    if (s == 0.0f) return (vec2_t){ 0, 0 };

    return (vec2_t) {
        v.x / s,
        v.y / s,
    };
}

float vec2_dot(vec2_t v0, vec2_t v1) {
    return v0.x * v1.x
         + v0.y * v1.y;
}

void vec2_normalize(vec2_t* v) {
    float len = vec2_length(*v);
    v->x /= len;
    v->y /= len;
}

vec2_t vec2_normalized(vec2_t v) {
    vec2_normalize(&v);
    return v;
}


// Vector 3D
float vec3_length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t v0, vec3_t v1) {
    return (vec3_t) {
        v0.x + v1.x,
        v0.y + v1.y,
        v0.z + v1.z,
    };
}

vec3_t vec3_sub(vec3_t v0, vec3_t v1) {
    return (vec3_t) {
        v0.x - v1.x,
        v0.y - v1.y,
        v0.z - v1.z,
    };
}

vec3_t vec3_mul(vec3_t v, float s) {
    return (vec3_t) {
        v.x * s,
        v.y * s,
        v.z * s,
    };
}

vec3_t vec3_div(vec3_t v, float s) {
    if (s == 0) return (vec3_t){ 0, 0, 0 };

    return (vec3_t) {
        v.x / s,
        v.y / s,
        v.z / s,
    };
}

vec3_t vec3_cross(vec3_t v0, vec3_t v1) {
    return (vec3_t) {
        v0.y * v1.z - v0.z * v1.y,
        v0.z * v1.x - v0.x * v1.z,
        v0.x * v1.y - v0.y * v1.x,
    };
}

float vec3_dot(vec3_t v0, vec3_t v1) {
    return v0.x * v1.x
         + v0.y * v1.y
         + v0.z * v1.z;
}

void vec3_normalize(vec3_t* v) {
    float len = vec3_length(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

vec3_t vec3_normalized(vec3_t v) {
    vec3_normalize(&v);
    return v;
}

vec3_t vec3_t_rotate_x(vec3_t v, float angle){
    return (vec3_t) {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
}

vec3_t vec3_t_rotate_y(vec3_t v, float angle){
    return (vec3_t) {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
}

vec3_t vec3_t_rotate_z(vec3_t v, float angle){
    return (vec3_t) {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
}

// Vector 4D
vec4_t vec4_from_vec3(vec3_t v) {
    return (vec4_t){
        v.x,
        v.y,
        v.z,
        1,
    };
}

vec3_t vec3_from_vec4(vec4_t v) {
    return (vec3_t){
        v.x,
        v.y,
        v.z,
    };
}

