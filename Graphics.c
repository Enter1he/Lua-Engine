#include "luadef.h"
#include <windows.h>


#include <GL/gl.h>

#include <GL/glext.h>

PFNGLGENBUFFERSPROC glGenBuffers; 
PFNGLBINDBUFFERPROC glBindBuffer; 
PFNGLBUFFERDATAPROC glBufferData; 
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;


#include "include/im.h"
#include "include/im_image.h"
#include "include/im_convert.h"

#include "include/ft2build.h"
#include "include/freetype/freetype.h"
#include "include/freetype/ftglyph.h"

#define ALPHA_BLEND 1
#define COLOR_BLEND 0

const GLdouble 
gtexcoord[] = {
    0, 0,
    1, 0,
    1,-1,
    0,-1    
},
gverts[] = {
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1
},
tverts[] = {
    0, 1, 
    1, 1, 
    1, 0, 
    0, 0
};
GLuint vxVBO, tcVBO, qVBO;
GLdouble colors[16], texcoords[8];

GLint color_idx[] = {
    0,1,2
};


typedef struct _spritecore{
    GLuint tex,format;
    imImage *im;
    
    

}SpriteCore;


typedef struct _textchar{
    GLuint tex;
    int w,h,top;
    double dx, dy;
}TextChar;



inline int next_p2 (int a)
{
    
    int rval=1;
    while(rval<a) 
        rval<<=1;

    return rval;
}

int LE_LoadText(lua_L)
{
    lua_getvalue(L, 1, "font");
    const char *folder = lua_tostring(L, -1);

    lua_getvalue(L, 1, "size");
    unsigned int h = lua_tointeger(L,-1);

    lua_pop(L, 2);

    TextChar *sc = lua_newuserdata(L, 128*sizeof(TextChar));
    lua_setfield(L, 1, "core");
    
    
    FT_Library Library = 0; FT_Face face = 0;
    FT_Init_FreeType(&Library);
    FT_New_Face(Library, folder, 0, &face);
    FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);

    
    for (unsigned char ch = 0; ch < 128; ch++){
        
        if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
            printf("%s %d: FT_Load_Glyph failed",  __FILE__ , __LINE__);
 
        FT_Glyph glyph;
        if(FT_Get_Glyph( face->glyph, &glyph ))
            printf("%s %d: FT_Get_Glyph failed", __FILE__ , __LINE__);

        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        FT_Bitmap *bitmap= &bitmap_glyph->bitmap;


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
                data[2*(j+i*width)] = data[2*(j+i*width)+1];
                
            };
        };
        glGenTextures(1, &sc[ch].tex);    
        
        glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi( GL_TEXTURE_ENV,  GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        free(data);
    }


    
    FT_Done_Face(face);
    FT_Done_FreeType(Library);

    return 0;
}

int LE_DrawText(lua_L)
{
    size_t len = 0;
    lua_getvalue(L, 1, "value");
    luaL_checklstring(L, -1, &len);
    const char* value = lua_tostring(L, -1);
    lua_getvalue(L, 1, "size");
    size_t size = lua_tointeger(L, -1);
    lua_getvalue(L, 1, "core");
    TextChar *sc = lua_touserdata(L, -1);
    lua_pop(L,2);
    lua_getvalue(L, 1, "color");
    lua_getidx(L, 3, 1);
    lua_getidx(L, 3, 2);
    lua_getidx(L, 3, 3);
    lua_getidx(L, 3, 4);
    
    colors[0] = colors[4] = colors[8] = colors[12] = lua_tonumber(L,-4);
    colors[1] = colors[5] = colors[9] = colors[13] = lua_tonumber(L,-3);
    colors[2] = colors[6] = colors[10] = colors[14] = lua_tonumber(L,-2);
    colors[3] = colors[7] = colors[11] = colors[15] = lua_tonumber(L,-1);
    glPushMatrix();
    

    lua_pop(L, 5);
    lua_getvalue(L, 1, "pos");
    lua_getidx(L, 3, 1);
    lua_getidx(L, 3, 2);
    int px = lua_tonumber(L,-2), py = lua_tonumber(L,-1);
    unsigned int lineW = 0;
    for (int i = 0; i < len; i++){
        char ch = value[i]; 
        
        if (i){
            char pch = value[i-1];
            
            px = (pch == 32) ? sc[37].w: sc[pch].w;
            lineW += px;
            if (pch=='\n'){
                py = size;
                px = -lineW+px;
                lineW = 0;
            }else{
                py = 0;
                px = px;
            }

        };
        if (ch != '\n'){
            texcoords[0] = 0,         texcoords[1] = sc[ch].dy;
            texcoords[2] = sc[ch].dx, texcoords[3] = sc[ch].dy;
            texcoords[4] = sc[ch].dx, texcoords[5] = 0;
            texcoords[6] = 0,         texcoords[7] = 0; 
            glTranslated(px, py, 0);
            glPushMatrix();
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glTranslated(0, -sc[ch].top,0);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                

                glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
                glColorPointer(4, GL_DOUBLE, 0, colors);
                
                glTexCoordPointer(2, GL_DOUBLE, 0, texcoords);
                glScaled(sc[ch].w, sc[ch].h,0);
                
                glVertexPointer(2, GL_DOUBLE, 0, tverts);
                

                glDrawArrays(GL_QUADS, 0, 4);
                // glBegin(GL_QUADS);
                //     glTexCoord2d(0, sc[ch].dy);         glVertex2d(0, sc[ch].h);
                //     glTexCoord2d(sc[ch].dx, sc[ch].dy); glVertex2d(sc[ch].w, sc[ch].h);
                //     glTexCoord2d(sc[ch].dx, 0);         glVertex2d(sc[ch].w, 0);
                //     glTexCoord2d(0, 0);                 glVertex2d(0, 0);
                // glEnd();
                
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisable(GL_BLEND);
                glDisable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();
            
            
        }
    };
    
    glPopMatrix();

    return 0;
}


