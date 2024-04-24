//Graphics.c


#include "Graphics.h"

// #define DEBUG 1
// #undef DEBUG

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

struct Entity Dyn, Sta, Prim;

GLdouble colors[24] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}, texcoords[12], text_uv[12]; // arrays that change over time
mat4 Pmat = {};
mat4 Mmat = {};

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
    {"BatchAdd",  &LE_BatchAdd},
    {"BatchShow", &LE_BatchShow},
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
out vec4 color;         \n\
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

#define LetShaderString(name)\
    lua_getfield(L, -1, #name "_vs");\
    const char* name##_vs = lua_tostring(L,-1);\
    lua_getfield(L, -2, #name "_fs");\
    const char* name##_fs = lua_tostring(L,-1);\
    lua_pop(L,2);


int luaopen_Graphics(lua_L)
{
    size_t t = lua_gettop(L);

    lua_require(L, "modules/shaders");

    LetShaderString(base)
    LetShaderString(single)
    LetShaderString(prim)


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

#undef LetShaderString
#undef glExt_cast