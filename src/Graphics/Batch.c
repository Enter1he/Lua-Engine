
#include "src/Graphics/Batch.h"

const size_t maxVertex = 1000;
#define ObjSize 12*sizeof(GLfloat)
#define NumVertex 12

static void setPoints(GLfloat* p, size_t i, float x, float y, float w, float h){
    p[i] = x;     p[i+1] = y;
    p[i+2] = x+w; p[i+3] = y;
    p[i+4] = x;   p[i+5] = y+h;
    p[i+6] = x+w; p[i+7] = y;
    p[i+8] = x;   p[i+9] = y+h;
    p[i+10] = x+w;p[i+11] = y+h;
}

static void fillPoints(GLfloat* p, size_t n, float x, float y, float w, float h){
    for (int i = 0; i < n*NumVertex; i+=NumVertex){ 
        p[i] = x;     p[i+1] = y;
        p[i+2] = x+w; p[i+3] = y;
        p[i+4] = x;   p[i+5] = y+h;
        p[i+6] = x+w; p[i+7] = y;
        p[i+8] = x;   p[i+9] = y+h;
        p[i+10] = x+w;p[i+11] = y+h;
    }
}

// b->vertex[i] = 0;     b->vertex[i+1] = 0;
// b->vertex[i+2] = 1;   b->vertex[i+3] = 0;
// b->vertex[i+4] = 0;   b->vertex[i+5] = 1;
// b->vertex[i+6] = 1;   b->vertex[i+7] = 0;   REMEMBER THE ORDER!
// b->vertex[i+8] = 0;   b->vertex[i+9] = 1;
// b->vertex[i+10] = 1;  b->vertex[i+11] = 1;

// proc LoadBatch(self : table(Batch?uninit), base: table(Sprite or Sheet))
int LE_LoadBatch(lua_L){
    
    if (lua_gettop(L) > 3) luaL_error(L, "%s function takes only 3 arguments!", __func__);
    if (lua_gettop(L) < 2) luaL_error(L, "%s function takes not less than 2 arguments!", __func__);
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    
    size_t initSize = 50;

    if (lua_isinteger(L,3)) initSize = lua_tointeger(L,3);
    
    Batch *b = lua_newuserdata(L, sizeof(Batch));
    b->objType = NULL;
    
    lua_setfield(L, 1, "_batch");
    lua_getvalue(L, 2, "_drawable");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    b->base = (Drawable *)lua_touserdata(L, -1);
    
    
    b->numObj = 0;
    b->freeObj = initSize;
    b->totalObj = initSize;
    b->vertex = (GLfloat*)calloc(b->freeObj*NumVertex, sizeof(GLfloat));
    b->texcoord = (GLfloat*)calloc(b->freeObj*NumVertex, sizeof(GLfloat));

    lua_Number w = b->base->width, h = b->base->height;
    GLfloat  rx = 0, ry = 0, rw = 1, rh = 1;
    if (b->base->src){ // for spriteAtlas
        printf("has source!\n");
        b->objType = (int*) calloc(b->freeObj*NumVertex, sizeof(int)); // allocating

        w *= b->base->src[0][2]; h *= b->base->src[0][3];
        rx = b->base->src[0][0]; ry = b->base->src[0][1]; rw = b->base->src[0][2]; rh = b->base->src[0][3];
    }
    fillPoints(b->vertex, b->totalObj, 0, 0, w, h);
    fillPoints(b->texcoord, b->totalObj, rx, ry, rw, rh);
    glUseProgram(Bat.sh);
    glGenVertexArrays(1, &b->vao);
    glGenBuffers(1, &b->vbo);

    glBindVertexArray(b->vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);

    glBufferData(GL_ARRAY_BUFFER, b->freeObj*ObjSize*2, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, b->freeObj*ObjSize, b->vertex);
    glBufferSubData(GL_ARRAY_BUFFER, b->freeObj*ObjSize, b->freeObj*ObjSize, b->texcoord);
    glVertexAttribPointer(Bat.attr.pos, 2, GL_FLOAT, GL_TRUE, 2*sizeof(GLfloat), 0);
    glVertexAttribPointer(Bat.attr.txc, 2, GL_FLOAT, GL_TRUE, 2*sizeof(GLfloat), (void*)((b->freeObj)*ObjSize));

    
    glEnableVertexAttribArray ( Bat.attr.txc );
    glEnableVertexAttribArray ( Bat.attr.pos );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
    return 0;
}

enum Batchtype{
    B_NORMAL = 3,
    B_ATLAS = 4,
};