int LE_LoadSprite(lua_L)
{
    int err;
    lua_cleanargs(L, 2);
    imImage *im = imFileImageLoad(lua_tostring(L, 2), 0, &err);
    GLuint *core = lua_newuserdata(L, sizeof(GLuint));
    lua_setfield(L, 1, "core");
    glGenTextures(1, core);
    
    glBindTexture(GL_TEXTURE_2D, *core);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int glformat;
    char* gldata = imImageGetOpenGLData(im, &glformat);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, gldata);
    
    
    lua_createtable(L, 2, 2);
    
    lua_pushinteger(L, im->width);
    lua_seti(L, -2, 1);

    lua_pushinteger(L, im->height);
    lua_seti(L, -2, 2);
    
    lua_setfield(L, 1, "size");

    glBindTexture(GL_TEXTURE_2D, 0);
    
    imImageDestroy(im);
    
    return 0;
}

int LE_DrawSprite(lua_L){
    
    lua_cleanargs(L, 1);
    lua_getvalue(L, 1, "pos");
    lua_getvalue(L, 1, "size");
    lua_getvalue(L, 1, "origin");
    

    lua_getidx(L, 2, 1);       //Sprite pos x
    lua_getidx(L, 2, 2);       //Sprite pos y
    lua_getidx(L, 3, 1);       //Sprite width
    lua_getidx(L, 3, 2);       //Sprite height
    lua_getidx(L, 4, 1);       //Sprite origin pos x
    lua_getidx(L, 4, 2);       //Sprite origin pos y
    
    
    lua_Number w = lua_tonumber(L,7), h = lua_tonumber(L,8);
    lua_Number x = lua_tonumber(L,5), xoff = w*lua_tonumber(L,9), 
               y = lua_tonumber(L,6), yoff = h*lua_tonumber(L,10);
    
    lua_pop(L, 9);


    lua_getvalue(L, 1, "color");
    lua_getidx(L,2,1); lua_getidx(L,2,2); lua_getidx(L,2,3); lua_getidx(L,2,4);
    colors[0] = colors[4] = colors[8] = colors[12] = lua_tonumber(L,3);
    colors[1] = colors[5] = colors[9] = colors[13] = lua_tonumber(L,4);
    colors[2] = colors[6] = colors[10] = colors[14] = lua_tonumber(L,5);
    colors[3] = colors[7] = colors[11] = colors[15] = lua_tonumber(L,6);
    
    
    

    lua_getvalue(L, 1, "core");
    GLuint *core = lua_touserdata(L, -1);
    
    lua_pop(L,1);
    
    lua_getvalue(L, 1, "angle");

    
    
    glPushMatrix();
        
        
        glTranslated(x,y,0);
        glRotated(lua_tonumber(L,-1), 0, 0, 1);
        glTranslated(xoff, -yoff,0);
        glScaled(w,h,0);
        
        
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, *core);

            glBindBuffer(GL_ARRAY_BUFFER, qVBO);
            glTexCoordPointer(2, GL_DOUBLE, 0, (void*)sizeof(gtexcoord));
            glVertexPointer(2, GL_DOUBLE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // glTexCoordPointer(2, GL_DOUBLE, 0, gtexcoord);
            // glVertexPointer(2, GL_DOUBLE, 0, gverts);
            glColorPointer(4,GL_DOUBLE,0,colors);
            glDrawArrays(GL_QUADS, 0, 4);
            
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);   
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
    
    return 0;
}


