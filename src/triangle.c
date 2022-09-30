#include "triangle.h"
#include "array.h"
#include "display.h"
#include "light.h"
#include "swap.h"
#include "texture.h"
#include "vector.h"

/*************************************/
/* Return the barycentric weights    */
/* alpha, beta, and gamma for point p*/
/*************************************/
/*                                   */
/*          A                        */
/*         /|\                       */
/*        / | \                      */
/*       /  |  \                     */
/*      /  (p)  \                    */
/*     /  /   \  \                   */
/*    / /       \ \                  */
/*   B-------------C                 */
/*                                   */
/*************************************/
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    // Find the vectors between the vertices ABC and point p
    vec2_t ab = vec2_sub(b, a);
    vec2_t bc = vec2_sub(c, b);
    vec2_t ac = vec2_sub(c, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t bp = vec2_sub(p, b);

    // Calcualte the area of the full triangle ABC using cross product (area of parallelogram)
    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

    // Weight alpha is the area of subtriangle BCP divided by the area of the full triangle ABC
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

    // Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

    // Weight gamma is easily found since barycentric cooordinates always add up to 1
    float gamma = 1 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0-1, y0, x1-1, y1, color);
    draw_line(x1-1, y1, x2-1, y2, color);
    draw_line(x2-1, y2, x0-1, y0, color);
}


void draw_texel(
    int x, int y, uint32_t* texture,
    vec2_t point_a, vec2_t point_b, vec2_t point_c,
    float u0, float v0, float u1, float v1, float u2, float v2
) {
    vec2_t point_p = { x, y };
    vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u = u0 * alpha  +  u1 * beta  +  u2 * gamma;
    float interpolated_v = v0 * alpha  +  v1 * beta  +  v2 * gamma;

    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    draw_pixel(x, y, texture[tex_y * texture_width + tex_x]);

}

void draw_triangle_pixel(
    int x, int y, uint32_t color,
    vec4_t point_a, vec4_t point_b, vec4_t point_c
) {
    // Create three vec2 to find the interpolation
    vec2_t p = { x, y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    
    // Interpolate the value of 1/w for the current pixel
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < z_buffer[(win_width * y) + x]) {
        // Draw a pixel at position (x,y) with a solid color
        draw_pixel(x, y, color);

        // Update the z-buffer value with the 1/w of this current pixel
        z_buffer[(win_width * y) + x] = interpolated_reciprocal_w;
    }
}

void draw_texel_perspective_correct(
    int x, int y, uint32_t* texture, float light_intensity,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
) {
    vec2_t p = { x, y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V, and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Now we can divide back both interpolated values by 1/w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    int zindex = (win_width * y + x) % (win_width * win_height);
    if (interpolated_reciprocal_w < z_buffer[zindex]) {
        int color_index = (texture_width * tex_y) + tex_x;
        draw_pixel(x, y, update_color_intensity(texture[color_index], light_intensity));
        // Update the z-buffer value with the 1/w of this current pixel
        z_buffer[zindex] = interpolated_reciprocal_w;
    }

}

/*************************************/
/*          (x0,y0)                  */
/*            / \                    */
/*           /   \                   */
/*          /     \                  */
/*         /       \                 */
/*        /         \                */
/*   (x1,y1)---------\(x2, y2)       */
/*************************************/
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {

    // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    // Start x_start and x_end from the top vertex (x0, y0);
    float x_start = x0;
    float x_end = x0;

    // Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; ++y) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }

}

/*************************************/
/*   (x0,y0)---------\(x1, x2)       */
/*       \_           \              */
/*          \_         \             */
/*             \_       \            */
/*                \_     \           */
/*                   \    \          */
/*                     \_  \         */
/*                        \_\        */
/*                           \       */
/*                         (x2,y2)   */
/*************************************/
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {

    // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    // Start x_start and x_end from the bottom vertex (x2, y2);
    float x_start = x2;
    float x_end = x2;

    // Loop all the scanlines from bottom to top
    for (int y = y2; y >= y0; --y) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }

}

/*************************************/
/*          (x0,y0)                  */
/*            / \                    */
/*           /   \                   */
/*          /     \                  */
/*         /       \                 */
/*        /         \                */
/*   (x1,y1)---------\(mx, my)       */
/*       \_           \              */
/*          \_         \             */
/*             \_       \            */
/*                \_     \           */
/*                   \    \          */
/*                     \_  \         */
/*                        \_\        */
/*                           \       */
/*                         (x2,y2)   */
/*************************************/
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2) {
        // We can simpoly draw the flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
        return;
    }

    if (y0 == y1) {
        // We can simpoly draw the flat-top triangle
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
        return;
    }

    // Calculate the new vertex (mx, my) using triangle similarity
    int my = y1;
    int mx = (float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;
    
    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);

    // Draw flat-top triangle
    fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);

}

void draw_filled_triangle_with_z_buffer_hacky(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t color,
    float light_intensity
) {
    int tw = texture_width;
    int th = texture_height;
    texture_width = 1;
    texture_height = 1;
    draw_textured_triangle(x0, y0, z0, w0, u0, v0, x1, y1, z1, w1, u1, v1, x2, y2, z2, w2, u2, v2, &color, light_intensity);
    texture_width = tw;
    texture_height = th;
}

void draw_filled_triangle_with_z(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color,
    float light_intensity
) {

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    // New triangle points after sorting them
    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    /*******************************************************/
    /* Render the upper part of the triangle (float-bottom)*/
    /*******************************************************/
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; ++y) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // Swap if x_start is to the right of the x_end
            }

            for (int x = x_start; x < x_end; ++x) {
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }

    /*******************************************************/
    /* Render the bottom part of the triangle (float-top)  */
    /*******************************************************/
    inv_slope_1 = 0;
    inv_slope_2 = 0;
    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; ++y) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // Swap if x_start is to the right of the x_end
            }

            for (int x = x_start; x < x_end; ++x) {
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }
}

/*************************************/
/*                                   */
/*        v0                         */
/*        /\                         */
/*       /  \                        */
/*      /    \                       */
/*     /      \                      */
/*   v1--------\                     */
/*     \_       \                    */
/*        \_     \                   */
/*           \_   \                  */
/*              \_ \                 */
/*                 \\                */
/*                   \               */
/*                    v2             */
/*                                   */
/*************************************/
void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture,
    float light_intensity
) {

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // Fliped the V component to account for inverted UV coordinates (V grows downwards)
    v0 = 1 - v0;
    v1 = 1 - v1;
    v2 = 1 - v2;

    // New triangle points after sorting them
    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    // UV
    tex2_t uv_a = { u0, v0 };
    tex2_t uv_b = { u1, v1 };
    tex2_t uv_c = { u2, v2 };
   
    /*******************************************************/
    /* Render the upper part of the triangle (float-bottom)*/
    /*******************************************************/
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; ++y) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // Swap if x_start is to the right of the x_end
            }

            for (int x = x_start; x < x_end; ++x) {
                draw_texel_perspective_correct(
                    x, y, texture, light_intensity,
                    point_a, point_b, point_c,
                    uv_a, uv_b, uv_c
                );
            }
        }
    }

    /*******************************************************/
    /* Render the bottom part of the triangle (float-top)  */
    /*******************************************************/
    inv_slope_1 = 0;
    inv_slope_2 = 0;
    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; ++y) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // Swap if x_start is to the right of the x_end
            }

            for (int x = x_start; x < x_end; ++x) {
                draw_texel_perspective_correct(
                    x, y, texture, light_intensity,
                    point_a, point_b, point_c,
                    uv_a, uv_b, uv_c
                );
            }
        }
    }


}
