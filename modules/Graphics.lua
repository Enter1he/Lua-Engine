gl = require"luagl"
_en ={
    dt = 0.016666666666667;
    screen = {w = 640, h = 480};
}

local function InitMain(width, height)
    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, _en.screen.w, 0, _en.screen.h, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()
end

local function DrawMain(CurrentLevel)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    
    CurrentLevel:Draw()
    
    gl.Flush()
end

return {
    InitMain = InitMain;
    DrawMain = DrawMain;
}