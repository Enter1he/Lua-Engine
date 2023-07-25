local Sprite = require"classes.Sprite"
local Mob = require"classes.Mob"
local Net = require "Scenes/Sim/classes/Neuronet"
local Human;
local screen = _en.screen
local Collide = Collision
local CtC = Collide.CtC
Human = {
    died = 0;
}

local brain_tick = 0.5

function Human.new(x,y)
    local a = {
        sight  = 200;
        dir = 1;
        score = 100;
        stamina = 100;
        Act = Human.Act;
        reset = Human.reset;
        s_max = 100;
        tick = 0;
    };
    Sprite.newSheet(a)
    Mob.newMob(a, x, y, a.s_max)
    a.Vel_Move = Human.Vel_Move
    Human.newBrain(a)
    return a;
end

function Human.reset(h, x, y)
    if type(h) ~= "table" then
        print(type(h))
        error"Human must be table!"
    end
    h.pos[1] = x or 0; h.pos[2] = y or 0
    -- Net.randomizeSynapses(h.brain)
    h.speed = h.s_max
    h.score = 100
end

local hlfv = 0.85090352453412
local w, h = 640, 320
function Human:Vel_Move()
    local sdt = self.speed * dt
    local pos, vel = self.pos, self.vel
    if vel[1] ~= 0 and vel[2] ~= 0 then
        vel[1] = hlfv*vel[1]; vel[2] = hlfv*vel[2]
        self.score = self.score - self.speed*0.001
        if self.score >= 50 then
            self.speed = self.s_max
        elseif self.score < 25 then
            self.speed = self.s_max*0.25
        end
    end
    local ox, oy = pos[1], pos[2]
    pos[1] = pos[1] + vel[1] * sdt
    pos[2] = pos[2] + vel[2] * sdt

    if pos[1] + 36 > w then
        pos[1] = 0
    elseif pos[1] < 0 then
        pos[1] = w - 36
    end
    if pos[2] - 24 > h then
        pos[2] = 0
    elseif pos[2] < 0 then
        pos[2] = h - 44
    end
end

function Human.newBrain(h)
    local brain = Net.new(3, 0, 2)
    Net.randomSynapses(brain)
    h.brain = brain
end
function Human:Act(flowers)
    if self.score <= 0 then
        self.Draw = false
        -- print"respawn"
        -- Human.died = Human.died + 1
        -- Human.top[Human.died] = self
        return
    end
    if not self.brain then
        error"Humans without brain can't act!"
    end
    local fAngle = 0;
    local nFlower = 0;
    local hScore = self.score;
    local a = self
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

    for j = 1, #flowers do
        b = flowers[j]
        if b.Draw then
            for j = 1, #flowers do
                if j ~= i then
                    local b = flowers[j]
                    local x, y = Collision.CafC(a.pos[1], a.pos[2], 25, b.pos[1], b.pos[2], 18)
                    if x ~= 0 or y ~= 0 then
                        b.pos[1] = b.pos[1] + x
                        b.pos[2] = b.pos[2] + y
                    end
                end
            end
            -- if CtC(a.pos[1], a.pos[2] , 25, b.pos[1], b.pos[2], 18) then
            --     local n = #flowers
            --     a.score = a.score + 5
            --     b.Draw = false
            --     break
            -- end
        end
        
    end
    
    fAngle = math.atan2(flo.ny, flo.nx)
    brain.inp[1]:rebias(fAngle)
    brain.inp[2]:rebias(nFlower)
    brain.inp[3]:rebias(a.score)

    if self.tick > brain_tick then
        if #brain.list < 20 then
            Net.mutate(brain)
        end
        self.tick = 0
    end
    self.tick = self.tick + dt
    Net.count(brain)

    vel[1] = brain.out[1].bias
    vel[2] = brain.out[2].bias
    self:Vel_Move()
end

return OOP.class("Human", Human) ;