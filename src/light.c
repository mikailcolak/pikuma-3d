#include "light.h"

light_t sun_light = {
    .direction = { 0, 0, 1 },
    .position = { 0, 0, 0 }
};

uint32_t update_color_intensity(uint32_t original_color, float intensity) {
    if (intensity < 0) intensity = 0;
    if (intensity > 1) intensity = 1;
    const uint32_t a = ((original_color & 0xFF000000) >> 24) * intensity;
    const uint32_t r = ((original_color & 0x00FF0000) >> 16) * intensity;
    const uint32_t g = ((original_color & 0x0000FF00) >>  8) * intensity;
    const uint32_t b = ((original_color & 0x000000FF) >>  0) * intensity;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