// proc BatchAdd (b: table(Batch), x: number, y: number, t: integer)
int LE_BatchAdd(lua_L){
    int args = lua_gettop(L);
    if (args < 3 || args > 4) luaL_error(L,"LoadBatch function takes exactly 3 or 4 arguments!");
    luaL_checktype(L, 1, LUA_TTABLE);
    float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
    int type = 0;
    int batchtype = B_NORMAL; 
    if (args == 4){
        batchtype = B_ATLAS; 
        luaL_checkinteger(L, 4);
        type = lua_tointeger(L,4)-1;
    }
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    char reallocate = 0;
    
    if (b->freeObj == 0){
        reallocate = 1;
        b->totalObj = 1.5*b->totalObj;
        size_t size = b->totalObj*(ObjSize);
        
        GLfloat *ndata = (GLfloat*)realloc(b->vertex, size);
        b->vertex = ndata;
        
        ndata = (GLfloat*)realloc(b->texcoord, size);
        b->texcoord = ndata;

        if (batchtype == B_ATLAS){
            int *data = (int*) realloc(b->objType, sizeof(int)*b->totalObj);
            b->objType = data;
        }

        b->freeObj = b->totalObj - b->numObj;
    }
    size_t first = (b->numObj*NumVertex);
    float w = (float)b->base->width, h = (float)b->base->height;
    float rx = 0, ry = 0, rw = 1, rh = 1;
    float **arr = b->base->src;
    if (batchtype == B_ATLAS) {
        rx = arr[type][0]; ry = arr[type][1];
        rw = arr[type][2]; rh = arr[type][3];
        w *= rw; h *= rh;
    }
    
    setPoints(b->vertex, first, x, y, w, h);
    
    setPoints(b->texcoord, first, rx, ry, rw, rh);
    
    glUseProgram(Bat.sh);
    glBindVertexArray(b->vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    if (reallocate){
        glBufferData(GL_ARRAY_BUFFER, (b->totalObj)*ObjSize*2, NULL, GL_DYNAMIC_DRAW);
    }
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, (b->totalObj)*ObjSize, b->vertex);
    glBufferSubData(GL_ARRAY_BUFFER, (b->totalObj)*ObjSize, (b->totalObj)*ObjSize, b->texcoord); 
    if (reallocate){
        // fillPoints(b->texcoord, b->totalObj, rx, ry, rw, rh);

        

        glVertexAttribPointer(Bat.attr.pos, 2, GL_FLOAT, GL_TRUE, 2*sizeof(GLfloat), 0);
        glVertexAttribPointer(Bat.attr.txc, 2, GL_FLOAT, GL_TRUE, 2*sizeof(GLfloat), (void*)((b->totalObj)*ObjSize));
        glEnableVertexAttribArray ( Bat.attr.txc );
        glEnableVertexAttribArray ( Bat.attr.pos );

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
    GLfloat x = lua_tonumber(L, 3), y = lua_tonumber(L, 4);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    GLfloat w = b->base->width, h = b->base->height;
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
    lua_Integer i = luaL_checkinteger(L, 2)+NumVertex;
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    lua_Integer last = b->numObj-1;
    GLfloat arr[12];
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
    
    lua_getvalue(L,1, "angle");
    double angle = lua_tonumber(L,-1);

    lua_Number x = 0, y = 0, w = b->base->width, h = b->base->height;
    mat4 pop, pop2;
    mat4_clone(pop, Mmat);
    vec2 n = {x,y};
    mat4_Rotate2D(pop2, pop, angle);
    mat4_Translate2D(pop, pop2, n);
    
    lua_getvalue(L, 1, "size");
    lua_getA2(L);
    n[0] = lua_tofloat(L,-2), n[1] = lua_tofloat(L,-1);
    mat4_Scale2D(pop, pop, n); 

    glBindTexture(GL_TEXTURE_2D, b->base->tex);
    glBindVertexArray(b->vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    
    glUseProgram(Bat.sh);
    glUniformMatrix4fv( Bat.uniforms.mod, 1, 0, pop);
    glUniform4f( Bat.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);

    glDrawArrays(GL_TRIANGLES, 0, b->numObj*6);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
    

    return 0;
}


int _LE_BatchGC(lua_L){
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!lua_getfield(L, 1, "_batch")) return 0;
    Batch *b = lua_touserdata(L, -1);
    if (b->objType) free(b->objType);
    free(b->vertex);
    free(b->texcoord);
    glDeleteBuffers(1, &b->vbo);
    glDeleteVertexArrays(1, &b->vao);
    return 0;
}

#undef ObjSize
#undef NumVertex