int LE_LoadSpriteSheet(lua_L)
{
    int err;
    lua_cleanargs(L,2);
    
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

    SpriteCore *sc = lua_newuserdata(L, sizeof(SpriteCore));
    lua_setfield(L, 1, "core");
    sc->im = imFileImageLoad(lua_tostring(L, -1), 0, &err);
    lua_cleanargs(L,3);
    
    
    
    
    glGenTextures(1, &sc->tex);
    

    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    
    GLubyte *buff = imImageGetOpenGLData(sc->im, &sc->format);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, sc->im->width, sc->im->height, 0, sc->format, GL_UNSIGNED_BYTE, buff);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    



    return 0;
}

int LE_GetSpriteSheetSize(lua_L){
    lua_getvalue(L, 1, "core");
    SpriteCore * sc = lua_touserdata(L, -1);
    lua_createtable(L, 2, 2);
    lua_pushinteger(L, sc->im->width);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, sc->im->height);
    lua_seti(L, -2, 2);
    return 1;
}

int LE_DestroySpriteSheet(lua_L){
    int type = lua_type(L, 1);
    if (type == LUA_TTABLE)
        lua_getvalue(L, 1, "core");
    else 
    if (type != LUA_TUSERDATA) 
        return 0;
    SpriteCore *sc = lua_touserdata(L, -1);
    imImageDestroy(sc->im);
    return 0;
}






int LE_DrawSpriteSheet(lua_L)
{
    
    lua_getvalue(L, 1, "core");
    if (lua_isnil(L,1)) return 0;
    SpriteCore *sc = lua_touserdata(L, -1);
    
    lua_pop(L,1);
    
    
    if(lua_getfield(L, 1, "pos")){
        lua_getidx(L, 2, 1);
        lua_getidx(L, 2, 2);
    }else{
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
    }
    double x = lua_tonumber(L, -2), y = lua_tonumber(L, -1);
    
    lua_getvalue(L, 1, "color");
    lua_getidx(L, 5, 1);
    lua_getidx(L, 5, 2);
    lua_getidx(L, 5, 3);
    lua_getidx(L, 5, 4);
    colors[0] = colors[4] = colors[8] = colors[12] = lua_tonumber(L,-4);
    colors[1] = colors[5] = colors[9] = colors[13] = lua_tonumber(L,-3);
    colors[2] = colors[6] = colors[10] = colors[14] = lua_tonumber(L,-2);
    colors[3] = colors[7] = colors[11] = colors[15] = lua_tonumber(L,-1);
    
    lua_getvalue(L, 1, "origin");
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);
    lua_getvalue(L, 1, "size");
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);
    
    double w = lua_tonumber(L, -2), h = lua_tonumber(L,-1);
    double offy = w*lua_tonumber(L, -4), offx = h*lua_tonumber(L, -5);
    
    lua_getvalue(L, 1, "angle");
    lua_getvalue(L, 1, "frame");
    lua_getvalue(L, 1, "anim");
    int anim = lua_tointeger(L, -1), frame = lua_tointeger(L, -2); double angle = lua_tonumber(L, -3);
    
    
    lua_pop(L, 17);
    
    lua_getvalue(L, 1, "src");
    
    lua_getvalue(L, -1, "anim");
    lua_getidx(L, -1, anim);
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);
    lua_getidx(L, -3, 3);
    lua_getidx(L, -4, 4);
    
    int tw = lua_tointeger(L, -2), th = lua_tointeger(L, -1);
    int tx = lua_tointeger(L, -4) + frame*tw, ty = lua_tointeger(L, -3); 
    double ox = (double)tw/sc->im->width, oy = (double)th/sc->im->height;

    texcoords[0] = (double)tx/sc->im->width; texcoords[1] = -(double)ty/sc->im->height; 
    texcoords[2] = texcoords[0] + ox;        texcoords[3] = texcoords[1]; 
    texcoords[4] = texcoords[2];             texcoords[5] = texcoords[1] - oy; 
    texcoords[6] = texcoords[0];             texcoords[7] = texcoords[5]; 

    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glTranslated(x,y,0);
        glRotated(angle, 0,0,1);
        glTranslated(offx, -offy,0);
        

        glBindTexture(GL_TEXTURE_2D, sc->tex);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glScaled(w, h, 0);
        // Error_print("%g %d", texcoords[2], frame);
        
        glTexCoordPointer(2, GL_DOUBLE, 0, texcoords);
        glColorPointer(4, GL_DOUBLE, 0, colors);

        glBindBuffer(GL_ARRAY_BUFFER, vxVBO);
        glVertexPointer(2, GL_DOUBLE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        
        glDrawArrays(GL_QUADS, 0, 4);
        
        
        
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
    glPopMatrix();

    return 0;
}


