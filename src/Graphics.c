#include "Graphics.h"

#include "glew.h"

#include <windows.h>

#include "include/im.h"
#include "include/im_image.h"
#include "include/im_convert.h"
#include "include/im_process.h"

#include "include/ft2build.h"
#include "include/freetype/freetype.h"
#include "include/freetype/ftglyph.h"
#include FT_FREETYPE_H

typedef float mat4[16];
typedef float vec3[3];
typedef float vec2[2];

void mat4_Ortho(float *out, float left, float right, float bottom, float top, float zNear, float zFar){
  const float lr = 1 / (left - right);
  const float bt = 1 / (bottom - top);
  const float nf = 1 / (zNear - zFar);
  out[0] = -2 * lr;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  out[4] = 0;
  out[5] = -2 * bt;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = nf;
  out[11] = 0;
  out[12] = (left + right) * lr;
  out[13] = (top + bottom) * bt;
  out[14] = zNear * nf;
  out[15] = 1;
}

void mat4_Translate2D(float *out, const float *a, float *v){
  float x = v[0],
    y = v[1];
  float a00, a01, a02, a03;
  float a10, a11, a12, a13;
  float a20, a21, a22, a23;

  if (a == out) {
    out[12] = a[0] * x + a[4] * y + a[12];
    out[13] = a[1] * x + a[5] * y + a[13];
    out[14] = a[2] * x + a[6] * y + a[14];
    out[15] = a[3] * x + a[7] * y + a[15];
  } else {
    a00 = a[0];
    a01 = a[1];
    a02 = a[2];
    a03 = a[3];
    a10 = a[4];
    a11 = a[5];
    a12 = a[6];
    a13 = a[7];
    a20 = a[8];
    a21 = a[9];
    a22 = a[10];
    a23 = a[11];

    out[0] = a00;
    out[1] = a01;
    out[2] = a02;
    out[3] = a03;
    out[4] = a10;
    out[5] = a11;
    out[6] = a12;
    out[7] = a13;
    out[8] = a20;
    out[9] = a21;
    out[10] = a22;
    out[11] = a23;

    out[12] = a00 * x + a10 * y + a[12];
    out[13] = a01 * x + a11 * y + a[13];
    out[14] = a02 * x + a12 * y + a[14];
    out[15] = a03 * x + a13 * y + a[15];
  }

}

void mat4_Scale2D(float *out, float *a, float *v){
  float x = v[0],
    y = v[1];

  out[0] = a[0] * x;
  out[1] = a[1] * x;
  out[2] = a[2] * x;
  out[3] = a[3] * x;
  out[4] = a[4] * y;
  out[5] = a[5] * y;
  out[6] = a[6] * y;
  out[7] = a[7] * y;
  out[12] = a[12];
  out[13] = a[13];
  out[14] = a[14];
  out[15] = a[15];
  
}

void mat4_identity(float *out){
  out[0] = 1;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  out[4] = 0;
  out[5] = 1;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = 1;
  out[11] = 0;
  out[12] = 0;
  out[13] = 0;
  out[14] = 0;
  out[15] = 1;
}

void mat4_clone(float *out, float *a){
  if (a == out) return;
  out[0] = a[0];
  out[1] = a[1];
  out[2] = a[2];
  out[3] = a[3];
  out[4] = a[4];
  out[5] = a[5];
  out[6] = a[6];
  out[7] = a[7];
  out[8] = a[8];
  out[9] = a[9];
  out[10] = a[10];
  out[11] = a[11];
  out[12] = a[12];
  out[13] = a[13];
  out[14] = a[14];
  out[15] = a[15];
}


#define DEBUG 1
#undef DEBUG

const GLdouble 
gvertex[] = {
    
    0, 1,
    1, 1,
    0, 0,
    1, 1,
    0, 0,
    1, 0,

    
    0, 1,
    1, 1,
    0, 0,
    1, 1,
    0, 0,
    1, 0,
},
tverts[] = {
    
    0, 1,
    1, 1,
    0, 0,
    1, 1,
    0, 0,
    1, 0,
    
};
struct Entity{
    GLuint vao, vbo, sh;
    struct {
        GLuint pnt, clr, pro, mod;
    }uniforms;
}Dyn, Sta, Prim;
GLdouble colors[24] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}, texcoords[12], text_uv[12]; // arrays that change over time
mat4 Pmat = {};
mat4 Mmat = {};

