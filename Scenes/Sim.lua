local Controls = require"modules.Controls"
local Layer = require"classes.Layer"
local Mob = require"classes.Mob"
local Sprite = require"classes.Sprite"
local Collide = require"Collision"


local dt, screen =_en[1], _en[2]

local flowers, humans, zombies, npc, player, flosprite, humarite, zombarite;

local walk = {1,2,3,4,3,2,1,5,6,7,6,5}

local function ClearFlowers(t)
    for i = 1, npc.length-zombies do
        if not t[i] then
            table.remove(t, i)
            npc.length = npc.length - 1
        end
    end
end


local function WalkDraw(self, obj)
    self:DrawAnim(obj,  walk)
    if obj.rate >= 5*dt then 
        if obj.frame < #walk then
            obj.frame = obj.frame + 1
        else
            obj.frame = 1
        end
        obj.rate = 0
        return 0;
    end
    obj.rate = obj.rate + dt
    
end

local Sim = {
    npc = {};

    
}

function Sim:Load( engine)
    math.randomseed(os.time())
    npc = self.npc
    
    player = new({pos = {screen.w*0.5,screen.h*0.5}}, Mob)
    player.speed = 100

    -- gen nums of flowers, humans, zombies
    flowers = math.random(1,20)
    humans = math.random(1,20)
    zombies = math.random(1,20)
    print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies)
    
    flosprite, humarite, zombarite = new({Draw = Sprite.DrawSheet}, Sprite), new({Draw = WalkDraw}, Sprite), new({Draw = WalkDraw}, Sprite)
    flosprite:Load"Resources/Flower/Flower"
    humarite:Load"Resources/St_Ava/Avatar1"
    zombarite:Load"Resources/Zomb/Zombie"
    npc.length = flowers+humans+zombies
    -- create mobs
    for i = 1, flowers do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({__name = "flower"; pos = {x,y}}, Mob) 
        npc[i].body = flosprite
    end
    
    for i = flowers+1, flowers+humans do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({__name = "human"; pos = {x,y}; MoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move}, Mob) 
        npc[i].body = humarite
        npc[i].speed = 100
        
    end
    for i = npc.length - zombies - 1, npc.length do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({__name = "zombie"; pos = {x,y}; MoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move}, Mob) 
        npc[i].body = zombarite
        npc[i].speed = 50
        
    end

end

function Sim:Update( key)
    Controls.Key(player,key)
    player:Vel_Move()
    local a, b;
    for i = flowers+1, #npc do
        a = npc[i]
        a:RMoveTo({50,screen.w-50}, {0,screen.h-60})
        if a.__name == 'human' then
            
            for j = 1, flowers do
                b = npc[j]
                if not b then
                    goto skip
                end
                if Collide.CtC(a.pos[1], a.pos[2], 20, b.pos[1], b.pos[2], 12) then
                    npc[j] = nil
                    flowers = flowers - 1
                    
                end
                ::skip::
            end
            
        elseif a.__name == 'zombie' then
            for j = flowers+1, npc.length - zombies do
                b = npc[j]
                
                if Collide.CtC(a.pos[1], a.pos[2], 20, b.pos[1], b.pos[2], 12) then
                    b.body = zombarite
                    b.__name = 'zombie'
                    npc[npc.length+1] = npc[j]
                    humans = humans - 1
                    zombies = zombies + 1 
                    table.remove(npc, j)
                end
                ::skip::
            end
        end
    end
    ClearFlowers(npc)
    
end

function Sim:Draw()
    local x, y = player.pos[1] - screen.w*0.5, player.pos[2] - screen.h*0.5
    local obj;
    do -- Layer 1
        for i=1, #npc do
            obj = npc[i]
            gl.PushMatrix()
                
                gl.Translate(-x+0.5, -y+0.5, 0)
                if obj.__name == 'flower' then
                    gl.Color(0,1,0)
                elseif obj.__name == 'human' then
                    gl.Color(0,0,1)
                elseif obj.__name == 'zombie' then
                    gl.Color(1,0,0)
                end
                obj.body:Draw(obj)
                
            gl.PopMatrix()
        end
    end
end

function Sim:Delete()
    flosprite:Delete() 
    humarite:Delete() 
    zombarite:Delete() 
end


-- Sim.Load = Load
-- Sim.Update = Update
-- Sim.Draw = Draw
-- Sim.Delete = Delete



return Sim