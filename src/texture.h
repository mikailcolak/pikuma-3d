#ifndef PK_TEXTURE_H
#define PK_TEXTURE_H

#include <stdint.h>
#include "upng.h"

typedef struct {
    float u;
    float v;
} tex2_t;

extern uint32_t* mesh_texture;
extern upng_t* png_texture;

extern int texture_width;
extern int texture_height;

void load_png_texture_data(const char* filename);

tex2_t tex2_clone(tex2_t* t);

extern const uint8_t REDBRICK_TEXTURE[];
#endif // PK_TEXTURE_H