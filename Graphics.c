#include "luadef.h"
#include <windows.h>


#include <GL/gl.h>

#include "include/im.h"
#include "include/im_image.h"
#include "include/im_convert.h"

#include "include/ft2build.h"
#include "include/freetype/freetype.h"
#include "include/freetype/ftglyph.h"

#define ALPHA_BLEND 1
#define COLOR_BLEND 0



typedef struct _spritecore{
    GLuint tex;
    GLuint list;

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
    lua_getfield(L, 1, "font");
    const char *folder = lua_tostring(L, -1);
    lua_getfield(L, 1, "size");
    unsigned int h = lua_tointeger(L,-1);
    lua_pop(L, 2);

    TextChar *sc = lua_newuserdata(L, 128*sizeof(TextChar));
    lua_setfield(L, 1, "core");
    
    
    FT_Library Library = 0; FT_Face face = 0;
    FT_Init_FreeType(&Library);
    FT_New_Face(Library, folder, 0, &face);
    FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);


    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    
    for (unsigned char ch = 0; ch < 128; ch++){
        
        if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
            printf("FT_Load_Glyph failed");
 
        FT_Glyph glyph;
        if(FT_Get_Glyph( face->glyph, &glyph ))
            printf("FT_Get_Glyph failed");

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
        
        free(data);
    }


    
    FT_Done_Face(face);
    FT_Done_FreeType(Library);

    return 0;
}

int LE_DrawText(lua_L)
{
    size_t len = 0;
    lua_getfield(L, 1, "value");
    luaL_checklstring(L, -1, &len);
    const char* value = lua_tostring(L, -1);
    lua_getfield(L, 1, "size");
    size_t size = lua_tointeger(L, -1);
    lua_getfield(L, 1, "core");
    TextChar *sc = lua_touserdata(L, -1);
    lua_pop(L,2);
    lua_getfield(L, 1, "color");
    lua_geti(L, 3, 1);
    lua_geti(L, 3, 2);
    lua_geti(L, 3, 3);
    
    glPushMatrix();
    glColor3d(lua_tonumber(L,-3), lua_tonumber(L,-2), lua_tonumber(L,-1));
    lua_pop(L, 4);
    lua_getfield(L, 1, "pos");
    lua_geti(L, 3, 1);
    lua_geti(L, 3, 2);
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
        if (ch != 10){
            glTranslated(px, py, 0);
            glPushMatrix();
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);

                glTranslated(0, -sc[ch].top,0);
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
                

                glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
                
                glBegin(GL_QUADS);
                    glTexCoord2d(0, sc[ch].dy); glVertex2d(0, sc[ch].h);
                    glTexCoord2d(sc[ch].dx, sc[ch].dy); glVertex2d(sc[ch].w, sc[ch].h);
                    glTexCoord2d(sc[ch].dx, 0); glVertex2d(sc[ch].w, 0);
                    glTexCoord2d(0, 0); glVertex2d(0, 0);
                glEnd();
                
                glDisable(GL_BLEND);
                glDisable(GL_TEXTURE_2D);
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
    SpriteCore *sc = lua_newuserdata(L, sizeof(SpriteCore));
    lua_setfield(L, 1, "core");
    glGenTextures(1, &sc->tex);
    
    glBindTexture(GL_TEXTURE_2D, sc->tex);
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

    
    
    imImageDestroy(im);
    
    return 0;
}