typedef struct _Drawable{
    GLuint tex;
    size_t width, height;
    lua_CFunction method;
    lua_Number **src;
}Drawable;

const size_t maxVertex = 1000;
typedef struct _Batch{
    size_t numVertex, freeVertex;
    GLuint vbo, ubo, vao;
    GLdouble *vertex, *texcoord;
    Drawable *base;
    struct Entity* type;
}Batch;

typedef struct _Vertex{
    GLdouble pos[2], uv[4], color[4];
}Vertex;

typedef struct _textchar{
    GLuint tex;
    int w,h,top, left;
    double dx, dy;
}TextChar;


static inline int next_p2 (int a)
{
    int rval=1;
    while(rval<a) 
        rval<<=1;

    return rval;
}

int Graphics_checkargs(lua_L, const char* func, int num, const char* str){
    int args = lua_gettop(L);
    if (num != args){
        Error_print("%s takes %d arguments! submitted %d",func, num, args);
        return 0;
    }
    for (char *c = (char*)str, i = 1; i < args; c++, i++){
        switch(*c){
            case 'b':
                if (!lua_isboolean(L, i)){
                    Error_print("%s arg %d isn't boolean! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            case 'i':
                if (!lua_isinteger(L, i)){
                    Error_print("%s arg %d isn't integer! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            case 'n':
                if (!lua_isnumber(L, i)){
                    Error_print("%s arg %d isn't number! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            case 's':
                if (!lua_isstring(L, i)){
                    Error_print("%s arg %d isn't string! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            case 't':
                if (!lua_istable(L, i)){
                    Error_print("%s arg %d isn't table! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            case 'u':
                if (!lua_isuserdata(L, i)){
                    Error_print("%s arg %d isn't userdata! submitted %s", func, i, lua_typename(L, i));
                    return 0;        
                }
            break;
            
        }
    }
    return num;
}

// text:Load( obj: table, linear: boolean)
int LE_LoadText(lua_L)
{
    if(lua_getfield(L, 1, "font") != LUA_TSTRING) {Error_print( "font_folder should be a string!");}
    const char *font_folder = lua_tostring(L, -1);

    lua_getvalue(L, 1, "size");
    size_t h = lua_tointeger(L,-1);

    lua_pop(L, 2);

    int linear = lua_toboolean(L, 2);
    int alpha = lua_getfield(L, 1, "alpha");

    TextChar *sc = lua_newuserdata(L, 128*sizeof(TextChar));
    lua_setfield(L, 1, "_drawable");
    
    
    FT_Library Library = 0; FT_Face face = 0;
    FT_Init_FreeType(&Library);
    FT_New_Face(Library, font_folder, 0, &face);
    FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);
    
    for (unsigned char ch = 0; ch < 128; ch++){
        
        if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
            Error_print("%s %s %d: FT_Load_Glyph failed",  __FILE__, __func__ , __LINE__);
 
        FT_Glyph glyph;
        if(FT_Get_Glyph( face->glyph, &glyph ))
            Error_print("%s %s %d: FT_Get_Glyph failed", __FILE__, __func__ , __LINE__);

        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        FT_Bitmap *bitmap= &bitmap_glyph->bitmap;
        
        
        sc[ch].left = bitmap_glyph->left;
        sc[ch].w = bitmap->width; 
        sc[ch].h = bitmap->rows; 
        sc[ch].top = face->glyph->bitmap_top;
        int width = next_p2(bitmap->width), height = next_p2(bitmap->rows);
        sc[ch].dx = (double)bitmap->width/(double)width; 
        sc[ch].dy = (double)bitmap->rows/(double)height; 

        GLubyte* data = malloc(sizeof(GLubyte) * width * height * 2);
        for(int i=0; i < height; i++) {
            for(int j=0; j < width; j++){
                data[2*(j+i*width)+1] = (j>=bitmap->width || i>=bitmap->rows) ? 0 : bitmap->buffer[j + bitmap->width*i];
                if (alpha)
                    data[2*(j+i*width)] = data[2*(j+i*width)+1];
                else
                    data[2*(j+i*width)] = (data[2*(j+i*width)+1]) ? 255 : 0;
                
            };
        };
        glGenTextures(1, &sc[ch].tex);    
        
        glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linear)? GL_LINEAR : GL_NEAREST);
        glTexEnvi( GL_TEXTURE_ENV,  GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, (alpha)? GL_RGBA : GL_RGB, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        free(data);
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(Library);

    return 0;
}

// DrawText (text: table)
int LE_DrawText(lua_L)
{
    size_t len = 0;
    lua_getvalue(L, 1, "value");
    luaL_checklstring(L, -1, &len);

    const char* value = lua_tostring(L, -1);
    lua_getvalue(L, 1, "size");
    size_t size = lua_tointeger(L, -1);

    lua_getvalue(L, 1, "_drawable");
    TextChar *sc = lua_touserdata(L, -1);
    lua_settop(L,1);
    
    lua_getvalue(L, 1, "color");
    lua_getA4(L);
    
    colors[0] = lua_tonumber(L,-4);
    colors[1] = lua_tonumber(L,-3);
    colors[2] = lua_tonumber(L,-2);
    colors[3] = lua_tonumber(L,-1);
    lua_settop(L, 1);

    lua_getvalue(L, 1, "pos");
    lua_getA2(L);

    double px = lua_tonumber(L,-2), py = lua_tonumber(L,-1);
    int line = 0;
    mat4 pop1, pop2;
    vec2 n = {};
    mat4_clone(pop1, Mmat);
    

    unsigned int lineW = 0, plineW = 0;
    for (int i = 0; i < len; i++){
        char ch = value[i]; 
        char pch = value[i-1];
        char sp = 'm';
        if (i){
            px = (ch == ' '|| ch == '\n') ? sc[sp].w: sc[pch].w+sc[ch].left;
            if (ch == '\t') px = sc[sp].w*4;
            if (ch == '\r') px = 0;
            lineW += px;
            
            if (pch=='\n'){
                line++;
                py = size;
                px -= lineW-sc[sp].w;
                lineW = 0;
            }else
            {
                py = 0;
                px = px;
            }
        };
        if (ch != '\n'){
            text_uv[7] = text_uv[1] = text_uv[3] = sc[ch].dy;
            text_uv[2] = text_uv[6] = text_uv[10] = sc[ch].dx;
            
            n[0] = px; n[1] = py;
            mat4_Translate2D(pop1, pop1, n);
            mat4_clone(pop2, pop1);
            n[0] = 0; n[1] = -sc[ch].top;
            mat4_Translate2D(pop2, pop2, n);
            n[0] = sc[ch].w; n[1] = sc[ch].h;
            mat4_Scale2D(pop2, pop2, n);

            glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
        
            glBindVertexArray(Dyn.vao);
            glBindBuffer(GL_ARRAY_BUFFER, Dyn.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), text_uv);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(texcoords), sizeof(texcoords), gvertex);
            
            glUseProgram(Dyn.sh);

            glUniformMatrix4fv( glGetUniformLocation(Dyn.sh, "mod"), 1, 0, pop2);
            glUniform4f( glGetUniformLocation(Dyn.sh, "clr"), colors[0], colors[1], colors[2], colors[3]);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }
    };

    return 0;
}

int LE_DrawSingleSprite(lua_L);
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
    
    mat4 pop;
    mat4_clone(pop, Mmat);
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

    lua_getvalue(L, 1, "angle");
    
    return 0;
}

int LE_DrawSpriteSheet(lua_L);
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

int LE_LoadBatch(lua_L){
    if (lua_gettop(L) != 2) Error_print("LoadBatch function takes exactly 2 arguments!");
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_getvalue(L, 2, "_drawable");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    
    Batch *b = lua_newuserdata(L, sizeof(Batch));
    lua_setfield(L, 1, "_batch");
    b->base = (Drawable *)lua_touserdata(L, -1);

    
    if (b->base->method == &LE_DrawSingleSprite){
        b->type = &Dyn;
    }else 
    if (b->base->method == &LE_DrawSpriteSheet){
        b->type = &Dyn;
    }else{
        Error_print("LoadBatch Unknown drawable!");
    }

    b->numVertex = 0;
    b->freeVertex = 60;
    b->vertex = (GLdouble*)calloc(b->freeVertex*2, sizeof(GLdouble));
    b->texcoord = (GLdouble*)calloc(b->freeVertex*2, sizeof(GLdouble));

    if (b->base->method == LE_DrawSingleSprite){
        for (int i = 0; i < b->freeVertex*2; i+=12){
            b->texcoord[i] = 0;     b->texcoord[i+1] = 0;
            b->texcoord[i+2] = 1;   b->texcoord[i+3] = 0;
            b->texcoord[i+4] = 0;   b->texcoord[i+5] = 1;
            b->texcoord[i+6] = 1;   b->texcoord[i+7] = 0;
            b->texcoord[i+8] = 0;   b->texcoord[i+9] = 1;
            b->texcoord[i+10] = 1;  b->texcoord[i+11] = 1;
        }
    }

    glGenVertexArrays(1, &b->vao);
    glGenBuffers(1, &b->vbo);
    glGenBuffers(1, &b->ubo);

    glBindVertexArray(b->vao);

    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    glBufferData(GL_ARRAY_BUFFER, b->freeVertex*sizeof(GLdouble)*2, b->vertex, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, b->ubo);
    glBufferData(GL_ARRAY_BUFFER, b->freeVertex*sizeof(GLdouble)*2, b->texcoord, (b->base->method == LE_DrawSingleSprite) ? GL_STATIC_DRAW : GL_STREAM_DRAW);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);

    
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;
}

int LE_DrawBatch(lua_L){
    if (lua_gettop(L) != 1) Error_print("DrawBatch function takes exactly 1 argument!");
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);

    Batch *b = lua_touserdata(L, -1);
    if (b->numVertex == 0) return 0;
    lua_getvalue(L, 1, "color");
    lua_toA4(L, colors, lua_tonumber);
    
    
    mat4 pop;
    mat4_clone(pop, Mmat);

    glBindTexture(GL_TEXTURE_2D, b->base->tex);
    glBindVertexArray(b->vao);
    
    glUseProgram(Dyn.sh);
    glUniformMatrix4fv( Dyn.uniforms.mod, 1, 0, pop);
    glUniform4f( Dyn.uniforms.clr, colors[0], colors[1], colors[2], colors[3]);

    glDrawArrays(GL_TRIANGLES, 0, b->numVertex*2);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
    

    return 0;
}

int LE_BatchAdd(lua_L){
    if (lua_gettop(L) != 1) Error_print("LoadBatch function takes exactly 1 argument!");
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Number x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    lua_getvalue(L, 1, "_batch");
    luaL_checktype(L, -1, LUA_TUSERDATA);
    Batch *b = lua_touserdata(L, -1);
    char reallocate = 0;
    if (b->freeVertex == 0){
        reallocate = 1;
        size_t size = b->numVertex*(1.5*12);
        GLdouble *ndata = (GLdouble*)calloc(size, sizeof(GLdouble));
        for (int i = 0; i < b->numVertex; i++){
            ndata[i] = b->vertex[i];
        }
        free(b->vertex);
        b->vertex = ndata;
        b->freeVertex = size - b->numVertex;
    }
    size_t first = b->numVertex;
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

    b->freeVertex -= 6;
    b->numVertex += 6;
    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
    if (reallocate){
        glBufferData(GL_ARRAY_BUFFER, (b->freeVertex+b->numVertex)*sizeof(GLdouble)*((b->base != &Dyn)?2:4), b->vertex, GL_STREAM_DRAW);
    }else{
        glBufferSubData(GL_ARRAY_BUFFER, 2*b->numVertex*sizeof(GLdouble), 12*sizeof(GLdouble), b->vertex+first);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 0;
}

int _LE_BatchGC(lua_L){
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getvalue(L, 1, "_batch");
    Batch *b = lua_touserdata(L, -1);
    free(b->vertex);
    glDeleteBuffers(1, b->vbo);
    glDeleteVertexArrays(1, b->vao);
    return 0;
}

//////////////////////////////////// METHODS TO WORK WITH OBJECT'S CORES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

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


////////////////////////////////////////////// LAYERS ARE HANDLED HERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void Graphics_SetMatrix4Layer(float* mat){
    glUseProgram(Prim.sh);
    glUniformMatrix4fv(Prim.uniforms.pro, 1, 0, mat);
    glUseProgram(Dyn.sh);
    glUniformMatrix4fv(Dyn.uniforms.pro, 1, 0, mat);
    glUseProgram(Sta.sh);
    glUniformMatrix4fv(Sta.uniforms.pro, 1, 0, mat);
    glUseProgram(0);
}

int LE_DrawLayer(lua_L){

    lua_getvalue(L, 1, "pos");
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);

    mat4 pop;
    vec2 n = {-lua_tonumber(L, -2), -lua_tonumber(L, -1)};
    mat4_clone(pop, Pmat);

    mat4_Translate2D(pop, pop, n);
    
    lua_getvalue(L, 1, "size");
    lua_getA2(L);
    
    n[0] = lua_tonumber(L, -2), n[1] = lua_tonumber(L, -1);
    mat4_Scale2D(pop, pop, n);

    lua_getvalue(L, 1, "blend");
    int check = lua_tointeger(L, -1);
    int src = (check) ? GL_SRC_COLOR : GL_SRC_ALPHA, one = (check) ? GL_ONE_MINUS_SRC_COLOR : GL_ONE_MINUS_SRC_ALPHA;

    lua_pop(L, 7);

    
    Graphics_SetMatrix4Layer(pop);
    for (int i = 1; i <= lua_rawlen(L,2); i++){
        lua_geti(L, 2, i);
        
        if(!lua_getfield(L,-1, "Draw") ){
            lua_pop(L,2);
            continue;
        }
        
        
        glEnable(GL_BLEND);
        glBlendFunc(src, one);
        lua_rotate(L, lua_gettop(L)-1, 1);
        lua_pcall(L, 1, 0, 0);
        glDisable(GL_BLEND);
    }
    Graphics_SetMatrix4Layer(Pmat);
    
    return 0;
}

int LE_DrawScene(lua_L){

    lua_getvalue(L, 1, "Draw");
    lua_pushvalue(L, 1);
    lua_pcall(L,1,0,0);
    
    return 0;
}

///////////////////////////////////////// PRIMITIVES START HERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


int LE_SetColor(lua_L){
    int args = lua_gettop(L);
    Graphics_checkargs(L, "Graphics.SetColor", 4, "nnnn");
    glUseProgram(Prim.sh);
    glUniform4f(Prim.uniforms.clr, luaL_checknumber(L,1), luaL_checknumber(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4));
    glUseProgram(0);
    return 0;
}

double *point = 0; size_t psize= 0;
int LE_DrawLine(lua_L){
    #ifdef DEBUG
        int args = Graphics_checkargs(L, "Graphics.DrawLine", 4, "nnnn");
    #else
        int args = lua_gettop(L);
    #endif
    double x1 = luaL_checknumber(L, 1), 
            y1 = luaL_checknumber(L, 2), 
            x2 = luaL_checknumber(L, 3),
            y2 = luaL_checknumber(L, 4);
    lua_settop(L, args);

    
    point[0] = x1; point[1] = y1;
    point[2] = x2; point[3] = y2;

    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLdouble)*4, point);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(Prim.sh);
    glDrawArrays(GL_LINES, 0, 2);

    
    glUseProgram(0);
    glBindVertexArray(0);
    
    return 0;
}


int LE_DrawLines(lua_L){
    #ifdef DEBUG
        Graphics_checkargs(L, "Graphics.DrawLines", 1, "t");
    #endif
    size_t len = lua_rawlen(L, 1);
    
    if (len % 2 != 0) {
        Error_print("Graphics.DrawLines: length of array must be an even number!"); 
        return 0;
    }
    if (len < 4){
        Error_print("Graphics.DrawLines: length of array must be more or even 4 numbers!"); 
        return 0;
    }
    if (len > psize/sizeof(GLdouble)){
        Error_print("Graphics.DrawLines: length of array is too much for engine! Less than %d required", psize/sizeof(GLdouble))
        return 0;
    }
    
    for (int i = 0; i < len; i+=2){
        lua_geti(L, 1, i+1);
        lua_geti(L, 1, i+2);
        point[i] = lua_tonumber(L, -2);
        point[i+1]   = lua_tonumber(L, -1);
        lua_settop(L, 1);
    }


    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLdouble)*len, point);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(Prim.sh);
    glDrawArrays(GL_LINE_STRIP, 0, len/2);

    
    glUseProgram(0);
    glBindVertexArray(0);
    return 0;
    
}


