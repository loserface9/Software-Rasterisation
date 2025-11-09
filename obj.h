#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include "matrices.h"

typedef struct ObjFace {
    int *vertices;
    int *vertex_textures;
    int *vertex_normals;
    int num_vertices;
} ObjFace;

extern const ObjFace ObjFace_NULL;

typedef struct Obj {
    Vec3 *vertices;
    Vec3 *vertex_textures;
    Vec3 *vertex_normals;
    ObjFace *faces;
    int num_v, num_vt, num_vn, num_f;
} Obj;

extern const Obj Obj_NULL;


Obj import_obj (const char *filepath);

#endif
