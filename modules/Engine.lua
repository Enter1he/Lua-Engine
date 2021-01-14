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

function NewScene(scene)
    
    scene.Load = EmptyFunc;
    scene.Draw = EmptyFunc;
    scene.Update = EmptyFunc;
    scene.Delete = EmptyFunc;
    scene.KeyPress = EmptyFunc;
    scene.KeyRelease = EmptyFunc;
    scene.Motion = EmptyFunc;
    
    return scene

end



Text = require"classes.Text"
Sprite = require"classes.Sprite"
Item = require"classes.Item"
Mob = require"classes.Mob"