#define pi 3.14159265358979323846
#define halfpi 1.5707963267949
#define twopi 6.2831853071796
#define invtwopi 0.15915494309189
#define threehalfpi 4.7123889803847

static double cos_32s(double x)
{
    const double c1= 0.99940307;
    const double c2=-0.49558072;
    const double c3= 0.03679168;
    x *= x;
    return (c1 + x*(c2 + c3 * x));
}
static double apcos(double angle){
    //clamp to the range 0..2pi
    angle=angle-(int)(angle*invtwopi)*twopi;
    angle=angle>0.f?angle:-angle;

    if(angle<halfpi) return cos_32s(angle);
    if(angle<pi) return -cos_32s(pi-angle);
    if(angle<threehalfpi) return -cos_32s(angle-pi);
    return cos_32s(twopi-angle);
}
static inline double apsin(double angle){
    return apcos(halfpi-angle);
}

// DrawCircle(lined: bool, x: double, y: double, raius: double)

int LE_DrawCircle(lua_L){
    #ifdef DEBUG
        int args = Graphics_checkargs(L, "Graphics.DrawCircle", 4, "bnnn");
    #else
        int args = lua_gettop(L);
    #endif
    args = -args;
    double px = lua_tonumber(L, ++args), py = lua_tonumber(L, ++args);
    double radius = lua_tonumber(L, ++args);
    int step = 32;
    
    
    char change = 0;
    double angle = twopi/step;
    GLenum mode = (lua_toboolean(L,1))? GL_TRIANGLE_FAN : GL_LINE_LOOP;
    
    int j = 0;
    for (int i = step; i > 0; i--){
        double x = radius*apcos(i*angle), y = radius*apsin(i*angle);
        point[j] = px+x; point[++j] = py+y;  
        j++;
    }

    
    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, psize, point);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    glUseProgram(Prim.sh);
    glDrawArrays(mode, 0, step);

    
    glUseProgram(0);
    glBindVertexArray(0);
    
    return 0;
}

