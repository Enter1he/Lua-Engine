--engine
_G.OOP = require"modules.OOP"
_G.new = setmetatable -- used in class creation, so I made it shoter
_G.iup = require"iuplua"; require"iupluagl" -- adds GlCanvas to iup
_G.gl = require"luagl"
_G.glu = require"luaglu"

_G.Controls = require"modules.Controls"
_G._en ={
    dt = 0.016666666666667;
    screen = {w = 1, h = 1};
}
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
    
    Scenes = require"Scenes.SceneEnum";
    
    output = iup.glcanvas{ -- canvas for OpenGL
        buffer="DOUBLE"; 
        rastersize = "640x480"; -- standard resolution
    
    };
    Update = iup.timer{time = 16; run = "no"};

}
Engine.dialog = iup.dialog
    {
        
        Engine.output;
        
        title="ASCII Console"; 
        size="640x480"; -- initial size
        icon=1; -- use the Lua icon from the executable in Windows
        
    };

local keydown =  "" -- second is for emulating keydown behaviour

local CurrentLevel = AddScene(Engine.Scenes[0])

local function Engine_Load(self)
    _en.output = self.output
end
Engine.Load = Engine_Load;


local function output_map(self)
    iup.GLMakeCurrent(self)
    
    CurrentLevel:Load()
    iup.GLSwapBuffers(self)
end
Engine.output.map_cb = output_map


local function output_action(self)
    
    iup.GLMakeCurrent(self)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    local t = os.clock() * 1
    
    CurrentLevel:Draw()
    
    gl.Flush()
    iup.GLSwapBuffers(self)
    
end
Engine.output.action = output_action


local function output_resize_cb(self, width, height)

    iup.GLMakeCurrent(self)

    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, width, 0, height, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()
    _en.screen.w = width; _en.screen.h = height
    

end
Engine.output.resize_cb = output_resize_cb


local function Update_action(self)
    
    iup.LoopStep()
    
    -- local s = iup.GetGlobal('CURSORPOS')
    -- local x, y = Stoxy(s)
    CurrentLevel = CurrentLevel:Update(keydown) or CurrentLevel
    iup.Update(_en.output)
    
end
Engine.Update.action_cb = Update_action


local function output_KeyState(self, key, press)
    
    local k = utf8.char(key)
    --simulating onkeydown event
    if press == 1 then
        
        keydown =(keydown:match(k)) and keydown or keydown.." "..k
        
    else

        keydown = (keydown:match(k)) and keydown:gsub(" "..k, "") or keydown
    
    end
end
Engine.output.keypress_cb = output_KeyState



local function Engine_Loop(self)
    self.dialog:showxy(0,0)
    iup.SetFocus(self.output)
    
    self.Update.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
      end
end
Engine.Loop = Engine_Loop;





return OOP.class(Engine, "Engine")