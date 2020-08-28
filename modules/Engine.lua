package.cpath = "./libs/?.dll;./libs/?53.dll;"..package.cpath

---------------------------------       ENGINE_PART        ----------------------------------
local t = os.clock()
OOP = require"modules.OOP"

local g = require"modules.Graphics"               -- Base modules


local iup = require"iuplua"; require"iupluagl"; -- adds GlCanvas to iup


_en ={
    0.016666666666667;
    {w = 640, h = 480};
    1
}

local Scenes = require"Scenes.SceneEnum"
--additional variables
local keydown =  "" -- second is for emulating keydown behaviour
local CurrentScene;
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
    
    fullscreen = "yes";
    size="640x480"; -- initial size
     -- use the Lua icon from the executable in Windows
    
};
local update = iup.timer{time = 16; run = "no"};


-- FUNCTIONS FOR EXTERNAL WORK
function Pause()
    update.run = "no"
end

function UnPause()
    update.run = "yes"
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
    
    iup.LoopStep()
    
    
    CurrentScene:Update(keydown)

    iup.Update(output)
    
end



function output:keypress_cb( key, press)
    
    local k = key < iup.K_z and utf8.char(key) or ""
    --simulating onkeydown event
    if press == 1 and k ~= "" then
        
        keydown = (keydown:match(k)) and keydown or keydown.." "..k
        
    else

        keydown = (keydown:match(k)) and keydown:gsub(" "..k, "") or keydown
    
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