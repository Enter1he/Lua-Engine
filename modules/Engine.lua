--engine
_G.new = setmetatable -- used in class creation, so I made it shoter
_G.iup = require"iuplua"; require"iupluagl" -- adds GlCanvas to iup
_G.gl = require"luagl"
_G.glu = require"luaglu"
_G.im = require"imlua"
_G.Char = require"modules/Char"

require"modules/Controls"



iup.SetGlobal("UTF8MODE", "Yes")
local AddScene = function(fold)
    local back = require(fold)
    package.loaded[fold] = nil
    return back
end

local Stoxy = function(s) -- converts String char pos to XYCoords
    local i = s:find("x")
    return tonumber(s:sub(1,i-1)), tonumber(s:sub(i+1))
end 

local publ, keydown = {}, ""

Engine = {dt = 1/60}
Engine.Scenes = {
    [0] = AddScene"Scenes/Test";
}
local TestLevel = Engine.Scenes[0]
--[[Final version
    Engine.Scenes = {
        ..some Scenes here..
    }
    local CurrentLevel = Engine.Scenes[1]
    then we change CurrentLevel with 
    
]]
Engine.output = iup.glcanvas{
    buffer="DOUBLE"; 
    rastersize = "256x240";
    
}

Engine.LoopTime = iup.timer{time = 10; run = "no"}

Engine.dialog = iup.dialog
{
    
    Engine.output;
    
    title="ASCII Console"; 
    size="640x320"; -- initial size
    icon=0; -- use the Lua icon from the executable in Windows
    
}


Engine.Load = function(self)
    publ = self
end

Engine.output.map_cb = function(self)
    iup.GLMakeCurrent(self)
    
    TestLevel:Load()
end
local preaction
Engine.output.action = function(self)
    iup.GLMakeCurrent(self)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    local t = os.clock()
    TestLevel:Draw()
    
    self.dt = os.clock()- t
    self.dt = (self.dt >= "0.0") and self.dt or 0.015
    
    gl.Flush()
    iup.GLSwapBuffers(self)
    
    
end



Engine.output.resize_cb = function(self, width, height)

    iup.GLMakeCurrent(self)

    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, width, 0, height, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()

end

Engine.LoopTime.action_cb = function(self)
    
    iup.LoopStep()
    iup.Update(publ.output)
    -- local s = iup.GetGlobal('CURSORPOS')
    -- local x, y = Stoxy(s)
    TestLevel = TestLevel:Update(keydown, publ) or TestLevel
    
    
end

Engine.output.keypress_cb = function(self, key, press)
    
    local k = utf8.char(key)
    --simulating onkeydown event
    if press == 1 then
        
        keydown =(keydown:match(k)) and keydown or keydown.." "..k
        
    else

        keydown = (keydown:match(k)) and keydown:gsub(" "..k, "") or keydown
    
    end
end




Engine.Loop = function(self)
    self.dialog:showxy(0,0)
    iup.SetFocus(self.output)
    
    self.LoopTime.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
      end
end

Engine.__index = Engine

return Engine