#include "clipping.h"

#include <math.h>

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

/********************************************************************/
/* Near plane   :  P=(0, 0, znear), N=(0, 0,  1)                    */
/* Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)                    */
/* Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))  */
/* Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))   */
/* Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))   */
/* Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))  */
/********************************************************************/
/*                                                                  */
/*           /|\                                                    */
/*         /  | |                                                   */
/*       /\   | |                                                   */
/*     /      | |                                                   */
/*  P*|-->  <-|*|   ----> +z-axis                                   */
/*     \      | |                                                   */
/*       \/   | |                                                   */
/*         \  | |                                                   */
/*           \|/                                                    */
/*                                                                  */
/********************************************************************/
void init_frustum_planes(float fovx, float fovy, float z_near, float z_far) {
    float cos_half_fovx = cos(fovx / 2);
    float sin_half_fovx = sin(fovx / 2);
    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovy = sin(fovy / 2);

    const vec3_t VEC3_ZERO = { 0, 0, 0 };

    frustum_planes[LEFT_FRUSTUM_PLANE].point      = VEC3_ZERO;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x   = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y   = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z   = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point     = VEC3_ZERO;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x  = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y  = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z  = sin_half_fovx;

    frustum_planes[TOP_FRUSTUM_PLANE].point       = VEC3_ZERO;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x    = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y    = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z    = sin_half_fovy;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point    = VEC3_ZERO;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[NEAR_FRUSTUM_PLANE].point      = (vec3_t){ 0, 0, z_near };
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x   = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y   = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z   = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point       = (vec3_t){ 0, 0, z_far };
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x    = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y    = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z    = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2) {
    return (polygon_t) {
        .vertices = { v0, v1, v2 },
        .texcoords = { t0, t1, t2 },
        .num_vertices = 3,
    };
}

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertices[MAX_NUM_POLYGON_VERTICES];
    tex2_t inside_texcoords[MAX_NUM_POLYGON_VERTICES];
    int num_inside = 0;

    vec3_t* current_vertex = &polygon->vertices[0];
    tex2_t* current_texcoord = &polygon->texcoords[0];

    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t* previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

    float current_dot = 0;
    float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

    while (current_vertex != &polygon->vertices[polygon->num_vertices]) {

        current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

        // If vertex changed from inside to outside or wise versa then we are looking for an intersection point to add
        if (current_dot * previous_dot < 0) {

            // Find the `t` (inverse lerp)
            float t = previous_dot / (previous_dot - current_dot);

            // Find the intersection point with the `t` (lerp) for vertex
            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t),
            };

            // Find the intersection point with the `t` (lerp) for texture coordinate
            tex2_t interpolated_texcoord = {
                .u = float_lerp(previous_texcoord->u, current_texcoord->u, t),
                .v = float_lerp(previous_texcoord->v, current_texcoord->v, t),
            };

            // Insert the intersection vertex into the list of "inside"
            inside_vertices[num_inside] = vec3_clone(&intersection_point);
            inside_texcoords[num_inside] = tex2_clone(&interpolated_texcoord);
            ++num_inside;
        }

        // If current point is inside the plane
        if (current_dot > 0) {
            // Insert the current vertex into the list of "inside"
            inside_vertices[num_inside] = vec3_clone(current_vertex);
            inside_texcoords[num_inside] = tex2_clone(current_texcoord);
            ++num_inside;
        }

        // Iterate to the next
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        current_vertex++;
        current_texcoord++;
    }
    
    // Copy the newly calculated list of vertices to the &polygon
    polygon->num_vertices = num_inside;
    for (int i = 0; i < num_inside; ++i) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }
}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles) {
    if (polygon->num_vertices == 0) {
        *num_triangles = 0;
        return;
    }
    for (size_t i = 0; i < polygon->num_vertices - 2; ++i) {
        size_t idx0 = 0;
        size_t idx1 = i + 1;
        size_t idx2 = i + 2;
        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[idx0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[idx1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[idx2]);

        triangles[i].texcoords[0] = polygon->texcoords[idx0];
        triangles[i].texcoords[1] = polygon->texcoords[idx1];
        triangles[i].texcoords[2] = polygon->texcoords[idx2];
    }
    *num_triangles = polygon->num_vertices - 2;
}