static int LEx_getArr(lua_L, size_t size){
    for (size_t i = 0; i < size; i++){
        lua_getidx(L, 1, i+1);
    }
    return size;
}

//DrawQuad(lined: bool, array: double[4])
int LE_DrawQuad(lua_L){
    #ifdef DEBUG
        int args = Graphics_checkargs(L, "Graphics.DrawQuad", 2, "bt");
    #else
        int args = lua_gettop(L);
    #endif
    GLenum mode = (lua_toboolean(L,1))? GL_TRIANGLE_STRIP: GL_LINE_LOOP;
    lua_remove(L, 1);
    for (size_t i = 0; i < 8; i++){
        lua_getidx(L, 1, i+1);
    }
    lua_Number x1 = lua_tonumber(L,3),
                y1 = lua_tonumber(L,4),
                x2 = lua_tonumber(L,5),
                y2 = lua_tonumber(L,6),
                x3 = lua_tonumber(L,7),
                y3 = lua_tonumber(L,8),
                x4 = lua_tonumber(L,9),
                y4 = lua_tonumber(L,10);
    lua_settop(L, 2);

    
    point[0] = x1; point[1] = y1;
    point[2] = x2; point[3] = y2;
    point[4] = x3; point[5] = y3;
    point[6] = x4; point[7] = y4;

    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLdouble)*8, point);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(Prim.sh);
    glDrawArrays(mode, 0, 4);

    
    glUseProgram(0);
    glBindVertexArray(0);

    return 0;
}

