-- package.cpath = "./lib/?.dll;./lib/?53.dll;"..package.cpath
-- package.loaded.name = "loaded"
---------------------------------       ENGINE_PART        ----------------------------------
require"modules/OOP"

EmptyFunc = function() end

require"modules/Controls"

-- if developer then
--     local tl = require"tl"
--     local env = tl.init_env(nil, nil, nil, {[1] = "config"})
--     local function checkT(T)
--         print(tostring(T).." check")
--         for k,v in pairs(T) do
--             print(k,v)
--         end
--         print()
--     end
--     print(SceneEnum[startscene]..".root.lua")
--     local name = SceneEnum[startscene].."/root.lua"
--     local result = tl.process_string([[
--         global Layer = require"classes/Layer"
--         global Text = require"classes/Text"
--         global Sprite = require"classes/Sprite"
--         global Mob = require"classes/Mob"
--         global Vector = require"classes/Vector"
--         global Color = require"classes/Color"
--     ]], env)
--     checkT(result)
--     local dependencies = result.dependencies
--     checkT(dependencies)
--     if #dependencies > 0 then
--         print(name .. ":" .. dependencies[1].y .. ":" .. dependencies[1].x .. ": " .. dependencies[1].msg)
--     end
--     local warnings = result.warnings
--     if #warnings > 0 then
--         print(name .. ":" .. warnings[1].y .. ":" .. warnings[1].x .. ": " .. warnings[1].msg)
--     end
--     local syntax_errors = result.syntax_errors
--     if #syntax_errors > 0 then
--         print(name .. ":" .. syntax_errors[1].y .. ":" .. syntax_errors[1].x .. ": " .. syntax_errors[1].msg)
--     end
--     local type_errors = result.type_errors
--     if #type_errors > 0 then
--         print(name .. ":" .. type_errors[1].y .. ":" .. type_errors[1].x .. ": " .. type_errors[1].msg)
--     end
-- end

require"Audio"
require"Graphics"
Collision = require"Collision"

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
    scene.Resize = EmptyFunc;

    scene.name = scene.name or format("%x",rand(1,30000000))

    setmetatable(scene, Destructor)
    return scene
end

Layer = require"classes/Layer"
Text = require"classes/Text"
Sprite = require"classes/Sprite"
Mob = require"classes/Mob"
Vector = require"classes/Vector"
Color = require"classes/Color"

_default = NewScene()

