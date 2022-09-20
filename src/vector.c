#include "vector.h"

#include <math.h>

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