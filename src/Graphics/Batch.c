
#include "src/Graphics/Batch.h"

const size_t maxVertex = 1000;
#define ObjSize 12*sizeof(GLdouble)
#define NumVertex 12
// proc LoadBatch(self : table(Batch?uninit), base: table(Sprite or Sheet))
int LE_LoadBatch(lua_L){
    if (lua_gettop(L) > 3) luaL_error(L, "%s function takes only 3 arguments!", __func__);
    if (lua_gettop(L) < 2) luaL_error(L, "%s function takes not less than 2 arguments!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    
    size_t initSize = 50;

    if (lua_isinteger(L,3)) initSize = lua_tointeger(L,3);
    
    Batch *b = lua_newuserdata(L, sizeof(Batch));
    lua_setfield(L, 1, "_batch");
    lua_getvalue(L, 2, "_drawable");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    b->base = (Drawable *)lua_touserdata(L, -1);
    
    b->type = &Dyn;
    // if (b->base->method == &LE_DrawSingleSprite){
    //     b->type = &Sta;
    // }else 
    // if (b->base->method == &LE_DrawSpriteSheet){
    //     b->type = &Dyn;
    // }else{
    //     luaL_error(L, "LoadBatch Unknown drawable!");
    // }

    b->numObj = 0;
    b->freeObj = initSize;
    b->totalObj = initSize;
    b->vertex = (GLdouble*)calloc(b->freeObj*NumVertex, sizeof(GLdouble));
    b->texcoord = (GLdouble*)calloc(b->freeObj*NumVertex, sizeof(GLdouble));

    lua_Number w = b->base->width, h = b->base->height;
    if (b->base->method == LE_DrawSingleSprite){
        
    }
    for (int i = 0; i < b->totalObj*NumVertex; i+=NumVertex){
            b->texcoord[i] = 0;     b->texcoord[i+1] = 0;
            b->texcoord[i+2] = 1;   b->texcoord[i+3] = 0;
            b->texcoord[i+4] = 0;   b->texcoord[i+5] = 1;
            b->texcoord[i+6] = 1;   b->texcoord[i+7] = 0;
            b->texcoord[i+8] = 0;   b->texcoord[i+9] = 1;
            b->texcoord[i+10] = 1;  b->texcoord[i+11] = 1;
        }
    glUseProgram(b->type->sh);
    glGenVertexArrays(1, &b->vao);
    glGenBuffers(1, &b->vbo);

    glBindVertexArray(b->vao);

    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    glBufferData(GL_ARRAY_BUFFER, b->freeObj*sizeof(GLdouble)*24, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, b->freeObj*ObjSize, b->vertex);
    glBufferSubData(GL_ARRAY_BUFFER, b->freeObj*ObjSize, b->freeObj*ObjSize, b->texcoord);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), (void*)((b->freeObj)*ObjSize));

    
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
    return 0;
}

// proc BatchAdd (b: table(Batch), x: number, y: number)
int LE_BatchAdd(lua_L){
    if (lua_gettop(L) != 3) luaL_error(L,"LoadBatch function takes exactly 3 arguments!");
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Number x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    char reallocate = 0;
    
    
    
    if (b->freeObj == 0){
        reallocate = 1;
        b->totalObj = 1.5*b->totalObj;
        size_t size = b->totalObj*(ObjSize);
        
        GLdouble *ndata = (GLdouble*)realloc(b->vertex, size);
        b->vertex = ndata;
        ndata = (GLdouble*)realloc(b->texcoord, size);
        b->texcoord = ndata;
        b->freeObj = b->totalObj - b->numObj;
    }
    size_t first = (b->numObj*NumVertex);
    lua_Number w = b->base->width, h = b->base->height;

    b->vertex[first] = x;
    b->vertex[first+1] = y;
    b->vertex[first+2] = x+w;
    b->vertex[first+3] = y;
    b->vertex[first+4] = x;
    b->vertex[first+5] = y+h;
    b->vertex[first+6] = x+w;
    b->vertex[first+7] = y;
    b->vertex[first+8] = x;
    b->vertex[first+9] = y+h;
    b->vertex[first+10] = x+w;
    b->vertex[first+11] = y+h;

    // w *= 0.5;
    // h *= 0.5;
    // b->texcoord[first] = 0;
    // b->texcoord[first+1] = 0;
    // b->texcoord[first+2] = w;
    // b->texcoord[first+3] = 0;
    // b->texcoord[first+4] = 0;
    // b->texcoord[first+5] = h;
    // b->texcoord[first+6] = w;
    // b->texcoord[first+7] = 0;
    // b->texcoord[first+8] = 0;
    // b->texcoord[first+9] = h;
    // b->texcoord[first+10] = w;
    // b->texcoord[first+11] = h;

    
    
    
    glUseProgram(b->type->sh);
    glBindVertexArray(b->vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    if (reallocate){
        glBufferData(GL_ARRAY_BUFFER, (b->totalObj)*ObjSize*2, NULL, GL_STREAM_DRAW);
    }
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, (b->totalObj)*ObjSize, b->vertex);

    if (reallocate){
        for (int i = (b->numObj)*NumVertex; i < b->totalObj*NumVertex; i+=NumVertex){
            b->texcoord[i] = 0;     b->texcoord[i+1] = 0;
            b->texcoord[i+2] = 1;   b->texcoord[i+3] = 0;
            b->texcoord[i+4] = 0;   b->texcoord[i+5] = 1;
            b->texcoord[i+6] = 1;   b->texcoord[i+7] = 0;
            b->texcoord[i+8] = 0;   b->texcoord[i+9] = 1;
            b->texcoord[i+10] = 1;  b->texcoord[i+11] = 1;
        }
        glBufferSubData(GL_ARRAY_BUFFER, (b->totalObj)*ObjSize, (b->totalObj)*ObjSize, b->texcoord); 

        glVertexAttribPointer(1, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
        glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), (void*)((b->totalObj)*ObjSize));
        glEnableVertexAttribArray ( 0 );
        glEnableVertexAttribArray ( 1 );

    }
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    b->freeObj --;
    b->numObj ++;
    lua_pushinteger(L, b->numObj);

    return 0;
}