//DrawRect(lined: bool, x1: double, y1: double, x2: double, y2: double)
int LE_DrawRect(lua_L){
    #ifdef DEBUG
        int args = Graphics_checkargs(L, "Graphics.DrawRect", 5, "bnnnn");
    #else
    int args = lua_gettop(L);
    #endif

    int lined = lua_toboolean(L,1);
    GLenum mode = (lined)? GL_TRIANGLE_STRIP: GL_LINE_LOOP;
    args = -args;
    double x = lua_tonumber(L,++args),
        y = lua_tonumber(L,++args),
        x2 = lua_tonumber(L,++args),
        y2 = lua_tonumber(L,++args);
    
    point[0] = x; point[1] = y;
    point[2] = x; point[3] = y2;
    point[4] = x2; point[5] = (lined)? y : y2;
    point[6] = x2; point[7] = (lined)? y2 : y;

    
    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLdouble)*8, point);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(Prim.sh);
    glDrawArrays(mode, 0, 4);
    
    glUseProgram(0);
    glBindVertexArray(0);

    return 0;
}

int ModuleGC(lua_L){
    glDeleteBuffers(1, &Sta.vbo);
    glDeleteBuffers(1, &Dyn.vbo);
    glDeleteBuffers(1, &Prim.vbo);
    glDeleteVertexArrays(1, &Sta.vao);
    glDeleteVertexArrays(1, &Dyn.vao);
    glDeleteVertexArrays(1, &Prim.vao);
    if (point) free(point);
    printf("resources freed!\n");
    return 0;
}

