-- shaders.lua
local shaders = {
    base_vs = -- BASE SHADERS FOR IMAGES
    [[  #version 330 core 
        layout (location = 1) in vec2 pos; 
        layout (location = 0) in vec2 txc; 
        uniform vec4 clr; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        out vec4 f_clr; 
        out vec2 f_txc; 
        void main() 
        { 
            f_clr = clr; 
            f_txc = txc; 
            gl_Position = pro*mod*vec4(pos, 0, 1.0);   
        }
    ]];

    base_fs = 
    [[  #version 330 core 
        in vec4 f_clr; 
        in vec2 f_txc; 
        out vec4 color; 
        uniform sampler2D tex; 
        void main() 
        { 
            color = f_clr*texture2D(tex, f_txc); 
        }
    ]];

    single_vs = 
    [[
        #version 330 core       
        vec2 vert[6] = vec2[]( 
            vec2(0, 1),          
            vec2(1, 1),          
            vec2(0, 0),          
            vec2(1, 1),          
            vec2(0, 0),          
            vec2(1, 0)           
        );                       
        uniform mat4 mod;        
        uniform mat4 pro;        
        out vec4 color;          
        out vec2 texc;           
        void main()              
        {                        
            int id = gl_VertexID; 
            texc = vert[id];     
            gl_Position = pro*mod*vec4(texc, 0, 1.0); 
        }
    ]];

    single_fs = [[
        #version 330 core       
        uniform vec4 clr;        
        out vec4 color;          
        uniform sampler2D tex;   
        in vec2 texc;            
        void main()              
        {                        
            color = clr*texture2D(tex, texc); 
        }
    ]];
    -- SHADERS FOR PRIMITIVES
    prim_vs = [[
        #version 330 core 
        layout (location = 0) in vec2 pos; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        uniform vec4 clr; 
        out vec4 f_clr; 

        void main() 
        { 
            f_clr = clr; 
            gl_Position = mod*pro*vec4(pos, 0, 1.0);   
        }
    ]];

    prim_fs =[[
        #version 330 core 
        in vec4 f_clr; 
        out vec4 color; 
        void main() 
        { 
            color = f_clr; 
        }
    ]];
}
return shaders