-- shaders.lua
local shaders = {
    base_vs = -- BASE SHADERS FOR IMAGES
    [[  
        attribute vec2 pos; 
        attribute vec2 txc; 
        uniform vec4 clr; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        varying vec4 f_clr; 
        varying vec2 f_txc; 
        void main() 
        { 
            f_clr = clr; 
            f_txc = txc; 
            gl_Position = pro*mod*vec4(pos, 0, 1.0);   
        }
    ]];

    base_fs = 
    [[
        precision mediump float;
        varying vec4 f_clr; 
        varying vec2 f_txc; 
        uniform sampler2D tex; 
        void main() 
        { 
            gl_FragColor = f_clr*texture2D(tex, f_txc); 
        }
    ]];

    single_vs = 
    [[
               
        attribute vec2 pos;           
        uniform mat4 mod;        
        uniform mat4 pro;                  
        varying vec2 texc;           
        void main()              
        {                        
            texc = pos;     
            gl_Position = pro*mod*vec4(texc, 0, 1.0); 
        }
    ]];

    single_fs = [[
        precision mediump float;
        uniform vec4 clr;        
        uniform sampler2D tex;   
        varying vec2 texc;            
        void main()              
        {                        
            gl_FragColor = clr*texture2D(tex, texc); 
        }
    ]];
    -- SHADERS FOR PRIMITIVES
    prim_vs = [[
         
        attribute vec2 pos; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        uniform vec4 clr; 
        varying vec4 f_clr; 

        void main() 
        { 
            f_clr = clr; 
            gl_Position = mod*pro*vec4(pos, 0, 1.0);   
        }
    ]];

    prim_fs =[[
        precision mediump float;
        varying vec4 f_clr; 
        void main() 
        { 
            gl_FragColor = f_clr; 
        }
    ]];

    text_vs = -- TEXT SHADERS
    [[   
        attribute vec2 pos; 
        attribute vec2 txc;
        uniform vec4 clr; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        varying vec2 f_txc; 
        void main() 
        {  
            f_txc = txc; 
            gl_Position = pro*mod*vec4(pos, 0, 1.0);   
        }
    ]];

    text_fs = 
    [[    
        precision mediump float;
        varying vec2 f_txc; 
        uniform vec4 clr;
        uniform sampler2D tex; 
        void main() 
        { 
            float sample = texture2D(tex, f_txc).a;
            gl_FragColor = clr*vec4(sample, sample, sample, 1.0 ); 
        }
    ]];

    batch_vs = -- batch shaders
    [[   
        attribute vec2 pos; 
        attribute vec2 txc; 
        uniform vec4 clr; 
        uniform mat4 mod; 
        uniform mat4 pro; 
        varying vec4 f_clr; 
        varying vec2 f_txc; 
        void main() 
        { 
            f_clr = clr; 
            f_txc = txc; 
            gl_Position = pro*mod*vec4(pos, 0, 1.0);   
        }
    ]];

    batch_fs = 
    [[
        precision mediump float;
        varying vec4 f_clr; 
        varying vec2 f_txc; 
        uniform sampler2D tex; 
        void main() 
        { 
            gl_FragColor = f_clr*texture2D(tex, f_txc); 
        }
    ]];
}
return shaders