lua_Table meta[] = {
    {"__gc", &ModuleGC},
    
    {NULL, NULL}
};

lua_Table func[] = {
    
    {"LoadText", &LE_LoadText},
    {"DrawText", &LE_DrawText},

    {"LoadSprite", &LE_LoadSingleSprite},
    {"DrawSprite", &LE_DrawSingleSprite},

    {"LoadBatch", &LE_LoadBatch},
    {"DrawBatch", &LE_DrawBatch},
    {"_BatchGC", &_LE_BatchGC},


    {"LoadSpriteSheet", &LE_LoadSpriteSheet},
    {"GetSize", &Sp_GetOSize},
    {"SetSize", &Sp_SetOSize},
    {"ResetSize", &Sp_ResetOSize},
    {"DrawSpriteSheet", &LE_DrawSpriteSheet},
    

    {"DrawLayer", &LE_DrawLayer},
    {"DrawScene", &LE_DrawScene},

    {"SetColor", &LE_SetColor},
    {"DrawLine", &LE_DrawLine},
    {"DrawLines", &LE_DrawLines},
    {"DrawCircle", &LE_DrawCircle},
    {"DrawQuad", &LE_DrawQuad},
    {"DrawRect", &LE_DrawRect},


    {NULL, NULL}
};




const char 
*base_vs = // BASE SHADERS FOR IMAGES
"#version 330 core\n\
layout (location = 1) in vec2 pos;\n\
layout (location = 0) in vec2 txc;\n\
uniform vec4 clr;\n\
uniform mat4 mod;\n\
uniform mat4 pro;\n\
out vec4 f_clr;\n\
out vec2 f_txc;\n\
void main()\n\
{\n\
   f_clr = clr;\n\
   f_txc = txc;\n\
   gl_Position = pro*mod*vec4(pos, 0, 1.0);  \n\
}",
*base_fs = 
"#version 330 core\n\
in vec4 f_clr;\n\
in vec2 f_txc;\n\
out vec4 color;\n\
uniform sampler2D tex;\n\
void main()\n\
{\n\
  color = f_clr*texture2D(tex, f_txc);\n\
}",
* single_vs = 
"#version 330 core      \n\
vec2 vert[6] = vec2[](\n\
    vec2(0, 1),         \n\
    vec2(1, 1),         \n\
    vec2(0, 0),         \n\
    vec2(1, 1),         \n\
    vec2(0, 0),         \n\
    vec2(1, 0)          \n\
);                      \n\
uniform mat4 mod;       \n\
uniform mat4 pro;       \n\
out vec2 texc;          \n\
void main()             \n\
{                       \n\
    int id = gl_VertexID;\n\
    texc = vert[id];    \n\
    gl_Position = pro*mod*vec4(texc, 0, 1.0);\n\
}",
* single_fs = 
"#version 330 core      \n\
uniform vec4 clr;       \n\
out vec4 color;         \n\
uniform sampler2D tex;  \n\
in vec2 texc;           \n\
void main()             \n\
{                       \n\
    color = clr*texture2D(tex, texc);\n\
}", 
// SHADERS FOR PRIMITIVES
* prim_vs = 
"#version 330 core\n\
layout (location = 0) in vec2 pos;\n\
uniform mat4 mod;\n\
uniform mat4 pro;\n\
uniform vec4 clr;\n\
out vec4 f_clr;\n\
void main()\n\
{\n\
    f_clr = clr;\n\
    gl_Position = mod*pro*vec4(pos, 0, 1.0);  \n\
}",
* prim_fs =
"#version 330 core\n\
in vec4 f_clr;\n\
out vec4 color;\n\
void main()\n\
{\n\
  color = f_clr;\n\
}";

