--engine
_G.new = setmetatable -- used in class creation, so I made it shoter
_G.iup = require"iuplua"; require"iupluagl" -- adds GlCanvas to iup
_G.gl = require"luagl"
_G.glu = require"luaglu"

_G.Controls = require"modules/Controls"

iup.SetGlobal("UTF8MODE", "Yes")


local function AddScene(fold)
    local back = require(fold)
    package.loaded[fold] = nil
    return back
end

local function StoXY(s) -- converts String char pos to XYCoords
    local i = s:find("x")
    return tonumber(s:sub(1,i-1)), tonumber(s:sub(i+1))
end 



local Engine = {
    dt = 1/60;
    Scenes = require"Scenes.SceneEnum";
    output = iup.glcanvas{ -- canvas for OpenGL
        buffer="DOUBLE"; 
        rastersize = "640x480"; -- standard resolution
    
    };
    Update = iup.timer{time = 15; run = "no"};

}
Engine.dialog = iup.dialog
    {
        
        Engine.output;
        
        title="ASCII Console"; 
        size="640x480"; -- initial size
        icon=0; -- use the Lua icon from the executable in Windows
        
    };

local publ, keydown = {}, "" -- first is engine's self pointer;
-- second is for emulating keydown behaviour

local CurrentLevel = AddScene(Engine.Scenes[0])


local function Engine_Load(self)
    publ = self
end


local function output_map(self)
    iup.GLMakeCurrent(self)
    
    CurrentLevel:Load()
end



local function output_action(self)
    
    iup.GLMakeCurrent(self)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    local t = os.clock()
    CurrentLevel:Draw()
    
    self.dt = os.clock()- t
    self.dt = (self.dt >= "0.0") and self.dt or 0.015
    
    gl.Flush()
    iup.GLSwapBuffers(self)
    
end



local function output_resize_cb(self, width, height)

    iup.GLMakeCurrent(self)

    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, width, 0, height, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()

end


local function Engine_Update(self)
    
    iup.LoopStep()
    iup.Update(publ.output)
    -- local s = iup.GetGlobal('CURSORPOS')
    -- local x, y = Stoxy(s)
    CurrentLevel = CurrentLevel:Update(keydown, publ) or CurrentLevel
    
    
end


local function output_KeyState(self, key, press)
    
    local k = utf8.char(key)
    --simulating onkeydown event
    if press == 1 then
        
        keydown =(keydown:match(k)) and keydown or keydown.." "..k
        
    else

        keydown = (keydown:match(k)) and keydown:gsub(" "..k, "") or keydown
    
    end
end




local function Engine_Loop(self)
    self.dialog:showxy(0,0)
    iup.SetFocus(self.output)
    
    self.Update.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
      end
end

Engine.output.map_cb = output_map
Engine.output.action = output_action
Engine.output.resize_cb = output_resize_cb
Engine.Update.action_cb = Engine_Update
Engine.output.keypress_cb = output_KeyState
Engine.Loop = Engine_Loop;
Engine.Load = Engine_Load;
Engine.__index = Engine

return Engine