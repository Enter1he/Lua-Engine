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
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
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
gvertex[] = {
    0, 0,
    1, 0,
    1,-1,
    0,-1,
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
GLuint vxVBO, tcVBO, gVBO;
GLuint vao;
GLdouble colors[16], texcoords[8];

GLint color_idx[] = {
    0,1,2
};


typedef struct _spritecore{
    GLuint tex;
    size_t width, height;
    
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
    lua_setfield(L, 1, "_sprite");
    
    
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
    lua_getvalue(L, 1, "_sprite");
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
                
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glTranslated(0, -sc[ch].top,0);

                glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
                glColorPointer(4, GL_DOUBLE, 0, colors);
                
                glTexCoordPointer(2, GL_DOUBLE, 0, texcoords);
                glScaled(sc[ch].w, sc[ch].h,0);
                
                glVertexPointer(2, GL_DOUBLE, 0, tverts);
                

                glDrawArrays(GL_QUADS, 0, 4);
                
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();
            
            
        }
    };
    
    glPopMatrix();

    return 0;
}


int LE_LoadSingleSprite(lua_L)
{
    int err;
    lua_settop(L, 2);
    imImage *im = imFileImageLoad(lua_tostring(L, 2), 0, &err);
    SpriteCore *sc = lua_newuserdata(L, sizeof(SpriteCore));
    lua_setfield(L, 1, "_sprite");
    glGenTextures(1, &sc->tex);
    
    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int glformat;
    GLubyte* gldata = imImageGetOpenGLData(im, &glformat);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, gldata);
    
    sc->height = im->height, sc->width = im->width;
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

int LE_DrawSingleSprite(lua_L){
    
    if (!lua_getfield(L, 1, "visible")) return 0;
    lua_settop(L, 1);
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
    
    lua_settop(L, 1);


    lua_getvalue(L, 1, "color");
    lua_getidx(L,2,1); lua_getidx(L,2,2); lua_getidx(L,2,3); lua_getidx(L,2,4);
    colors[0] = colors[4] = colors[8] = colors[12] = lua_tonumber(L,3);
    colors[1] = colors[5] = colors[9] = colors[13] = lua_tonumber(L,4);
    colors[2] = colors[6] = colors[10] = colors[14] = lua_tonumber(L,5);
    colors[3] = colors[7] = colors[11] = colors[15] = lua_tonumber(L,6);
    
    
    lua_settop(L, 1);

    lua_getvalue(L, 1, "_sprite");
    SpriteCore *sc = lua_touserdata(L, -1);
    
    lua_pop(L,1);
    
    lua_getvalue(L, 1, "angle");

    
    
    glPushMatrix();
        
        
        glTranslated(x+xoff,y-yoff,0);
        glRotated(lua_tonumber(L,-1), 0, 0, 1);
        
        glScaled(w,h,0);
        
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
            
            glBindTexture(GL_TEXTURE_2D, sc->tex);

            
            glBindBuffer(GL_ARRAY_BUFFER, gVBO);
            glTexCoordPointer(2, GL_DOUBLE, 0, (void*)(sizeof(gvertex)>>1));
            glVertexPointer(2, GL_DOUBLE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glColorPointer(4,GL_DOUBLE,0,colors);
            glDrawArrays(GL_QUADS, 0, 4);
            
        glBindTexture(GL_TEXTURE_2D, 0);  
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
    
    return 0;
}


int LE_LoadSpriteSheet(lua_L)
{
    int err;
    lua_settop(L,2);
    
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
    lua_setfield(L, 1, "_sprite");
    imImage *im = imFileImageLoad(lua_tostring(L, -1), 0, &err);
    lua_settop(L,3);
    
    glGenTextures(1, &sc->tex);
    

    glBindTexture(GL_TEXTURE_2D, sc->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int glformat;
    GLubyte *buff = imImageGetOpenGLData(im, &glformat);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, buff);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    sc->height = im->height, sc->width = im->width;

    imImageDestroy(im);

    return 0;
}

int Sp_GetOSize(lua_L){
    lua_getvalue(L, 1, "_sprite");
    SpriteCore * sc = lua_touserdata(L, -1);
    lua_createtable(L, 2, 2);
    lua_pushinteger(L, sc->width);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, sc->height);
    lua_seti(L, -2, 2);
    return 1;
}

