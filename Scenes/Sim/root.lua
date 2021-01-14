

local Collide = require"Collision"

local K_W, K_w = 87, 119
local K_A, K_a = 65, 97
local K_D, K_d = 68, 100
local K_S, K_s = 83, 115


local dt, screen, iup =_en.dt, _en.screen, iup

local flowers, humans, zombies, npc, player, flosprite, humarite, zombarite;
local text = new({ value = "fr\nam\ne:", pos = {10, 50}, font = Fonts.."consola.ttf", size = 14, color = {1,1,1,1}}, Text)
text.Draw = Graphics.DrawText
local walk = {1,2,3,2,1,5,6,5, frame = 1}


local function checkT(t)
    for k,v in pairs(t) do
        print(k,v)
    end
end


Controls.AddCommand(0xFF1B, os.exit)
Controls.AddCommand(9, 
    function()
        print(npc[5].core == humarite.core)
        print(npc[humans+1].core == zombarite.core)
    end
)
Controls.AddCommand(122, ResumeLoading)

local Sim = NewScene{
    npc = {};
    name = "Sim"
    
}

local std_origin = {0.5, 0.5}
local green = {0,1,0}
local blue = {0,0,1}
local red = {1,0,0}

function Sim:Load()
    
    npc = self.npc
    player = new(
        {
            Stop = Mob.Stop;
            pos = {0,0};
            vel = {0,0}
        }, Mob)
    player.speed = 100
    text:Load()
    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time())
    flowers = 100
    humans = 20
    zombies = 10
    
    print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies) -- print number of mobs
    

    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = {},{}, {}
    Sprite.newSimple(flosprite)
    flosprite:Load("Scenes/Sim/res/Flower/Flower.png")
    flosprite.color = green
    
    
    
    Sprite.newSheet(humarite)
    humarite:Load("Scenes/Sim/res/St_Ava/Avatar1", 'color')
    humarite.color = blue


    Sprite.newSheet(zombarite)
    zombarite:Load("Scenes/Sim/res/Zomb/Zombie", 'color')
    zombarite.color = red
--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    npc.length = 0
    if flowers > 0 then
        for i = 1, flowers do
            
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            local a = {
                __name = "flower"; 
            }
            Sprite.newSimple(a)
            flosprite:CopySprite(a)
            a.pos = {x, y};
            npc[i] = a
        end
        npc.length = npc.length + flowers
    end
    if humans > 0 then
        for i = 1, humans do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            npc[n] = new({
                __name = 'human'
            }, Mob)
            Sprite.newSheet(npc[n])
            Mob.newMob(npc[n], x, y, 100)
            humarite:CopySprite(npc[n])
            npc[n].dir = 0
        end
        npc.length = npc.length + humans
    end
    coroutine.yield()
    
    if zombies > 0 then
        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            npc[n] = {
                __name = 'zombie'
            }
            Sprite.newSheet(npc[n])
            Mob.newMob(npc[n], x, y, 50)
            zombarite:CopySprite(npc[n])
            npc[n].dir = 0
            
        end
        npc.length = npc.length + zombies
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
local dx, dy = {50,screen.w-50}, {0,screen.h-60}
local CtC = Collide.CtC
local function Npc_Update()
    local i,len = 0, npc.length
    local a, b;
    for i = flowers+1, len do
        
        a = npc[i]
        local ax, ay = a.pos[1] + a.origin[1]*a.size[1] , a.pos[2] - a.origin[2]*a.size[2]
        a:RMoveTo( dx, dy)
        Npc_Controls(a)
        if a.core == zombarite.core then
            
            a:PlayAnim(a.dir*7, true, 15, 5)
            for j = flowers+1, len-zombies do 
                b = npc[j]
                if b.core == humarite.core then
                    
                    local bx, by = b.pos[1] , b.pos[2] - b.origin[2]*b.size[2]
                    if CtC(ax, ay, 20, bx, by, 12) then
                        zombarite:CopySprite(b)
                        
                        b.CopySprite = nil
                        b.__name = 'zombie'
                        b.speed = a.speed
                        humans = humans - 1
                        break
                    end    
                end
            end

        elseif a.core == humarite.core then
            
            a:PlayAnim(a.dir*7, true, 10, 5)
            for j = 1, flowers do
                b = npc[j]
                
                if b.core then
                    local bx, by = b.pos[1] , b.pos[2] - b.origin[2]*b.size[2]
                    if CtC(ax, ay, a.sight, bx, by, b.size[1]) then
                        a.rx, a.ry = b.pos[1], b.pos[2]
                    end
                    if CtC(ax, ay , 10, bx, by,25) then
                        a.over = true
                        b.core = nil
                        break
                    end
                end
            end
        end

    end
end

function Sim:Update()

    text.value = string.format('frame_t: %s ms\n fps: %s', tostring(ft), tostring(fps))
    if not paused then
        
        Npc_Update()
    end
    if humans == 0 then
        print"Changing scene..."
        ChangeScene(3)
    end
end

function Sim:KeyPress(key, down)
    player:Stop()
    if key[K_W] or key[K_w] then 
        player.vel[2] = -1
    end
    if key[K_S] or key[K_s] then 
        player.vel[2] = 1
    end
    if key[K_A] or key[K_a] then 
        player.vel[1] = -1
    end
    if key[K_D] or key[K_d] then 
        player.vel[1] = 1
    end
    
    player:Vel_Move(dt)
end

local tointeger = math.ceil
function Sim:Draw(gl)
    
    do -- Layer 1
        for i=1, npc.length do
            local obj = npc[i]
            
            if obj.core  then
               
                obj:Draw()
                
            end
            
        end
        text:Draw()
    end
    
end

return Sim