int LE_LoadSpriteSheet(lua_L)
{
    int err;
    const char* fold = lua_tostring(L, 2);
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_getfield(L, -1, fold);
    if (lua_toboolean(L, -1)){
        lua_pop(L,1);
        lua_setfield(L, 1, "src");
    }else{
        
        lua_pop(L, 3);
        lua_require(L, fold);
        lua_setfield(L, 1, "src");
    }
    lua_getfield(L, 1, "src");
    lua_getfield(L, -1, "fmt");
    lua_pushvalue(L, 2);
    lua_pushvalue(L,-2);
    
    lua_concat(L, 2);
    
    imImage *im = imFileImageLoad(lua_tostring(L, -1), 0, &err);
    lua_cleanargs(L,3);
    
    SpriteCore *sc = lua_newuserdata(L, sizeof(SpriteCore));
    lua_setfield(L, 1, "core");
    
    
    lua_getfield(L, -1, "numf");
    glGenTextures(1, &sc->tex);
    
    
    sc->list = glGenLists(lua_tointeger(L, -1));

    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int glformat;
    char* gldata = imImageGetOpenGLData(im, &glformat);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, gldata);
    

    lua_pop(L,1);
    lua_getfield(L, -1, "anim");
    
    lua_len(L, -1);
    int len = lua_tointeger(L, -1);
    lua_pop(L,1);
    int blend = lua_tointeger(L, 3);
    int SRC = (blend)? GL_SRC_ALPHA : GL_SRC_COLOR, ONE = (blend)? GL_ONE_MINUS_SRC_ALPHA : GL_ONE_MINUS_SRC_COLOR;
    for(int i = 1; i <= len; i++){
        
        lua_geti(L, -1, i);
        
        lua_geti(L, -1, 5);
        
        
        int la = (lua_isnil(L,-1))? 1 : lua_tointeger(L, -1);
        int n = (i-1)*la-1;
        lua_pop(L,1);
        
        lua_geti(L, -1, 1);
        lua_geti(L, -2, 2);
        lua_geti(L, -3, 3);
        lua_geti(L, -4, 4);
        
        double a1 = lua_tonumber(L,-4), a2 = lua_tonumber(L,-3), a3 = lua_tonumber(L,-2), a4 = lua_tonumber(L,-1);
        lua_pop(L, 5);
        
        for (int j = 1; j <= la; j++){
            
            glNewList(sc->list+j+n , GL_COMPILE);
                
                if (blend > -1) {
                    glEnable(GL_BLEND);
                    glBlendFunc(SRC, ONE);
                };

                glBindTexture(GL_TEXTURE_2D, sc->tex);
                glBegin(GL_QUADS);
                    glTexCoord2d(a1 + a3*(j-1), a2); glVertex2d(0, 0);
                    glTexCoord2d(a3*j, a2);          glVertex2d(1, 0);
                    glTexCoord2d(a3*j, a4);          glVertex2d(1, -1);
                    glTexCoord2d(a1 + a3*(j-1), a4); glVertex2d(0, -1);
                glEnd();

                if (blend > -1) 
                    glDisable(GL_BLEND);
                 
            glEndList();
            
        } 
        
    }
    
    
    lua_getfield(L, 1, "size");
    
    lua_getfield(L, -3, "w");
    lua_seti(L, -2, 1);
    
    lua_getfield(L, -3, "h");
    lua_seti(L, -2, 2);
    

    imImageDestroy(im);

    return 0;
}

const double 
texcoord[] = {
    0, 0,
    1, 0,
    1,-1,
    0,-1    
},
verts[] = {
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1
};
#define v_array 6

