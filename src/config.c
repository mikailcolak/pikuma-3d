#include "config.h"
#include <stdint.h>

draw_config_t draw_config = D_WIREFRAME
                          | D_VERTEX_POINT
                          | D_SOLID
                          | D_BACK_FACE_CULLED
                          ;

void enable_wireframe()            { draw_config |= D_WIREFRAME;                                      }
void enable_vertex_point()         { draw_config |= D_VERTEX_POINT;                                   }
void enable_solid()                { draw_config |= D_SOLID; disable_textured();                      }
void enable_textured()             { draw_config |= D_TEXTURED; disable_solid();                      }
void enable_backface_culling()     { draw_config |= D_BACK_FACE_CULLED;                               }
void disable_wireframe()           { draw_config &= ~D_WIREFRAME;                                     }
void disable_vertex_point()        { draw_config &= ~D_VERTEX_POINT;                                  }
void disable_solid()               { draw_config &= ~D_SOLID;                                         }
void disable_textured()            { draw_config &= ~D_TEXTURED;                                      }
void disable_backface_culling()    { draw_config &= ~D_BACK_FACE_CULLED;                              }
void toggle_wireframe()            { draw_config ^= D_WIREFRAME;                                      }
void toggle_vertex_point()         { draw_config ^= D_VERTEX_POINT;                                   }
void toggle_solid()                { draw_config ^= D_SOLID; disable_textured();                      }
void toggle_textured()             { draw_config ^= D_TEXTURED; disable_solid();                      }
void toggle_backface_culling()     { draw_config ^= D_BACK_FACE_CULLED;                               }
bool is_wireframe_enabled()        { return (draw_config & D_WIREFRAME) == D_WIREFRAME;               }
bool is_vertex_point_enabled()     { return (draw_config & D_VERTEX_POINT) == D_VERTEX_POINT;         }
bool is_solid_enabled()            { return (draw_config & D_SOLID) == D_SOLID;                       }
bool is_textured_enabled()         { return (draw_config & D_TEXTURED) == D_TEXTURED;                 }
bool is_backface_culling_enabled() { return (draw_config & D_BACK_FACE_CULLED) == D_BACK_FACE_CULLED; }