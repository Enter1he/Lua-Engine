// Sprite.c


#include "src/Graphics/Sprite.h"
#define STB_IMAGE_IMPLEMENTATION
#include "src/Graphics/stb_image.h" 


//LoadSprite (sprite : table, folder : string, linear : boolean)
int LE_LoadSingleSprite(lua_L)
{
    int err;
    lua_settop(L, 3);
    const char* folder = lua_tostring(L, 2);

    Drawable *sc = lua_newuserdata(L, sizeof(Drawable));
    if (!sc){
        Error_print("LE_LoadSingleSprite: failed to create userdata!");
    }
    sc->src = NULL;
    sc->method = &LE_DrawSingleSprite;
    int linear = lua_toboolean(L, 3);
    int repeat = lua_toboolean(L, 4);
    lua_setfield(L, 1, "_drawable");
    glGenTextures(1, &sc->tex);
    
    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat)? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat)? GL_REPEAT : GL_CLAMP_TO_EDGE);
    

    int width = 0, height = 0, channels = 0;
    GLubyte* gldata = stbi_load(folder, &width, &height, &channels, STBI_default);

    switch(channels){
        case 3:{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, gldata);
            break;
        }
        case 4: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gldata);
            break;
        }
        default:{
            luaL_error(L,"Something wrong with an image file! Not enough channels");
        }
    }

    
    
    sc->height = height, sc->width = width;
    

    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(gldata);
    
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
    
    
    sc->src = NULL;
    float w = sc->width, h = sc->height;
    float x = lua_tofloat(L,4), y = lua_tofloat(L,5), 
               offx = w*lua_tofloat(L,6), offy = h*lua_tofloat(L,7);
    
    lua_settop(L, 1);


    lua_getvalue(L, 1, "color");
    lua_toA4(L, colors, lua_tofloat);

    lua_getvalue(L, 1, "angle");
    float angle = lua_tofloat(L,-1);
    
    mat4 pop;
    mat4_clone(pop, Mmat);

    vec2 n = {x,y};

    n[0] = x, n[1] = y;
    mat4_Translate2D(pop, pop, n);
    mat4_Rotate2D(pop, pop, -angle);
    n[0] = -offx, n[1] = -offy;
    mat4_Translate2D(pop, pop, n);
    n[0] = w, n[1] = h;
    mat4_Scale2D(pop, pop, n);

    // glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sc->tex);

    glBindVertexArray(Sta.vao);
    glUseProgram(Sta.sh);
    glUniformMatrix4fv( Sta.uniforms.mod, 1, 0, pop);
    glUniform4f( Sta.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    lua_settop(L, 1);

    // glDisable(GL_TEXTURE_2D);

    
    
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
    const char* folder = lua_tostring(L, -1);
    int glformat;
    int channels = 0;
    GLubyte* gldata = stbi_load(folder, &sc->width, &sc->height, &channels, STBI_default);
    
    

    lua_settop(L,3);
    lua_getvalue(L, 1, "src");
    lua_getvalue(L, -1, "anim");
    lua_Integer arrlen = luaL_len(L, -1);
    sc->src = lua_newuserdata(L, sizeof(float*)*arrlen);
    lua_setfield(L, -2, "_");
    
    
    for(int i = 0; i < arrlen; i++){
        lua_getidx(L, -1, i+1);
        sc->src[i] = lua_newuserdata(L, sizeof(float)*4);
        lua_setfield(L, -2, "_");
        lua_Integer arr[4];
        for (int j = 0; j < 4; j++){
            lua_getidx(L, -1, j+1);
            arr[j] = lua_tointeger(L,-1);
            lua_pop(L,1);
        }
        lua_pop(L,1);

        register const float fw = 1.0/(float)sc->width; register const float fh = 1.0/(float)sc->height; 
        sc->src[i][2] = arr[2]*fw; sc->src[i][3] = arr[3]*fh;
        sc->src[i][0] = arr[0]*fw; sc->src[i][1] = arr[1]*fh; 
        
    }


    glGenTextures(1, &sc->tex);
    

    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    
    
    switch(channels){
        case 3:{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sc->width, sc->height, 0, GL_RGB, GL_UNSIGNED_BYTE, gldata);
            break;
        }
        case 4: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sc->width, sc->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gldata);
            break;
        }
        default:{
            luaL_error(L,"Something wrong with an image file! Not enough channels");
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    

    stbi_image_free(gldata);

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
    float x = lua_tofloat(L, -2), y = lua_tofloat(L, -1);
    
    lua_getvalue(L, 1, "color");
    lua_getA4(L);
    GLfloat colors[4];
    colors[0] =  lua_tofloat(L,-4);
    colors[1] =  lua_tofloat(L,-3);
    colors[2] = lua_tofloat(L,-2);
    colors[3] = lua_tofloat(L,-1);

    lua_settop(L, 1);
    
    lua_getvalue(L, 1, "origin");
    lua_getA2(L);
    lua_getvalue(L, 1, "size");
    lua_getA2(L);
    
    float w = lua_tofloat(L, -2), h = lua_tofloat(L,-1);
    float offy = lua_tofloat(L, -4), offx = lua_tofloat(L, -5);
    
    lua_getvalue(L, 1, "angle");
    lua_getvalue(L, 1, "frame");
    lua_getvalue(L, 1, "anim");
    int anim = lua_tointeger(L, -1), frame = lua_tointeger(L, -2); float angle = lua_tofloat(L, -3);
    
    
    lua_settop(L, 1);
    
 
    GLfloat tw = sc->src[anim-1][2], th = sc->src[anim-1][3];
    GLfloat tx = sc->src[anim-1][0] + frame*tw, ty = sc->src[anim-1][1]; 

    // 0, 1,
    // 1, 1,
    // 0, 0,
    // 1, 1,
    // 0, 0,
    // 1, 0,
    
    texcoords[8] = texcoords[4] = texcoords[0] = tx;  texcoords[9] = texcoords[11] = texcoords[5] = ty; 
    texcoords[10] = texcoords[6] = texcoords[2] = tx + tw;    texcoords[7] = texcoords[3] = texcoords[1] = ty + th;

    offx *= (float)(tw*sc->width), offy *= (float)(th*sc->height);
    mat4 pop;

    
    
    mat4_clone(pop, Mmat);

    vec2 n = {x, y};
        
       
        n[0] = x, n[1] = y;
        mat4_Translate2D(pop, pop, n);
        mat4_Rotate2D(pop, pop, -angle);
        n[0] = -offx, n[1] = -offy;
        mat4_Translate2D(pop, pop, n);
        

        n[0] = sc->width*tw*w, n[1] = sc->height*th*h;
        mat4_Scale2D(pop, pop, n);

        // glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, sc->tex);
        
        glBindVertexArray(Dyn.vao);
        glBindBuffer(GL_ARRAY_BUFFER, Dyn.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
        
        
        glUseProgram(Dyn.sh);
        glUniformMatrix4fv( Dyn.uniforms.mod, 1, 0, pop);
        glUniform4f( Dyn.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // glDisable(GL_TEXTURE_2D);


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