int Sp_SetOSize(lua_L){
    lua_getvalue(L, 1, "_sprite");
    SpriteCore * sc = lua_touserdata(L, -1);
    lua_getvalue(L, 1, "size");
    lua_pushinteger(L, sc->width);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, sc->height);
    lua_seti(L, -2, 2);
    return 1;
}




int LE_DrawSpriteSheet(lua_L)
{
    if (!lua_getfield(L, 1, "visible")) return 0;
    
    lua_getvalue(L, 1, "_sprite");
    
    SpriteCore *sc = lua_touserdata(L, -1);
    
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
    
    
    lua_settop(L, 1);
    
    lua_getvalue(L, 1, "src");
    
    lua_getvalue(L, -1, "anim");
    lua_getidx(L, -1, anim);
    lua_getidx(L, -1, 1);
    lua_getidx(L, -2, 2);
    lua_getidx(L, -3, 3);
    lua_getidx(L, -4, 4);
    
    int tw = lua_tointeger(L, -2), th = lua_tointeger(L, -1);
    int tx = lua_tointeger(L, -4) + frame*tw, ty = lua_tointeger(L, -3); 
    double ox = (double)tw/sc->width, oy = (double)th/sc->height;

    texcoords[0] = (double)tx/sc->width; texcoords[1] = -(double)ty/sc->height; 
    texcoords[2] = texcoords[0] + ox;        texcoords[3] = texcoords[1]; 
    texcoords[4] = texcoords[2];             texcoords[5] = texcoords[1] - oy; 
    texcoords[6] = texcoords[0];             texcoords[7] = texcoords[5]; 

    glPushMatrix();
        
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glTranslated(x,y,0);
        glRotated(angle, 0,0,1);
        glTranslated(offx, -offy,0);
        

        glBindTexture(GL_TEXTURE_2D, sc->tex);
        
        
        glScaled(w, h, 0);
        
        
        glTexCoordPointer(2, GL_DOUBLE, 0, texcoords);
        glColorPointer(4, GL_DOUBLE, 0, colors);

        glBindBuffer(GL_ARRAY_BUFFER, vxVBO);
        glVertexPointer(2, GL_DOUBLE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        
        glDrawArrays(GL_QUADS, 0, 4);
        
        
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
    lua_getvalue(L, 1, "blend");
    int check = lua_tointeger(L, -1);
    int src = (check) ? GL_SRC_COLOR : GL_SRC_ALPHA, one = (check) ? GL_ONE_MINUS_SRC_COLOR : GL_ONE_MINUS_SRC_ALPHA;

    lua_pop(L, 7);

    lua_len(L,2);
    
    for (int i = 1; i <= lua_tointeger(L,-1); i++){
        lua_geti(L, 2, i);
        
        if(!lua_getfield(L,-1, "Draw") ){
            lua_pop(L,2);
            continue;
        }
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(src, one);
        lua_rotate(L, lua_gettop(L)-1, 1);
        lua_pcall(L, 1, 0, 0);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }
    
    glPopMatrix();
    return 0;
}

int LE_DrawScene(lua_L){
    
    

    lua_getvalue(L, 1, "Draw");
    lua_pushvalue(L, 1);
    lua_pcall(L,1,0,0);
    
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

double *point = 0; size_t psize= 0;
int LE_DrawCircle(lua_L){
    int top = lua_gettop(L);
    if (top==4){
        top = -top;
        double px = lua_tonumber(L, ++top), py = lua_tonumber(L, ++top);
        double radius = lua_tonumber(L, ++top);
        int step = 2*radius - 1 ;
        
        lua_getglobal(L, "Graphics");
        lua_getvalue(L, -1, "color");
        lua_getidx(L,-1, 1);
        lua_getidx(L,-2, 2);
        lua_getidx(L,-3, 3);
        lua_getidx(L,-4, 4);
        glColor4d(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));

        
         
        double angle = twopi/step;
        GLenum mode = (lua_toboolean(L,1))? GL_POLYGON : GL_LINE_LOOP;
        if (!point){
            psize = sizeof(double)*2*step;
            point = malloc(psize);
        } else {
            if (psize < sizeof(double)*2*step) {
                psize = sizeof(double)*2*step;
                realloc(point, psize);
            }
        }  
        int j = 0;
        for (int i = step; i > 0; i--){
            double x = radius*apcos(i*angle), y = radius*apsin(i*angle);
            point[j] = px+x; point[++j] = py+y;  
            j++;
        }
        
        
        
        
        glEnableClientState(GL_VERTEX_ARRAY);

        
        
        glVertexPointer(2, GL_DOUBLE, 0, point);
        glDrawArrays(mode, 0, step);

        
        glDisableClientState(GL_VERTEX_ARRAY);
        
        
    } else
        return 1;
    return 0;
}

int LE_DrawRect(lua_L){
    int top = lua_gettop(L);
    if (top >= 4){
        GLenum mode = (lua_toboolean(L,1))? GL_QUADS: GL_LINE_LOOP;
        top = -top;
        double x = lua_tonumber(L,++top),
               y = lua_tonumber(L,++top),
               x2 = lua_tonumber(L,++top),
               y2 = lua_tonumber(L,++top);
        lua_getglobal(L, "Graphics");
        lua_getvalue(L, -1, "color");
        lua_getidx(L,-1, 1);
        lua_getidx(L,-2, 2);
        lua_getidx(L,-3, 3);
        lua_getidx(L,-4, 4);
        
        colors[12] = colors[8] = colors[4] = colors[0] = lua_tonumber(L, -4),
        colors[13] = colors[9] = colors[5] = colors[1] = lua_tonumber(L, -3),
        colors[14] = colors[10] = colors[6] = colors[2] = lua_tonumber(L, -2),
        colors[15] = colors[11] = colors[7] = colors[3] = lua_tonumber(L, -1);
        if (!point) {
            psize = sizeof(double)*8;
            point = malloc(psize);
        }
        if (psize < sizeof(double)*8){
            psize = sizeof(double)*8;
            point = realloc(point, psize);
        }
        point[0] = x; point[1] = y;
        point[2] = x2; point[3] = y;
        point[4] = x2; point[5] = y2;
        point[6] = x; point[7] = y2;
        
        
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        
        glColorPointer(4, GL_DOUBLE, 0, colors);
        glVertexPointer(2, GL_DOUBLE, 0, point);
        glDrawArrays(mode, 0, 4);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
    }
    return 0;
}

int ModuleGC(lua_L){
    glDeleteBuffers(1, &gVBO);
    glDeleteBuffers(1, &vxVBO);
    
    if (point) free(point);
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


    {"LoadSpriteSheet", &LE_LoadSpriteSheet},
    {"GetSize", &Sp_GetOSize},
    {"ResetSize", &Sp_SetOSize},
    {"DrawSpriteSheet", &LE_DrawSpriteSheet},
    

    {"DrawLayer", &LE_DrawLayer},
    {"DrawScene", &LE_DrawScene},


    {"DrawCircle", &LE_DrawCircle},
    {"DrawRect", &LE_DrawRect},


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
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) wglGetProcAddress("glDeleteVertexArrays");

    glewInit();
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO );
    
    size_t gvs = sizeof(gvertex)>>1;
    glBufferData(GL_ARRAY_BUFFER, sizeof(gvertex), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gvs, gvertex);
    glBufferSubData(GL_ARRAY_BUFFER, gvs, gvs, gvertex+8);

    

    glGenBuffers(1, &vxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vxVBO);
    glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLdouble), gvertex+8, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    luaL_newlib(L, func);
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
    
    return 1;
}