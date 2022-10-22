local path = ".Scenes.Sim."


local Collide = require"Collision"


local dt, screen =_en.dt, _en.screen

local   flowers, humans, zombies, -- numbers of objects
        npc, objs,  -- individual obj and npc tables
        flosprite, humarite, zombarite, music; -- sprites and music files


local Listener= Audio.Listener

local text = Text.newText(
    { 
        value = "fr\nam\ne:";
        pos = {10, 50};
        -- font = "Teletactile-3zavL.ttf";
        size = 14;
        color = {1,1,1,1};
        alpha = 1;
        
    }
)

local interface = Layer.new{}
interface:AddDrawable(text)

local Sim = NewScene{
    
    name = "Sim"

}

Controls.AddCommand(B.esc, Close)

Controls.AddCommand(B.z, ResumeLoading)


local green = {0,1,0,1}
local blue = {0,0,1,1}
local red = {1,0,0,1}


local main = Layer.new{blend = 0}


local Human = require (path.."classes.Human")
local Zombie = require (path.."classes.Zombie")

local function Npc_Controls(npc) -- adjusting directions for moving mobs
    if npc.vel[2] > 0 then
        npc.dir = 1
    end
    if npc.vel[1] > 0 then
        npc.dir = 2
    end
    if npc.vel[2] < 0 then
        npc.dir = 3
    end
    if npc.vel[1] < 0 then
        npc.dir = 4
    end
    
end


local int = int
local DrawSprite = Graphics.DrawSprite
local DrawSpriteSheet = Graphics.DrawSpriteSheet
local DrawCircle = Graphics.DrawCircle

local function drawobj(self)
    local ax, ay = self.pos[1] + self.size[1], self.pos[2]
    DrawCircle(2, ax, ay, 5)
    -- Graphics.DrawRect(false, ax, ay, self.size[1], self.size[2])
    DrawSprite(self)
    
end
local function drawnpc(self)
    local ax, ay = self.pos[1] + self.size[1]+10, self.pos[2] + self.size[2]*self.origin[2]
    
    DrawSpriteSheet(self)
    DrawCircle(false, ax, ay, 10,35)
end 

function Sim.Load(Sim)
    
    npc = {}
    objs = {}
    player = new(
        {
            speed = 150;
            Stop = Mob.Stop;
            pos = {0,0,0};
            vel = {0,0}
        }, Mob)
    
    text:Load(true)
    
    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time()) --initialize randomness
    flowers = 100
    humans = 20
    zombies = 10

    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = {}, {}, {}
    Sprite.newSimple(flosprite)
    flosprite:Load("Scenes/Sim/res/Plants/Flower.png",true)
    flosprite.color = green
        
    Sprite.newSheet(humarite)
    humarite:Load("Scenes/Sim/res/Human/Human", false)
    humarite.color = blue

    Sprite.newSheet(zombarite)
    zombarite:Load("Scenes/Sim/res/Zomb/Zombie", false)
    zombarite.color = red
    
--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    
    npc.length = 0
    if flowers > 0 then
        for i = 1, flowers do

            local x, y = math.random(50,screen.w-50), math.random(50,screen.h-50)
            local a = {
                __name = "flower";
                
            }
            Sprite.newSimple(a)
            flosprite:CopySprite(a)
            a.pos = {x, y};
            
            objs[i] = a
            
            main:AddDrawable(a)
            
        end

    end
    if humans > 0 then

        for i = 1, humans do
            
            local x, y = math.random(50,screen.w-50), math.random(60,screen.h-60) --math.random(dx[1],dx[2]), math.random(dy[1],dy[2])
            local a = Human.new(x,y)
            humarite:CopySprite(a)
            main:AddDrawable(a)
            npc[i] = a
        end
        npc.length = npc.length + humans
    end

    
    music = Sound.new{
        pos = {0,0,0};
        pitch =1;
        max_distance = 30;
        rolloff = 1;
    }
    
    Audio.LoadSound(music,"Test.ogg")
    Listener.pos = player.pos
    
    coroutine.yield()

    if zombies > 0 then

        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            local a = Zombie.new(x, y)
            zombarite:CopySprite(a)

            npc[n] = a
            main:AddDrawable(a)
        end
        npc.length = npc.length + zombies
    end

    
end

function Sim:Delete()
    Graphics.DestroySpriteSheet(humarite)
    Graphics.DestroySpriteSheet(zombarite)
end

local Npc_Update;
local fmt = string.format
function Sim:Update()
    -- print"Update"
    text.value = fmt('flowers: %s\n fps: %s', flowers,  tostring(fps))
    if not paused then

        Npc_Update()
        
    end
    
end


local CtC = Collide.CtC
local dx, dy = {50,screen.w-50}, {60,screen.h-60}
function Zombie.Act(a)
    local b;

    if a.score <= 0 then

        a:PlayAnim(5, false, 15, 5)
        return;
    end
    local ax, ay = a.pos[1] + a.origin[1]*a.size[1] , a.pos[2] - a.origin[2]*a.size[2]
    for j = 1, npc.length do
        b = npc[j]
        if b.Act == Human.Act then
            local bx, by = b.pos[1] , b.pos[2] - b.origin[2]*b.size[2]
            if CtC(ax, ay, 20, bx, by, 12) then
                zombarite:CopySprite(b)
                Zombie.infect(b)
                a.score = a.score - 1 
                break
            end
        end
    end
    a:PlayAnim(a.dir, true, 15, 5)
    a:RMoveTo( dx, dy)
end

function Human.Act(a)
    local b;
    local ax, ay = a.pos[1] + a.size[1] + 10, a.pos[2] + a.size[2]*a.origin[2]
    if a.over then
        a.found = nil
    end
    for j = 1, flowers do
        b = objs[j]
        if b.visible then
            local bx, by = b.pos[1] + b.size[1], b.pos[2]
            if CtC(ax, ay , 25, bx, by, 18) then
                local n = #objs
                a.over = true
                a.found = nil
                b.visible = nil
                objs[j], objs[n] = objs[n], objs[j]
                objs[n] = nil
                a.score = a.score + 1
                main:RemoveDrawable(b)
                flowers = flowers - 1
                break
            end
            if CtC(ax, ay, a.sight, bx, by, 25) and not a.found then
                a.rx, a.ry = b.pos[1]+25, b.pos[2] - 28 
                a.found = true
            end
            
        end
    end
    a:PlayAnim(a.dir, true, 10, 4)
    a:RMoveTo( dx, dy)
    
end

function Npc_Update()
    local len = npc.length
    local a;
    
    for i = 1, len do
        a = npc[i]
        Npc_Controls(a)
        a:Act()
        
    end
end






function Sim:KeyPress(key, down)
    player:Stop()
    if key[B.w] then
        player.vel[2] = -1
    end
    if key[B.s] then
        player.vel[2] = 1
    end
    if key[B.a] then
        player.vel[1] = -1
    end
    if  key[B.d] then
        player.vel[1] = 1
    end
    if key[B.m] then
        music:Play()
    end
    if player:isMoving() then
        player:Vel_Move()
        main.pos[1] = int(player.pos[1])
        main.pos[2] = int(player.pos[2])
        Listener:Update()
    end
end




function Sim:Draw()
    
    Graphics.color = red
    main:Draw()
    interface:Draw()
    Graphics.DrawRect(false, 300, 300, 400, 400)
end



return Sim