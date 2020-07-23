---------------------------------       ENGINE_PART        ----------------------------------
local t = os.clock()
OOP = require"modules.OOP"
local g = require"modules.Graphics"               -- Base modules


local iup = require"iuplua"; require"iupluagl" -- adds GlCanvas to iup



local Scenes = require"Scenes.SceneEnum"
--additional variables
local keydown =  "" -- second is for emulating keydown behaviour
local CurrentLevel;
-- Utility functions


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
    
    title="LuaEngine-player"; 
    size="640x480"; -- initial size
    icon=1; -- use the Lua icon from the executable in Windows
    
};
local update = iup.timer{time = 16; run = "no"};



-- IUP CALLLBACKS


function output:map_cb()
    
    iup.GLMakeCurrent(self)

    CurrentLevel = require(Scenes[0])
    CurrentLevel:Load()
    
    
    iup.GLSwapBuffers(self)

end



function output:action(x,y)
    
    iup.GLMakeCurrent(self)
    g.DrawMain(CurrentLevel)
    iup.GLSwapBuffers(self)
    
end



function output:resize_cb( width, height)

    iup.GLMakeCurrent(output)
    g.InitMain(width, height)

    
end



function update:action_cb()
    
    iup.LoopStep()
    
    -- local s = iup.GetGlobal('CURSORPOS')
    -- local x, y = Stoxy(s)
    CurrentLevel = CurrentLevel:Update(keydown) or CurrentLevel
    iup.Update(output)
    
end



function output:keypress_cb( key, press)
    
    local k = utf8.char(key)
    --simulating onkeydown event
    if press == 1 then
        
        keydown = (keydown:match(k)) and keydown or keydown.." "..k
        
    else

        keydown = (keydown:match(k)) and keydown:gsub(" "..k, "") or keydown
    
    end
end

function dialog:destroy_cb()
    CurrentLevel:Delete()
end

print("Entering time",os.clock()-t)

return function()
    dialog:showxy(0,0)
    iup.SetFocus(output)
    
    update.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
    end
end