#define glExt_cast(name, type) name = (type) wglGetProcAddress(#name)


void Graphics_checkShader(GLuint sh){
    GLint isCompiled = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        GLchar* errorLog = malloc(maxLength);
        glGetShaderInfoLog(sh, maxLength, &maxLength, errorLog);

        printf("%s\n", errorLog);
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(sh); // Don't leak the shader.
        return;
    }

}
void Graphics_makeShaderProg(GLuint* sh, const char** vs, const char** fs){
    *sh = glCreateProgram();
    GLuint v = glCreateShader(GL_VERTEX_SHADER),
         f = glCreateShader(GL_FRAGMENT_SHADER);
  
    glShaderSource(v, 1, vs, NULL);
    glCompileShader(v);
    Graphics_checkShader(v);
    
    glShaderSource(f, 1, fs, NULL);
    glCompileShader(f);
    Graphics_checkShader(f);

    
    
    glAttachShader(*sh, v);
    glAttachShader(*sh, f);

    glLinkProgram(*sh);
    
}

void Graphics_SetUniforms(struct Entity *e){
    e->uniforms.pro = glGetUniformLocation(e->sh, "pro");
    e->uniforms.mod = glGetUniformLocation(e->sh, "mod");
    e->uniforms.clr = glGetUniformLocation(e->sh, "clr");
    glUseProgram(e->sh);
    glUniformMatrix4fv( e->uniforms.pro, 1, 0, Pmat);
    glUniformMatrix4fv( e->uniforms.mod, 1, 0, Mmat);
    glUniform4f( e->uniforms.clr, 1.0, 1.0, 1.0, 1.0);
    glUseProgram(0);
}


