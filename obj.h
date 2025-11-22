#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include "matrices.h"

typedef struct Face {
    // Array of pointers to Vec3s
    const Vec3 **vertices;
    const Vec3 **vertex_textures;
    const Vec3 **vertex_normals;
    int num_vertices;
} Face;

extern const Face ObjFace_NULL;

typedef struct Obj {
    Vec3 *vertices;
    Vec3 *vertex_textures;
    Vec3 *vertex_normals;
    Face *faces;
    int num_v, num_vt, num_vn, num_f;
} Obj;

extern const Obj Obj_NULL;


Obj import_obj (const char *filepath);

#endif
