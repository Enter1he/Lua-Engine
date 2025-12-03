#ifndef TEXT_H
#define TEXT_H


#include "src/Graphics/Graphics.h"

#ifdef _WIN32
#include "include/ft2build.h"
#include "include/freetype/freetype.h"
#include "include/freetype/ftglyph.h"
#endif

#ifdef __EMSCRIPTEN__
#include <ft2build.h>
#endif

#include FT_FREETYPE_H
#include FT_GLYPH_H

typedef struct _utf8range{
    int low, up;
}U8Range;

typedef struct _textchar{
    GLuint tex;
    int w,h,top, left;
    float dx, dy;
}TextChar;

typedef struct _font{
    size_t h;
    int linear;
    TextChar *tc;
    U8Range *rng;
}Font;



int LE_LoadText(lua_L);

// DrawText (text: table)
int LE_DrawText(lua_L);

#endif