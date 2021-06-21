
local Zombie;
local Human = require("Scenes.Sim.classes.Human")
Zombie = {}

local oldnew = Human.new
function Zombie.new(x,y)
    local a = oldnew(x,y)
    a.score = 5
    a.speed = 150
    return a
end



return Zombie