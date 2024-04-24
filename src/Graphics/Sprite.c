// Sprite.c


#include "src/Graphics/Sprite.h"


//LoadSprite (sprite : table, folder : string, linear : boolean)
int LE_LoadSingleSprite(lua_L)
{
    int err;
    lua_settop(L, 3);
    imImage *im = imFileImageLoad(lua_tostring(L, 2), 0, &err);
    imProcessFlip(im, im);
    Drawable *sc = lua_newuserdata(L, sizeof(Drawable));
    sc->method = &LE_DrawSingleSprite;
    int linear = lua_toboolean(L, 3);
    lua_setfield(L, 1, "_drawable");
    glGenTextures(1, &sc->tex);
    
    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int glformat;
    GLubyte* gldata = imImageGetOpenGLData(im, &glformat);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, gldata);
    
    sc->height = im->height, sc->width = im->width;
    

    glBindTexture(GL_TEXTURE_2D, 0);
    
    imImageDestroy(im);
    
    return 0;
}

//DrawSprite (sprite : table)
int LE_DrawSingleSprite(lua_L){
    #ifdef DEBUG
        Graphics_checkargs(L, "Graphics.DrawSprite", 1, "t");
    #endif
    if (lua_gettop(L) != 1) luaL_error(L, "DrawSingleSprite takes exactly 1 argument!");
    
    if (!lua_getfield(L, 1, "visible")) return 0;

    lua_getvalue(L, 1, "_drawable");
    Drawable *sc = lua_touserdata(L, -1);
    
    lua_settop(L, 1);
    lua_getvalue(L, 1, "pos");
    lua_getvalue(L, 1, "origin");
    

    lua_getidx(L, 2, 1);       //Sprite pos x
    lua_getidx(L, 2, 2);       //Sprite pos y
    lua_getidx(L, 3, 1);       //Sprite width
    lua_getidx(L, 3, 2);       //Sprite height
    
    
    
    lua_Number w = sc->width, h = sc->height;
    lua_Number x = lua_tonumber(L,4), offx = w*lua_tonumber(L,6), 
               y = lua_tonumber(L,5), offy = h*lua_tonumber(L,7);
    
    lua_settop(L, 1);


    lua_getvalue(L, 1, "color");
    lua_toA4(L, colors, lua_tonumber);

    lua_getvalue(L, 1, "angle");
    lua_Number angle = lua_tonumber(L,-1);
    
    mat4 pop;
    mat4_clone(pop, Mmat);
    mat4_Rotate2D(pop, angle);

    vec2 n = {x+offx,y-offy};

    mat4_Translate2D(pop, pop, n);
    n[0] = w, n[1] = h;
    mat4_Scale2D(pop, pop, n);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sc->tex);

    // glBindVertexArray(Sta.vao);
    glUseProgram(Sta.sh);
    glUniformMatrix4fv( Sta.uniforms.mod, 1, 0, pop);
    glUniform4f( Sta.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // glBindVertexArray(0);
    lua_settop(L, 1);

    glDisable(GL_TEXTURE_2D);

    
    
    return 0;
}


// LoadSpriteSheet (Sp_sheet: table, Sp_desc_folder: string, linear: boolean)
int LE_LoadSpriteSheet(lua_L)
{
    int err;
    lua_settop(L,3);
    int linear = lua_toboolean(L,3);
    
    lua_pushvalue(L, 2);
    lua_pushstring(L, ".lua");
    lua_concat(L, 2);
    const char* fold = lua_tostring(L, -1);
    lua_pop(L,1);

    luaL_dofile(L, fold);
    lua_setfield(L, 1, "src");
    
    lua_getvalue(L, 1, "src");
    
    
    lua_getvalue(L, -1, "fmt");
    lua_pushvalue(L, 2);
    lua_pushvalue(L,-2);
    
    lua_concat(L, 2);
    
    Drawable *sc = lua_newuserdata(L, sizeof(Drawable));
    sc->method = &LE_DrawSpriteSheet;
    lua_setfield(L, 1, "_drawable");
    imImage *im = imFileImageLoad(lua_tostring(L, -1), 0, &err);
    imProcessFlip(im, im); // fliping image cause IM thinks that's everything is upside-down
    int glformat;
    GLubyte *buff = imImageGetOpenGLData(im, &glformat);
    sc->height = im->height, sc->width = im->width;

    lua_settop(L,3);
    lua_getvalue(L, 1, "src");
    lua_getvalue(L, -1, "anim");
    lua_Integer arrlen = luaL_len(L, -1);
    sc->src = lua_newuserdata(L, sizeof(lua_Number*)*arrlen);
    lua_setfield(L, -2, "_");
    
    
    for(int i = 0; i < arrlen; i++){
        lua_getidx(L, -1, i+1);
        sc->src[i] = lua_newuserdata(L, sizeof(lua_Number)*4);
        lua_setfield(L, -2, "_");
        lua_Integer arr[4];
        for (int j = 0; j < 4; j++){
            lua_getidx(L, -1, j+1);
            arr[j] = lua_tointeger(L,-1);
            lua_pop(L,1);
        }
        lua_pop(L,1);

        register const double fw = 1.0/sc->width; register const double fh = 1.0/sc->height; 
        sc->src[i][2] = arr[2]*fw; sc->src[i][3] = arr[3]*fh;
        sc->src[i][0] = arr[0]*fw; sc->src[i][1] = arr[1]*fh; 
        
    }


    glGenTextures(1, &sc->tex);
    

    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, buff);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    

    imImageDestroy(im);

    return 0;
}

