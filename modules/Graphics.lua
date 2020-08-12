gl = require"luagl"
_en ={
    0.016666666666667;
    {w = 640, h = 480};
}

local function InitMain(width, height)
    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, _en[2].w, 0, _en[2].h, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()
end

local function DrawMain(CurrentScene)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    
    CurrentScene:Draw()
    
    gl.Flush()
end

return {
    InitMain = InitMain;
    DrawMain = DrawMain;
}