

---------------------------------       ENGINE_PART        ----------------------------------

OOP = require"modules.OOP"

local g = require"modules.Graphics"               -- Base modules
require"modules.Controls"

require"iuplua"; require"iupluagl"; -- adds GlCanvas to iup

require"config"

local Scenes = require"Scenes.SceneEnum"
--additional variables
local key, down = {},0
local CurrentScene;
-- Utility functions

Graphics = g.Drawing

local function StoXY(s) -- converts String char pos to XYCoords(needed for mouse pos convert)
    local i = s:find("x")
    return tonumber(s:sub(1,i-1)), tonumber(s:sub(i+1))
end 



---------------------------------       IUP_PART        ----------------------------------

iup.SetGlobal("UTF8MODE", "Yes") -- some settings

-- Controls
local output = iup.glcanvas{ -- canvas for OpenGL
    buffer="DOUBLE"; 
    rastersize = "640x480"; -- standard resolution

};

local dialog = iup.dialog
{
    
    output;
    
    fullscreen = "no";
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
    CurrentScene = require(Scenes[_en[3]])
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

    
end



function update:action_cb()
    
    CurrentScene:Update(key, down)

    iup.Update(output)
    
end



function output:keypress_cb( k, d)
    if d == 1 then
        key[k] = k; 
    elseif d == 0 then
        key[k] = nil
    end
    down = d
    if d == 1 then
        Controls.Command(k)
    end
end

function dialog:destroy_cb()
    CurrentScene:Delete()
end



local function Engine()
    dialog:show()
    
    iup.SetFocus(output)
    
    update.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
    end
end
return Engine