int LE_DrawSprite(lua_L){
    lua_cleanargs(L, 1);
    lua_getfield(L, 1, "pos");
    lua_getfield(L, 1, "size");
    lua_getfield(L, 1, "origin");
    

    lua_geti(L, 2, 1);       //Sprite pos x
    lua_geti(L, 2, 2);       //Sprite pos y
    lua_geti(L, 3, 1);       //Sprite width
    lua_geti(L, 3, 2);       //Sprite height
    lua_geti(L, 4, 1);       //Sprite origin pos x
    lua_geti(L, 4, 2);       //Sprite origin pos y
    
    
    lua_Number w = lua_tonumber(L,7), h = lua_tonumber(L,8);
    lua_Number x = lua_tonumber(L,5), xoff = w*lua_tonumber(L,9), 
               y = lua_tonumber(L,6), yoff = h*lua_tonumber(L,10);
    
    lua_pop(L, 9);


    lua_getfield(L, 1, "color");
    lua_geti(L,2,1); lua_geti(L,2,2); lua_geti(L,2,3);
    
    lua_getfield(L, 1, "core");
    SpriteCore *sc = lua_touserdata(L, -1);
    
    lua_pop(L,1);
    
    lua_getfield(L, 1, "angle");

    
    
    glPushMatrix();
        glColor3d(lua_tonumber(L,3), lua_tonumber(L,4), lua_tonumber(L,5));
        glTranslated(x,y,0);
        glRotated(lua_tonumber(L,-1), 0, 0, 1);
        glTranslated(xoff, -yoff,0);
        glScaled(w,h,0);
        
        
#ifdef v_array
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, sc->tex);
#ifdef v_array
            glVertexPointer(2, GL_DOUBLE, 0, verts);
            glTexCoordPointer(2, GL_DOUBLE, 0, texcoord);
            glDrawArrays(GL_QUADS, 0, 4);
#else
            glBegin(GL_QUADS);
                glTexCoord2d(0, 0); glVertex2d(0, 0);
                glTexCoord2d(1, 0); glVertex2d(1, 0);
                glTexCoord2d(1,-1); glVertex2d(1, 1);
                glTexCoord2d(0,-1); glVertex2d(0, 1);
            glEnd();
#endif
            
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);   
#ifdef v_array
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
    glColor3d(1, 1, 1);
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

int LE_DrawSpriteSheet(lua_L)
{
    lua_getfield(L, 1, "core");
    if (lua_isnil(L,1)) return 0;
    SpriteCore *sc = lua_touserdata(L, -1);
    
    lua_pop(L,1);
    
    
    lua_getfield(L, 1, "pos");
    lua_geti(L, 2, 1);
    lua_geti(L, 2, 2);
    
    double x = lua_tonumber(L, -2), y = lua_tonumber(L, -1);
    
    lua_getfield(L, 1, "color");
    lua_geti(L, 5, 1);
    lua_geti(L, 5, 2);
    lua_geti(L, 5, 3);
    double c1 = lua_tonumber(L, -3), c2 = lua_tonumber(L, -2), c3 = lua_tonumber(L, -1);
    
    lua_getfield(L, 1, "origin");
    lua_geti(L, 9, 1);
    lua_geti(L, 9, 2);
    lua_getfield(L, 1, "size");
    lua_geti(L, 12, 1);
    lua_geti(L, 12, 2);
    
    double w = lua_tonumber(L, -2), h = lua_tonumber(L,-1);
    double offy = w*lua_tonumber(L, -4), offx = h*lua_tonumber(L, -5);
    
    lua_getfield(L, 1, "angle");
    lua_getfield(L, 1, "frame");
    int frame = lua_tointeger(L, -1); double angle = lua_tonumber(L, -2);
    
    
    lua_pop(L, 15);
    
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3d(c1, c2, c3);
        glTranslated(x,y,0);
        glRotated(angle, 0,0,1);
        glTranslated(offx, -offy,0);
        glScaled(w, h, 0);
        glBindTexture(GL_TEXTURE_2D, sc->tex);   
        glCallList( sc->list + frame);
        glDisable(GL_TEXTURE_2D);
        glColor3d(1, 1, 1);
    glPopMatrix();

    return 0;
}

static const struct luaL_Reg func[] = {
    
    {"LoadText", &LE_LoadText},
    {"LoadSprite", &LE_LoadSprite},
    {"LoadSpriteSheet", &LE_LoadSpriteSheet},
    
    {"DrawText", &LE_DrawText},
    {"DrawSprite", &LE_DrawSprite},
    {"DrawSpriteSheet", &LE_DrawSpriteSheet},
    
    {"DrawPolygon", &LE_DrawPolygon},

    {NULL, NULL}
};

int LUA_DLL luaopen_Graphics(lua_L)
{
    
    luaL_newlib(L, func);
    
    return 1;
}