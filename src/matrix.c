#include "matrix.h"
#include "math.h"

mat4_t mat4_identity() {
    return (mat4_t){{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    }};
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    return (mat4_t){{
        { sx,  0,  0, 0 },
        {  0, sy,  0, 0 },
        {  0,  0, sz, 0 },
        {  0,  0,  0, 1 },
    }};
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    /*
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = tx;
    m.m[2][3] = tz;
    return m;
    */
    return (mat4_t){{
        { 1, 0, 0, tx },
        { 0, 1, 0, ty },
        { 0, 0, 1, tz },
        { 0, 0, 0,  1 },
    }};
}

mat4_t mat4_make_rotation_x(float angle) {
    const float s = sin(angle);
    const float c = cos(angle);
    return (mat4_t){{
        { 1, 0,  0, 0 },
        { 0, c, -s, 0 },
        { 0, s,  c, 0 },
        { 0, 0,  0, 1 },
    }};
}

mat4_t mat4_make_rotation_y(float angle) {
    const float s = sin(angle);
    const float c = cos(angle);
    return (mat4_t){{
        {  c, 0, s, 0 },
        {  0, 1, 0, 0 },
        { -s, 0, c, 0 },
        {  0, 0, 0, 1 },
    }};
}

mat4_t mat4_make_rotation_z(float angle) {
    const float s = sin(angle);
    const float c = cos(angle);
    return (mat4_t){{
        { c, -s, 0, 0 },
        { s,  c, 0, 0 },
        { 0,  0, 1, 0 },
        { 0,  0, 0, 1 },
    }};
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
    float y = 1.0f / tan(fov / 2.0f);
    float x = aspect * y;
    float z = zfar / zfar - znear;
    float w = -znear * zfar / zfar - znear;
    return (mat4_t){{
        { x, 0, 0, 0 },
        { 0, y, 0, 0 },
        { 0, 0, z, w },
        { 0, 0, 1, 0 },
    }};
}

vec4_t mat4_mul_vec4_project(mat4_t m, vec4_t v) {
    vec4_t r = mat4_mul_vec4(m, v);

    if (r.w != 0.0f) {
        r.x /= r.w;
        r.y /= r.w;
        r.z /= r.w;
    }
    return r;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    return (vec4_t){
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
        m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w,
    };
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t m;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
    m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return m;
}

