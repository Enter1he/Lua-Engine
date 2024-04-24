#ifndef BATCH_H
#define BATCH_H

#include "src/Graphics/Graphics.h"


extern const size_t maxVertex;
typedef struct _Batch{
    size_t numObj, freeObj, totalObj;
    GLuint vbo, ubo, vao;
    GLdouble *vertex, *texcoord;
    Drawable *base;
    struct Entity* type;
}Batch;


int LE_LoadBatch(lua_L);

int LE_DrawBatch(lua_L);

int LE_BatchAdd(lua_L);

int LE_BatchShow(lua_L);

int _LE_BatchGC(lua_L);

#endif