int LE_DrawLayer(lua_L){

    lua_getvalue(L, 1, "pos");
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);

    glPushMatrix();

    glTranslated(-lua_tonumber(L, -2), -lua_tonumber(L, -1), 0);
    
    lua_getvalue(L, 1, "size");
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);

    glScaled(lua_tonumber(L, -2), lua_tonumber(L, -1), 0);

    lua_pop(L, 6);

    lua_len(L,2);
    
    for (int i = 1; i <= lua_tointeger(L,-1); i++){
        lua_geti(L, 2, i);
        
        if(!lua_getfield(L,-1, "Draw")){
            lua_pop(L,2);
            continue;
        }
        lua_rotate(L, lua_gettop(L)-1, 1);
        lua_pcall(L, 1, 0, 0);
        
        
    }
    
    glPopMatrix();
    return 0;
}


#define pi 3.14159265358979323846
#define halfpi 1.5707963267949
#define twopi 6.2831853071796
#define invtwopi 0.15915494309189
#define threehalfpi 4.7123889803847

double cos_32s(double x)
{
    const double c1= 0.99940307;
    const double c2=-0.49558072;
    const double c3= 0.03679168;
    x *= x;
    return (c1 + x*(c2 + c3 * x));
}
double apcos(double angle){
    //clamp to the range 0..2pi
    angle=angle-(int)(angle*invtwopi)*twopi;
    angle=angle>0.f?angle:-angle;

    if(angle<halfpi) return cos_32s(angle);
    if(angle<pi) return -cos_32s(pi-angle);
    if(angle<threehalfpi) return -cos_32s(angle-pi);
    return cos_32s(twopi-angle);
}
inline double apsin(double angle){
    return apcos(halfpi-angle);
}


int LE_DrawPolygon(lua_L){
    int top = lua_gettop(L);
    if (top==4){
        double range = lua_tonumber(L, -2);
        int step = lua_tointeger(L,-1);
        step = (step > 2) ? step : 3;
        double px = lua_tonumber(L, -4), py = lua_tonumber(L, -3); 
        double angle = 360.0/step;

        glBegin(GL_POLYGON);
        for (int i = step; i > 0; i--){
            double x = range*apcos(i*angle), y = range*apsin(i*angle);
            glVertex2d(px+x, py+y);
        }
        glEnd();
    } else
        return 1;
    return 0;
}

int ModuleGC(lua_L){
    glDeleteBuffers(1, &qVBO);
    glDeleteBuffers(1, &vxVBO);
    return 0;
}


static const struct luaL_Reg meta[] = {
    {"__gc", &ModuleGC},
    
    {NULL, NULL}
};

static const struct luaL_Reg func[] = {
    
    {"LoadText", &LE_LoadText},
    {"DrawText", &LE_DrawText},

    {"LoadSprite", &LE_LoadSprite},
    {"DrawSprite", &LE_DrawSprite},


    {"LoadSpriteSheet", &LE_LoadSpriteSheet},
    {"GetSize", &LE_GetSpriteSheetSize},
    {"DestroySpriteSheet", &LE_DestroySpriteSheet},
    {"DrawSpriteSheet", &LE_DrawSpriteSheet},
    

    {"DrawLayer", &LE_DrawLayer},


    {"DrawPolygon", &LE_DrawPolygon},


    {NULL, NULL}
};




int LUA_DLL luaopen_Graphics(lua_L)
{
    
    glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBufferARB");
    glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglGetProcAddress("glBufferSubData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) wglGetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");

    size_t v = sizeof(gverts), t = sizeof(gtexcoord);
    glGenBuffers(1, &qVBO);
    glBindBuffer(GL_ARRAY_BUFFER, qVBO );
    glBufferData(GL_ARRAY_BUFFER, v+t, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, v, gverts);
    glBufferSubData(GL_ARRAY_BUFFER, v, t, gtexcoord);

    glGenBuffers(1, &vxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vxVBO);
    glBufferData(GL_ARRAY_BUFFER, v, gverts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    luaL_newlib(L, func);
    luaL_newlib(L, meta);
    lua_setmetatable(L, -2);
    
    
    return 1;
}