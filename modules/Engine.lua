package.cpath = "./libs/?.dll;./libs/?53.dll;"..package.cpath

---------------------------------       ENGINE_PART        ----------------------------------

OOP = require"modules.OOP"

EmptyFunc = function() end

local g = require"modules.Graphics"               -- Base modules
require"modules.Controls"

require"iuplua"; 
require"iupluagl"; -- adds GlCanvas to iup


local iup = iup

require"config"

local Scenes = require"Scenes.SceneEnum"
--additional variables
local key, down = {},0
local CurrentScene;
-- Utility functions

Graphics = g.Drawing

function StoXY(s) -- converts String char pos to XYCoords(needed for mouse pos convert)
    local i = s:find("x")
    return tonumber(s:sub(1,i-1)), tonumber(s:sub(i+1))
end 

_Close = iup.Close
_mouse = {0,0}



function NewScene(scene)
    
    scene.Load = EmptyFunc;
    scene.Draw = EmptyFunc;
    scene.Update = EmptyFunc;
    scene.Delete = EmptyFunc;
    scene.KeyPress = EmptyFunc;
    scene.Motion = EmptyFunc;
    
    return scene

end



Text = require"classes.Text"
Sprite = require"classes.Sprite"
Item = require"classes.Item"
Mob = require"classes.Mob"

---------------------------------       IUP_PART        ----------------------------------

iup.SetGlobal("INPUTCALLBACKS", "Yes")
iup.SetGlobal("UTF8MODE", "Yes") -- some settings

-- Controls
local output = iup.glcanvas{ -- canvas for OpenGL
    buffer="DOUBLE"; 
    rastersize = "640x480"; -- standard resolution

};

local dialog = iup.dialog
{
    
    output;
    
    fullscreen = "yes";
    size="640x480"; -- initial size
     -- use the Lua icon from the executable in Windows
    
};
local update = iup.timer{time = 16; run = "no"};

local pausef = 0
-- FUNCTIONS FOR EXTERNAL WORK
function Pause()
    if update.run == 'no' then
        update.run = 'yes'
    elseif update.run == 'yes' then
        update.run = 'no'
    end
end



-- IUP CALLLBACKS


function output:map_cb()
    
    iup.GLMakeCurrent(self)
    g.MapMain()
    CurrentScene = require(Scenes[Scene.TDS])
    CurrentScene:Load()
    iup.GLSwapBuffers(self)
    
end



function output:action(x,y)
    
    iup.GLMakeCurrent(self)
    g.DrawMain(CurrentScene)
    iup.GLSwapBuffers(self)
    
end



function output:resize_cb( width, height)

    iup.GLMakeCurrent(output)
    g.InitMain(width, height)
    _en.ah = _en[2].h/height
    _en.aw = _en[2].w/width
    
end



function update:action_cb()
    iup.LoopStep()
    CurrentScene:KeyPress(key, down)
    CurrentScene:Update()
    
    iup.Update(output)
    
end



local function keypress_cb( k, d)
    if d == 1 then
        key[k] = key[k] or undef; 
    elseif d == 0 then
        key[k] = nil
    end
    
    down = d
    if d == 1 then
        Controls.Command(k)
    end
end

local function motion_cb(x,y, status)
    CurrentScene:Motion(x,y)
end

function dialog:destroy_cb()
    CurrentScene:Delete()
end



local function Engine()
    dialog:show()
    
    
    iup.SetGlobalCallback("GLOBALKEYPRESS_CB", keypress_cb)
    iup.SetGlobalCallback("GLOBALMOTION_CB", motion_cb)
    update.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
    end
end
return Engine