
local Item = require"classes.Item"
local Collide = require"Collision"
local Text = require"classes.Text"
local Mob = require"classes.Mob"



local dt, screen, iup =_en[1], _en[2], _en[4]

local flowers, humans, zombies, npc, player, flosprite, humarite, zombarite;
local text = new({Draw = Graphics.DrawTextListed, value = "fr\nam\ne:", pos = {10, 50}, font = Fonts.."consola.ttf", size = 36, color = {1,1,1,1}}, Text)
local timer = 1;
local walk = {1,2,3,2,1,5,6,5, frame = 1}

Controls.AddButton(iup.K_A, 
    function()
        player.vel[1] = -1
        player.angle = 3
    end
)
Controls.AddButton(iup.K_D, 
    function()
        player.vel[1] = 1
        player.angle = 1
    end
)
Controls.AddButton(iup.K_W, 
    function()
        player.vel[2] = 1
        player.angle = 4
    end
)
Controls.AddButton(iup.K_S, 
    function()
        player.vel[2] = -1
        player.angle = 2
    end
)

Controls.AddCommand(iup.K_E,
    function()
        humarite.frame = humarite.frame + 1
    end
)

Controls.AddCommand(iup.K_Q,
    function()
        humarite.frame = humarite.frame - 1
    end
)

Controls.AddCommand(iup.K_ESC, 
    function()
        iup.Close()
    end
)
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




local Sim = {
    npc = {};

    
}

function Sim:Load()
    
    npc = self.npc
    player = new({Stop = Mob.Stop,pos = {screen.w*0.5,screen.h*0.5}}, Mob)
    player.speed = 100
    text:LoadList()
    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time())
    flowers = math.random(1,20)
    humans =10
    zombies = math.random(0,3)
    
    print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies) -- print number of mobs
    

    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = new({  Load = Graphics.LoadSprite, color = {0,1,0}}, Sprite),
                                     new({ Load = Graphics.LoadSpriteSheet, color = {0,0,1}, mrate = 4*dt; }, Sprite), 
                                     new({ Load = Graphics.LoadSpriteSheet, color = {1,0,0}, mrate = 8*dt; }, Sprite)
    flosprite:Load"Scenes/Sim/Resources/Flower/Flower.png"
    humarite:Load"Scenes/Sim/Resources/St_Ava/Avatar1"
    zombarite:Load"Scenes/Sim/Resources/Zomb/Zombie"

    
--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    npc.length = 0
    for i = 1, flowers do
        
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[i] = new({Draw = Graphics.DrawSprite, CopySprite = Graphics.CopySprite, __name = "flower"; pos = {x,y}}, Item) 
        npc[i]:CopySprite(flosprite)
        
    end
    npc.length = npc.length + flowers

    for i = 1, humans do
        local n = npc.length + i
        local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
        npc[n] = new({ Draw = Graphics.DrawSpriteSheetAnim,  CopySprite = Graphics.CopySprite, __name = "human"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move,  sight = 100}, Mob) -- c is a collect table fo AI
        npc[n].speed = 100
        npc[n]:CopySprite(humarite)
    end
    npc.length = npc.length + humans

    if zombies > 0 then
        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            npc[n] = new({ Draw = Graphics.DrawSpriteSheetAnim,  CopySprite = Graphics.CopySprite, __name = "zombie"; pos = {x,y}; RMoveTo = Mob.RMoveTo; Vel_Move = Mob.Vel_Move}, Mob) 
            npc[n].speed = 50
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

local function Npc_Update()
    WalkDraw(humarite)
    local a, b;
    local i = npc.length
    while i > flowers do
        a = npc[i]
        if a then
            local o = a
            a:RMoveTo({50,screen.w-50}, {0,screen.h-60})
            
            if a.vel[2] > 0 then
                a.anim = 24
            elseif a.vel[2] < 0 then
                a.anim = 8
            end
            if a.vel[1] > 0 then
                a.anim = 0
            elseif a.vel[1] < 0 then
                a.anim = 16
            end
            a.frame = walk[walk.frame]
            local ax, ay = a.pos[1] - a.w*a.origin[1], a.pos[2] + a.h*a.origin[2]
            if a.__name == 'human' then
            
                local j = flowers
                while j > 0 do
                    b = npc[j]
                    if b.tex then 
                        local bx, by = b.pos[1] - b.w*b.origin[1], b.pos[2] + b.h*b.origin[2]
                        if Collide.CtC(ax, ay, a.sight, bx, by, b.w) then
                            a.rx, a.ry = b.pos[1], b.pos[2]
                        end
                        if Collide.CtC(ax, ay, 20, bx, by, 20) then
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
                    if b then
                        local ax, ay = a.pos[1] - a.w*a.origin[1], a.pos[2] + a.h*a.origin[2]
                        local bx, by = b.pos[1] - b.w*b.origin[1], b.pos[2] + b.h*b.origin[2]
                        if Collide.CtC(ax, ay, 20, bx, by, 12) then
                            b.tex = zombarite.tex
                            b.list = zombarite.list
                            b.color = zombarite.color
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

function Sim:Update( key, down)
    player:Stop()
    Controls.Key( key, down)
    player:Vel_Move()
    
    if not paused then
        Npc_Update()
    end
end



function Sim:Draw(gl)
    local x, y = player.pos[1] - screen.w*0.5, player.pos[2] - screen.h*0.5
    local obj;
    do -- Layer 1
        for i=1, npc.length do
            obj = npc[i]
            
            if obj then
                if obj.tex then
                    gl.PushMatrix()
                        gl.Translate(-x+0.5, -y+0.5, 0)
                        obj:Draw()
                        text:Draw()
                    gl.PopMatrix()
                end
            end
        end
    end
    
end

function Sim:Delete()
    timer = nil
     
end






return Sim