int LE_BatchGetIdx(lua_L){
    if (lua_gettop(L) != 2) luaL_error(L,"%s function takes exactly 2 arguments!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer i = lua_tointeger(L, 2)+NumVertex;
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    lua_pushnumber(L, b->vertex[i]);
    lua_pushnumber(L, b->vertex[i+1]);
    return 2;
}

int LE_BatchSetIdx(lua_L){
    if (lua_gettop(L) != 4) luaL_error(L,"%s function takes exactly 4 arguments!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TNUMBER);
    luaL_checktype(L, 3, LUA_TNUMBER);
    luaL_checktype(L, 4, LUA_TNUMBER);

    lua_Integer i = lua_tointeger(L, 2)+NumVertex;
    GLdouble x = lua_tonumber(L, 3), y = lua_tonumber(L, 4);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    GLdouble w = b->base->width, h = b->base->height;
    b->vertex[i] = x;
    b->vertex[i+1] = y;
    b->vertex[i+2] = x+w;
    b->vertex[i+3] = y;
    b->vertex[i+4] = x;
    b->vertex[i+5] = y+h;
    b->vertex[i+6] = x+w;
    b->vertex[i+7] = y;
    b->vertex[i+8] = x;
    b->vertex[i+9] = y+h;
    b->vertex[i+10] = x+w;
    b->vertex[i+11] = y+h;
    return 2;
}

int LE_BatchRemove(lua_L){
    if (lua_gettop(L) != 2) luaL_error(L,"%s function takes exactly 2 arguments!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer i = lua_tointeger(L, 2)+NumVertex;
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    lua_Integer last = b->numObj-1;
    GLdouble arr[12];
    for (int j = 0; j < 12; j++){
        arr[j] = b->vertex[i+j];
    }
    for (int j = 0; j < 12; j++){
        b->vertex[i+j] = b->vertex[last+j];
    }
    for (int j = 0; j < 12; j++){
        b->vertex[last+j] = arr[j];
    }

    b->numObj--;
    b->freeObj++;
    
    return 0;
}

int LE_BatchShow(lua_L){
    if (lua_gettop(L) != 1) luaL_error(L,"%s function takes exactly 1 argument!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);

    Batch *b = lua_touserdata(L, -1);

    for (int i = 0; i < b->numObj*NumVertex; i+=2){
        printf("i: %d x: %f y:%f \n", i/2, b->vertex[i], b->vertex[i+1]);
    }

    return 0;
}

// DrawBatch (t : table(Batch))
int LE_DrawBatch(lua_L){
    if (lua_gettop(L) != 1) luaL_error(L,"DrawBatch function takes exactly 1 argument!");
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);

    Batch *b = lua_touserdata(L, -1);
    if (b->numObj == 0) return 0;
    lua_getvalue(L, 1, "color");
    lua_getA4(L);
    colors[0] =  lua_tonumber(L,-4);
    colors[1] =  lua_tonumber(L,-3);
    colors[2] = lua_tonumber(L,-2);
    colors[3] = lua_tonumber(L,-1);
    
    
    lua_Number x = 0, y = 0, w = b->base->width, h = b->base->height;
    mat4 pop;
    mat4_clone(pop, Mmat);
    vec2 n = {x,y};
    
    mat4_Translate2D(pop, pop, n);
    
    // n[0] = w, n[1] = h;
    // mat4_Scale2D(pop, pop, n);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, b->base->tex);
    glBindVertexArray(b->vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    
    glUseProgram(b->type->sh);
    glUniformMatrix4fv( b->type->uniforms.mod, 1, 0, pop);
    glUniform4f( b->type->uniforms.clr, colors[0], colors[1], colors[2], colors[3]);

    glDrawArrays(GL_TRIANGLES, 0, b->numObj*6);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
    glDisable(GL_TEXTURE_2D);
    

    return 0;
}


int _LE_BatchGC(lua_L){
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getvalue(L, 1, "_batch");
    Batch *b = lua_touserdata(L, -1);
    free(b->vertex);
    free(b->texcoord);
    glDeleteBuffers(1, &b->vbo);
    glDeleteVertexArrays(1, &b->vao);
    return 0;
}

#undef ObjSize
#undef NumVertex