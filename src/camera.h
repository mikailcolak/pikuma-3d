#ifndef PK_CAMERA_H
#define PK_CAMERA_H

#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t rotation;
} camera_t;

extern camera_t camera;

#endif // PK_CAMERA_H