local Controls = require"modules.Controls"
local Layer = require"classes.Layer"
local Mob = require"classes.Mob"
local Sprite = require"classes.Sprite"
local Collide = require"Collision"


local dt, screen, iup =_en[1], _en[2], _en[3]

local flowers, humans, zombies, npc, player, flosprite, humarite, zombarite;
local timer = 0;
local walk = {1,2,3,4,3,2,1,5,6,7,6,5}
Mob.body = nil




local function Npc_Update()
    local a, b;
    local i = npc.length
    while i > flowers do
        a = npc[i]
        if a then
            
            if a.__name == 'human' then
                a:RMoveTo({50,screen.w-50}, {0,screen.h-60})
                local j = flowers
                while j > 0 do
                    b = npc[j]
                    if b.body then 
                        
                        local ax, ay = a.pos[1] - a.body.w*a.body.origin[1], a.pos[2] + a.body.h*a.body.origin[2]
                        local bx, by = b.pos[1] - b.body.w*b.body.origin[1], b.pos[2] + b.body.h*b.body.origin[2]
                        if Collide.CtC(a.pos[1], a.pos[2], a.sight, b.pos[1], b.pos[2], b.w) then
                            a.rx, a.ry = b.pos[1], b.pos[2]
                        end
                        if Collide.CtC(ax, ay-10, 20, bx, by, 20) then
                            a.over = true
                            j = j - 1
                            b.body = nil
                        end
                        
                    end
                    j = j - 1
                end
                

            elseif a.__name == 'zombie' then
                a:RMoveTo({50,screen.w-50}, {0,screen.h-60})
                for j = flowers+1, npc.length - zombies do
                    b = npc[j]
                    if b then
                        local ax, ay = a.pos[1] - a.body.w*a.body.origin[1], a.pos[2] + a.body.h*a.body.origin[2]
                        local bx, by = b.pos[1] - b.body.w*b.body.origin[1], b.pos[2] + b.body.h*b.body.origin[2]
                        if Collide.CtC(ax, ay, 20, bx, by, 12) then
                            b.body = zombarite
                            b.__name = 'zombie'
                        end    
                    end
                    
                end
            end
        end

        i = i - 1
        
    end
    -- timer = timer + dt
    -- if timer >= 1 then
    --     os.execute('cls')
    --     for i = flowers+1, npc.length do
    --         local o = npc[i]
    --         if o then
    --             print(i-flowers,o, o.rx, o.ry, o.over)
    --         else
    --             print(i-flowers,o)
    --         end
    --     end
    --     timer = 0
    -- end
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
    
    npc = self.npc
    player = new({pos = {screen.w*0.5,screen.h*0.5}}, Mob)
    player.speed = 100

    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time())
    flowers = math.random(1,20)
    humans = 10
    zombies = math.random(1,3)
    print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies) -- print number of mobs
    

    ----------------------------------------------ADDING TETURES----------------------------------------------------------------
    flosprite, humarite, zombarite = new({Draw = Sprite.DrawSheet, color = {0,1,0}}, Sprite),
                                     new({Draw = WalkDraw, color = {0,0,1}}, Sprite), 
                                     new({Draw = WalkDraw, color = {1,0,0}}, Sprite)
    flosprite:Load"Resources/Flower/Flower"
    humarite:Load"Resources/St_Ava/Avatar1"
    zombarite:Load"Resources/Zomb/Zombie"


--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    npc.length = 0
    for i = 1, flowers do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({__name = "flower"; pos = {x,y}}, Mob) 
        npc[i].body = flosprite
        
    end
    npc.length = npc.length + flowers

    for i = 1, humans do
        local n = npc.length + i
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[n] = new({__name = "human"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move,  sight = 100}, Mob) -- c is a collect table fo AI
        npc[n].body = humarite
        npc[n].speed = 100
        
    end
    npc.length = npc.length + humans

    if zombies > 0 then
        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            npc[n] = new({__name = "zombie"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move}, Mob) 
            npc[n].body = zombarite
            npc[n].speed = 50
            
        end
        npc.length = npc.length + zombies
    end
    
end

function Sim:Update( key)
    Controls.Key(player,key)
    player:Vel_Move()
    
    Npc_Update()
end

function Sim:Draw(gl)
    local x, y = player.pos[1] - screen.w*0.5, player.pos[2] - screen.h*0.5
    local obj;
    do -- Layer 1
        for i=1, npc.length do
            obj = npc[i]
            if obj.body then
                gl.PushMatrix()
                    
                    gl.Translate(-x+0.5, -y+0.5, 0)
                    
                    obj.body:Draw(obj)
                    
                gl.PopMatrix()
            end
        end
    end
    
end

function Sim:Delete()
    flosprite:Delete() 
    humarite:Delete() 
    zombarite:Delete() 
end






return Sim