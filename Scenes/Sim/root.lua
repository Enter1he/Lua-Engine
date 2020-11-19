
local Item = require"classes.Item"
local Collide = require"Collision"
local Text = require"classes.Text"
local Mob = require"classes.Mob"



local dt, screen, iup =_en.dt, _en.screen, iup

local flowers, humans, zombies, npc, player, flosprite, humarite, zombarite;
local text = new({Draw = Graphics.DrawTextListed, value = "fr\nam\ne:", pos = {10, 50}, font = Fonts.."consola.ttf", size = 36, color = {1,1,1,1}}, Text)

local walk = {1,2,3,2,1,5,6,5, frame = 1}




Controls.AddCommand(iup.K_ESC, _Close)

local paused = false
Controls.AddCommand(iup.K_CR, 
    function()
        if paused then
            paused = false
        else
            paused = true
        end
    end
)




local Sim = NewScene{
    npc = {};

    
}
local std_origin = {-0.5, -0.5}
function Sim:Load()
    
    npc = self.npc
    player = new(
        {
            Stop = Mob.Stop;
            pos = {0,0};
            vel = {0,0}
        }, Mob)
    player.speed = 100
    text:LoadList()
    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time())
    flowers = math.random(1,20)
    humans =10
    zombies = math.random(0,3)
    
    print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies) -- print number of mobs
    

    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = new({  Load = Graphics.LoadSprite, color = {0,1,0}, origin = std_origin }, Sprite),
                                     new({ Load = Graphics.LoadSpriteSheet, color = {0,0,1}, mrate = 4*dt, origin = std_origin }, Sprite), 
                                     new({ Load = Graphics.LoadSpriteSheet, color = {1,0,0}, mrate = 8*dt, origin = std_origin }, Sprite)
    flosprite:Load("Scenes/Sim/res/Flower/Flower.png", 'color')
    humarite:Load("Scenes/Sim/res/St_Ava/Avatar1", 'color')
    zombarite:Load("Scenes/Sim/res/Zomb/Zombie", 'color')
    
--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    npc.length = 0
    for i = 1, flowers do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({
            Draw = Graphics.DrawSprite;
            CopySprite = Graphics.CopySprite; 
            __name = "flower"; 
            pos = {x,y};
            angle = 0
        }, 
        Item) 
        npc[i]:CopySprite(flosprite)
        
    end
    npc.length = npc.length + flowers

    for i = 1, humans do
        local n = npc.length + i
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[n] = new({ Draw = Graphics.DrawSpriteSheetAnim,  CopySprite = Graphics.CopySprite, __name = "human"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move,  sight = 100}, Mob) -- c is a collect table fo AI
        npc[n].speed = 100
        npc[n].dir = 0
        npc[n].angle = 0
        npc[n]:CopySprite(humarite)
    end
    npc.length = npc.length + humans

    if zombies > 0 then
        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            npc[n] = new({ Draw = Graphics.DrawSpriteSheetAnim, CopySprite = Graphics.CopySprite,  __name = "zombie"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move}, Mob) 
            npc[n].speed = 50
            npc[n].angle = 0
            npc[n].dir = 0
            npc[n]:CopySprite(zombarite)
        end
        npc.length = npc.length + zombies
    end
    
end

local function WalkDraw(o)

    if o.rate >= o.mrate then 
        if walk.frame < 7 then
            walk.frame = walk.frame + 1
        else
            walk.frame = 1
        end
        o.rate = 0
    else
        o.rate = o.rate + dt
    end

end

local function Npc_Controls(npc)
    if npc.vel[2] > 0 then
        npc.dir = 1
    end
    if npc.vel[1] > 0 then
        npc.dir = 2
    end
    if npc.vel[1] < 0 then
        npc.dir = 0
    end
    if npc.vel[2] < 0 then
        npc.dir = 3
    end
end

local function Npc_Update()
    WalkDraw(humarite)
    local a, b;
    local i = npc.length
    while i > flowers do
        a = npc[i]
        if a then
            
            local o = a
            a:RMoveTo({50,screen.w-50}, {0,screen.h-60})
            
            Npc_Controls(a)
            
            a.frame =a.dir*7 + walk[walk.frame]
            local ax, ay = a.pos[1] - a.origin[1]*a.w , a.pos[2] + a.origin[2]*a.h
            if a.__name == 'human' then
            
                local j = flowers
                while j > 0 do
                    b = npc[j]
                    if b.tex and b.__name == 'flower' then 
                        local bx, by = b.pos[1] - b.origin[1]*b.w, b.pos[2] + b.origin[2]*b.h
                        if Collide.CtC(ax, ay, a.sight, bx, by, b.w) then
                            a.rx, a.ry = b.pos[1], b.pos[2]
                        end
                        if Collide.CtC(ax, ay, 10, bx, by, b.w) then
                            a.over = true
                            j = j - 1
                            b.tex = nil
                            
                        end
                        
                    end
                    j = j - 1
                end
                

            elseif a.__name == 'zombie' then
                for j = flowers+1, npc.length - zombies do
                    b = npc[j]
                    if b.__name == 'human' then
                        local ax, ay = a.pos[1] - a.w*a.origin[1], a.pos[2] + a.h*a.origin[2]
                        local bx, by = b.pos[1] - b.w*b.origin[1], b.pos[2] + b.h*b.origin[2]
                        if Collide.CtC(ax, ay, 20, bx, by, 12) then
                            b:CopySprite(zombarite)
                            b.CopySprite = nil
                            b.__name = 'zombie'
                            b.mrate = a.mrate
                            b.speed = a.speed
                        end    
                    end
                    
                end
            end
            
            
        end

        i = i - 1
        
    end
end

function Sim:Update()

    if not paused then
        
        Npc_Update()
    end
    
end

function Sim:KeyPress(key, down)
    player:Stop()
    if key[iup.K_W] or key[iup.K_w] then 
        player.vel[2] = -1
    end
    if key[iup.K_S] or key[iup.K_s] then 
        player.vel[2] = 1
    end
    if key[iup.K_A] or key[iup.K_a] then 
        player.vel[1] = -1
    end
    if key[iup.K_D] or key[iup.K_d] then 
        player.vel[1] = 1
    end
    player:Vel_Move()
end
local tointeger = math.ceil
function Sim:Draw(gl)
    local x, y = player.pos[1], player.pos[2]
    local obj;
    local mx, my = _mouse[1] + x//1, _mouse[2] - y//1
    text.value = (mx.." "..my)
    do -- Layer 1
        for i=1, npc.length do
            obj = npc[i]
            if obj.tex then
                gl.PushMatrix()
                    gl.Translate(-x, -y, 0)
                    obj:Draw()
                    text:Draw()
                gl.PopMatrix()
            end
            
        end
    end
    
end

function Sim:Delete()
     
end






return Sim