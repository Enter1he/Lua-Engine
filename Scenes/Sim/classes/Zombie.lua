
local Zombie;
local Human = require("Scenes.Sim.classes.Human")
Zombie = {
    speed = 70;
}

local oldnew = Human.new
function Zombie.new(x,y)
    local a = oldnew(x,y)
    a.score = 5
    a.speed = Zombie.speed
    a.Act = Zombie.Act
    return a
end

function Zombie.infect(a)
    a.score = 5
    a.speed = Zombie.speed
    a.Act = Zombie.Act
end


return Zombie