// DrawSpriteSheet (Sp_sheet : table)
int LE_DrawSpriteSheet(lua_L)
{
    #ifdef DEBUG
        Graphics_checkargs(L, "Graphics.DrawSpriteSheet", 1, "t");
    #endif
    if (!lua_getfield(L, 1, "visible")) return 0;
    
    lua_getvalue(L, 1, "_drawable");
    
    Drawable *sc = lua_touserdata(L, -1);
    
    lua_pop(L,2);
    
    
    if(lua_getfield(L, 1, "pos")){
        lua_getidx(L, 2, 1);
        lua_getidx(L, 2, 2);
    }else{
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
    }
    double x = lua_tonumber(L, -2), y = lua_tonumber(L, -1);
    
    lua_getvalue(L, 1, "color");
    lua_getA4(L);
    colors[0] =  lua_tonumber(L,-4);
    colors[1] =  lua_tonumber(L,-3);
    colors[2] = lua_tonumber(L,-2);
    colors[3] = lua_tonumber(L,-1);

    lua_settop(L, 1);
    
    lua_getvalue(L, 1, "origin");
    lua_getA2(L);
    lua_getvalue(L, 1, "size");
    lua_getA2(L);
    
    double w = lua_tonumber(L, -2), h = lua_tonumber(L,-1);
    double offy = lua_tonumber(L, -4), offx = lua_tonumber(L, -5);
    
    lua_getvalue(L, 1, "angle");
    lua_getvalue(L, 1, "frame");
    lua_getvalue(L, 1, "anim");
    int anim = lua_tointeger(L, -1), frame = lua_tointeger(L, -2); double angle = lua_tonumber(L, -3);
    
    
    lua_settop(L, 1);
    
 
    GLdouble tw = sc->src[anim-1][2], th = sc->src[anim-1][3];
    GLdouble tx = sc->src[anim-1][0] + frame*tw, ty = sc->src[anim-1][1]; 

    // 0, 1,
    // 1, 1,
    // 0, 0,
    // 1, 1,
    // 0, 0,
    // 1, 0,
    
    texcoords[8] = texcoords[4] = texcoords[0] = tx;  texcoords[9] = texcoords[11] = texcoords[5] = ty; 
    texcoords[10] = texcoords[6] = texcoords[2] = tx + tw;    texcoords[7] = texcoords[3] = texcoords[1] = ty + th;

    offx *= tw*sc->width, offy *= th*sc->height;
    mat4 pop;
    mat4_clone(pop, Mmat);
    mat4_Rotate2D(pop, angle);

    vec2 n = {x+offx, y-offy};

        mat4_Translate2D(pop, pop, n);
        n[0] = sc->width*tw*w, n[1] = sc->height*th*h;
        mat4_Scale2D(pop, pop, n);

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, sc->tex);
        
        glBindVertexArray(Dyn.vao);
        glBindBuffer(GL_ARRAY_BUFFER, Dyn.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
        // glBufferSubData(GL_ARRAY_BUFFER, sizeof(texcoords), sizeof(texcoords), gvertex);
        
        glUseProgram(Dyn.sh);
        glUniformMatrix4fv( Dyn.uniforms.mod, 1, 0, pop);
        glUniform4f( Dyn.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glDisable(GL_TEXTURE_2D);


    return 0;
}

int Sp_GetOSize(lua_L){
    lua_getvalue(L, 1, "_drawable");
    Drawable * sc = lua_touserdata(L, -1);
    lua_createtable(L, 2, 2);
    lua_pushinteger(L, sc->width);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, sc->height);
    lua_seti(L, -2, 2);
    return 1;
}

int Sp_ResetOSize(lua_L){
    lua_getvalue(L, 1, "_drawable");
    Drawable * sc = lua_touserdata(L, -1);
    lua_getvalue(L, 1, "size");
    lua_pushinteger(L, sc->width);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, sc->height);
    lua_seti(L, -2, 2);
    return 1;
}

int Sp_SetOSize(lua_L){
    lua_getvalue(L, 1, "_drawable");
    Drawable * sc = lua_touserdata(L, -1);
    sc->width = luaL_checkinteger(L,2), sc->height = luaL_checkinteger(L,3);
    return 0;
}