int luaopen_Graphics(lua_L)
{
    size_t t = lua_gettop(L);
    if (glewInit() != GLEW_OK){
        printf("something wrong with glew!");
    };
    glEnable(GL_PROGRAM_POINT_SIZE);
    Graphics_makeShaderProg(&Dyn.sh, &base_vs, &base_fs);
    Graphics_makeShaderProg(&Sta.sh, &single_vs, &single_fs);
    
    Graphics_makeShaderProg(&Prim.sh, &prim_vs, &prim_fs);

   
    mat4_identity(Mmat);
    mat4_identity(Pmat);
    if (!lua_getglobal(L, "_en")) lua_error(L);
    lua_getvalue(L, -1, "screen");
    lua_getvalue(L, -1, "w");
    lua_getvalue(L, -2, "h");

    mat4_Ortho(Pmat, 0, lua_tointeger(L, -2), lua_tointeger(L, -1), 0, 1, 0);
    
    Graphics_SetUniforms(&Prim);
    Graphics_SetUniforms(&Dyn);
    Graphics_SetUniforms(&Sta);
    lua_settop(L, t);

    psize = sizeof(GLdouble)*64;
    point = malloc(psize);
    
    glUseProgram(Sta.sh);  
    glGenVertexArrays(1, &Sta.vao);
    glGenBuffers(1, &Sta.vbo);

    glBindVertexArray(Sta.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Sta.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), gvertex);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(texcoords), sizeof(texcoords), gvertex+12);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), (void*)(sizeof(texcoords)));
    
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    

    
    glUseProgram(Dyn.sh);
    glGenVertexArrays(1, &Dyn.vao);
    glGenBuffers(1, &Dyn.vbo);

    glBindVertexArray(Dyn.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Dyn.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors)*2, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), gvertex);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), (void*)(sizeof(texcoords)));
    
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    
    

    glUseProgram(Prim.sh);
    glGenVertexArrays(1, &Prim.vao);
    glGenBuffers(1, &Prim.vbo);

    glBindVertexArray(Prim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, Prim.vbo);
    glBufferData(GL_ARRAY_BUFFER, psize, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), gvertex);
    
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
    
    glEnableVertexAttribArray ( 0 );

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    luaL_newlib(L, func);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "Graphics");
    lua_nameAtable(L, -2, "Graphics");
    
    luaL_newlib(L, meta);
    lua_setmetatable(L, -2);
    lua_createtable(L, 4, 4);
    lua_pushinteger(L, 1);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, 1);
    lua_seti(L, -2, 2);
    lua_pushinteger(L, 1);
    lua_seti(L, -2, 3);
    lua_pushinteger(L, 1);
    lua_seti(L, -2, 4);
    lua_setfield(L, -2, "color");

    return 0;
}

#undef glExt_cast