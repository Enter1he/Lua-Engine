package.cpath = "./lib/?.dll;./lib/?53.dll;"..package.cpath
package.loaded.name = "loaded"
---------------------------------       ENGINE_PART        ----------------------------------

OOP = require"modules.OOP"

EmptyFunc = function() end

require"modules.Controls"

-- local g = require"modules.Graphics"
-- Graphics = g.Drawings
_en ={
    dt = 0.016666666666667;
    screen = {w = 640, h = 480, name = "screen"};
    1;
    name = "_en";
}


Graphics = require"Graphics"

Fonts = "C:/Windows/Fonts/" -- Windows font factory


local screen = _en.screen
--additional variables
local key, down = {},0



-- Utility functions


_mouse = {0,0}

local Destructor = {
    __gc = 
    function(self)
        self:Delete()
    end
}
local EmptyFunc = EmptyFunc
local DrawScene = Graphics.DrawScene
local setmetatable = setmetatable
function NewScene(scene)
    scene = scene or {}
    local _ENV = scene
    Load = EmptyFunc;
    Draw = EmptyFunc;
    Update = EmptyFunc;
    Delete = EmptyFunc;
    KeyPress = EmptyFunc;
    KeyRelease = EmptyFunc;
    Motion = EmptyFunc;
    Button = EmptyFunc;
    Render = DrawScene;
    setmetatable(scene, Destructor)
    
    return scene

end


Layer = require"classes.Layer"
Text = require"classes.Text"
Sprite = require"classes.Sprite"
Item = require"classes.Item"
Mob = require"classes.Mob"
Vector = require"classes.Vector"
Color = require"classes.Color"



