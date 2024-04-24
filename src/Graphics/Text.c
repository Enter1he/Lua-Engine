#include "src/Graphics/Text.h"




static inline int next_p2 (int a)
{
    int rval=1;
    while(rval<a) 
        rval<<=1;

    return rval;
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

    
    
    FT_Library Library = 0; FT_Face face = 0;
    FT_Init_FreeType(&Library);
    FT_New_Face(Library, font_folder, 0, &face);
    FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);

    FT_UInt num = 255;
    // FT_UInt agindx = 0;
    // FT_Get_First_Char(face, &agindx);
    
    // for (agindx;;num++){
    //     FT_Get_Next_Char(face, agindx, &agindx);
    // }

    TextChar *sc = lua_newuserdata(L, num*sizeof(TextChar));
    lua_setfield(L, 1, "_drawable");
    
    
    for (unsigned int ch = 0; ch < num; ch++){
        
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