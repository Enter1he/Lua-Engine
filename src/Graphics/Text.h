#ifndef TEXT_H
#define TEXT_H

#include "src/Graphics/Graphics.h"

#include "include/ft2build.h"
#include "include/freetype/freetype.h"
#include "include/freetype/ftglyph.h"
#include FT_FREETYPE_H

typedef struct _textchar{
    GLuint tex;
    int w,h,top, left;
    double dx, dy;
}TextChar;

int LE_LoadText(lua_L);

// DrawText (text: table)
int LE_DrawText(lua_L);

#endif