#include "src/Graphics/Text.h"




static inline int next_p2 (int a)
{
    int rval=1;
    while(rval<a) 
        rval<<=1;
    return rval;
}

static int collectText(lua_L){
    Font *f = lua_touserdata(L,1);
    
    return 0;
}

lua_Table gcText[] = {
    {"__gc", &collectText},
    lua_eoT
};

static void Text_setRng(Font* f, const char *folder, int lower, int higher){
    
    TextChar *sc = f->tc;
    size_t h = f->h;
    
    FT_Library Library = 0; 
    FT_Face face = 0;
    FT_Init_FreeType(&Library);
    if(FT_New_Face(Library, folder, 0, &face)){
        Error_print("%s %s %d: FT_New_Face failed for file %s",  __FILE__, __func__ , __LINE__, folder);
    }
    
    FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);
    
    for (unsigned int ch = lower; ch < higher; ch++){
        FT_Error err;
        if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_RENDER ))
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
        


        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &sc[ch].tex);    
        
        glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (f->linear)? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (f->linear)? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap->width, bitmap->rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap->buffer);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    FT_Done_Face(face);
    FT_Done_FreeType(Library);
}

// text:Load( obj: table, linear: boolean)
int LE_LoadText(lua_L)
{
    Table *t = hvalue(index2value(L,1));
    TValue *str = lua_tablevalue(L, t, "font");
    if(!ttisstring(str)) {Error_print( "font_folder should be a string!");}

    const char *font_folder = getstr(tsvalue(str));
    
    lua_Integer i;
    tointeger(lua_tablevalue(L, t, "size"), &i);
    size_t h = (size_t)(i);

    int linear = lua_toboolean(L, 2);
    int alpha = !l_isfalse(lua_tablevalue(L, t, "alpha"));


    FT_UInt num = 128;
    Font* f = lua_newuserdata(L, sizeof(Font));
    lua_setfield(L, 1, "_drawable");
    f->tc = lua_newuserdata(L, num*sizeof(TextChar));
    f->rng = NULL;
    f->h = h;
    f->linear = linear;
    
    Text_setRng(f, font_folder, 0, num);

    return 0;
}

// DrawText (text: table)
int LE_DrawText(lua_L)
{
    size_t len = 0;

    Table *t = hvalue(index2value(L,1));
    TValue *v = lua_tablevalue(L, t, "value");
    TString *str = tsvalue(v);
    len = (size_t)(tsslen(str));
    const char* value = getstr(str);

    v = lua_tablevalue(L, t, "size");
    lua_Integer size = 0;
    tointeger(v, &size);
    
    v = lua_tablevalue(L, t, "_drawable");
    Font* f = (Font*)(touserdata(v));
    lua_assert(f);
    TextChar *sc = f->tc;
    
    float color[4];
    Table *arr = hvalue(lua_tablevalue(L, t, "color"));
    lua_arrtofloat(L, arr, color, 4);
    

    float a[2];
    arr = hvalue(lua_tablevalue(L, t, "pos"));
    lua_arrtofloat(L, arr, a, 2);

    float px = a[0], py = a[1];
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
                py = (float)(size);
                px -= (float)(lineW-sc[sp].w);
                lineW = 0;
            }else
            {
                py = 0;
                px = px;
            }
        };
        if (ch != '\n'){
            text_uv[7] = text_uv[1] = text_uv[3] = 1;//sc[ch].dy;
            text_uv[2] = text_uv[6] = text_uv[10] = 1;//sc[ch].dx;
            
            n[0] = px; n[1] = py;
            mat4_Translate2D(pop1, pop1, n);
            mat4_clone(pop2, pop1);
            n[0] = 0; n[1] = -sc[ch].top;
            mat4_Translate2D(pop2, pop2, n);
            n[0] = sc[ch].w; n[1] = sc[ch].h;
            mat4_Scale2D(pop2, pop2, n);

            glBindTexture(GL_TEXTURE_2D, sc[ch].tex);
        
            glBindVertexArray(Text.vao);
            glBindBuffer(GL_ARRAY_BUFFER, Text.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), text_uv);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(texcoords), sizeof(texcoords), gvertex);
            
            glUseProgram(Text.sh);

            glUniformMatrix4fv( glGetUniformLocation(Text.sh, "mod"), 1, 0, pop2);
            glUniform4f( glGetUniformLocation(Text.sh, "clr"), color[0], color[1], color[2], color[3]);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }
    };

    return 0;
}