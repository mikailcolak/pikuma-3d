#include <stdbool.h>

typedef enum {
    D_NONE              = 0 << 0,
    D_WIREFRAME         = 1 << 0,
    D_VERTEX_POINT      = 1 << 1,
    D_SOLID             = 1 << 2,
    D_TEXTURED          = 1 << 3,
    D_BACK_FACE_CULLED  = 1 << 4,
} draw_config_t;

extern draw_config_t draw_config;

void enable_wireframe();
void enable_vertex_point();
void enable_solid();
void enable_textured();
void enable_backface_culling();
void disable_wireframe();
void disable_vertex_point();
void disable_solid();
void disable_textured();
void disable_backface_culling();
void toggle_wireframe();
void toggle_vertex_point();
void toggle_solid();
void toggle_textured();
void toggle_backface_culling();
bool is_wireframe_enabled();
bool is_vertex_point_enabled();
bool is_solid_enabled();
bool is_textured_enabled();
bool is_backface_culling_enabled();