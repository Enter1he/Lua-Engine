local Sprite = require"classes.Sprite"
local Mob = require"classes.Mob"
local Human;
Human = {

}

function Human.new(x,y)
    local a = {
        sight  = 200;
        dir = 1;
        score = 0;
        Act = Human.Act;
        
    };
    Sprite.newSheet(a)
    Mob.newMob(a, x, y, 100)
    a.Vel_Move = Human.Vel_Move
    Human.newBrain(a)
    return a;
end
local hlfv = 0.85090352453412
local w, h = 640, 320
function Human:Vel_Move()
    local sdt = self.speed * dt
    local pos, vel = self.pos, self.vel
    if vel[1] ~= 0 and vel[2] ~= 0 then
        vel[1] = hlfv*vel[1]; vel[2] = hlfv*vel[2]
    end
    local ox, oy = pos[1], pos[2]
    pos[1] = pos[1] + vel[1] * sdt
    pos[2] = pos[2] + vel[2] * sdt
    if pos[1] + 36 > w or pos[1] < 0 then
        pos[1] = ox
    end
    if pos[2] + 44 > h or pos[2] < 0 then
        pos[2] = oy
    end
end
local Net = require "Scenes/Sim/classes/Neuronet"
function Human.newBrain(h)
    local brain = Net.new(3, 0, 2)
    Net.randomSynapses(brain)
    h.brain = brain
end
function Human:Act(flowers)
    if not self.brain then
        error"Humans without brain cant act!"
    end
    local fAngle = 0;
    local nFlower = 0;
    local hScore = self.score;

    local pos, vel = self.pos, self.vel
    local brain = self.brain
    local max = 2500;
    local flo = flowers[1]
    
    for i = 1, #flowers do
        local f = flowers[i]
        f.ny = 0
        f.nx = 0
        local dx, dy = pos[1] - f.pos[1], pos[2] - f.pos[2]
        local r2 = dx*dx + dy*dy
        if r2 > max then
            max = r2
            f.nx = dx
            f.ny = dy
            flo = f
        end
        if r2 < self.sight*self.sight then
            nFlower = nFlower + 1
        end
    end
    fAngle = math.atan2(flo.ny, flo.nx)
    brain.inp[1]:rebias(fAngle)
    brain.inp[2]:rebias(nFlower)
    brain.inp[3]:rebias(hScore)
    Net.count(brain)

    vel[1] = brain.out[1].bias
    vel[2] = brain.out[2].bias
    self:Vel_Move()
end

return OOP.class("Human", Human) ;