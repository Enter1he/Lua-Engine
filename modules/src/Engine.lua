package.cpath = "./lib/?.dll;./lib/?53.dll;"..package.cpath
package.loaded.name = "loaded"
---------------------------------       ENGINE_PART        ----------------------------------

OOP = require"modules.OOP"
Util = require"modules.Util"

EmptyFunc = function() end

require"modules.Controls"


_en ={
    dt = 0.015;
    screen = {w = 640, h = 480, name = "screen"};
    1;
    name = "_en";
}

Audio = require"Audio"
Graphics = require"Graphics"

Fonts = Fonts or "C:/Windows/Fonts/" -- Windows font's folder


local screen = _en.screen



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
local format = string.format
local rand = math.random
local seed = math.randomseed
local clock = os.clock

function NewScene(scene)
    scene = scene or {}
    
    scene.Load = EmptyFunc;
    scene.Draw = EmptyFunc;
    scene.Update = EmptyFunc;
    scene.Delete = EmptyFunc;
    scene.KeyPress = EmptyFunc;
    scene.KeyRelease = EmptyFunc;
    scene.Motion = EmptyFunc;
    scene.Button = EmptyFunc;

    scene.name = scene.name or format("%x",rand(1,30000000))

    setmetatable(scene, Destructor)
    
    return scene
end

Layer = require"classes.Layer"
Text = require"classes.Text"
Sprite = require"classes.Sprite"
Mob = require"classes.Mob"
Vector = require"classes.Vector"
Color = require"classes.Color"

_default = NewScene()

