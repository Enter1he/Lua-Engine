
require"luagl"
local gl = gl

local function InitMain(width, height)
    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, _en[2].w, 0, _en[2].h, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()
end

local function DrawMain(CurrentScene)
    
    gl.ClearColor(0,0,0,0)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    
    CurrentScene:Draw(gl)
    
    gl.Flush()
    
end

local function MapMain()
    gl.PixelStore(gl.UNPACK_ALIGNMENT, 1)
end

return {
    InitMain = InitMain;
    DrawMain = DrawMain;
    MapMain = MapMain;
}