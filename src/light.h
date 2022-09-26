#ifndef PK_LIGHT_H
#define PK_LIGHT_H

#include "vector.h"

#include <stdint.h>

typedef struct {
    vec3_t direction;
} light_t;

extern light_t sun_light;

uint32_t update_color_intensity(uint32_t original_color, float intensity);

#endif // PK_LIGHT_H
