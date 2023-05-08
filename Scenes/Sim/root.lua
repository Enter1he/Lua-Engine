print"Sim.root Start"
local Collide = Collision


-- for i = 1, 100000000 do
    
-- end
local  screen = _en.screen

local flowers = 10; local flocount = flowers;
local humans = 10
local zombies = 10 -- numbers of objects
local npc = {top = {}} 
local objs = {}  -- individual obj and npc tables
local flosprite = {}
local humarite = {} 
local zombarite = {}
local music = Sound.new{
        pos = {0,0,0};
        pitch =3;
        max_distance = 30;
        rolloff = 0;
    }; -- sprites and music files


local Listener = Audio.Listener

local text = Text.newText(
    { 
        value = "frame:";
        pos = {10, 50};
        -- font = "Teletactile-3zavL.ttf";
        size = 14;
        color = {1,1,1,1};
        alpha = 1;
        
    }
)

local interface = Layer.new{}


local Sim = NewScene{
    
    name = "Sim"

}

Controls.AddCommand(B.esc, LE.Close)

Controls.AddCommand(B.space, function()
    paused = not paused and true or false
end)

Controls.AddCommand(B.n, function()
    print(#npc)
    for i = 1, #npc do
        print(i..":"..npc[i].pos[1].." "..npc[i].pos[2])
        -- if npc[i].brain then
        --     npc[i].brain:print()
        -- end
    end
    
end)



local green = {1,1,0,1}
local blue = {0,0,1,1}
local red = {1,0,0,1}


local main = Layer.new{blend = 0}


local Human = require ("Scenes/Sim/classes/Human")
local Zombie = require ("Scenes/Sim/classes/Zombie")

Controls.AddCommand(B.z, 
function()
    if #npc > humans then
        if zombies > 0 then

            for i = humans+1, #npc do
                local a = npc[i]
                local n = npc.length + i
                local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
                a.pos, a.score = {x, y}, 5
                zombarite:CopySprite(a)
                a.Act = Zombie.Act
                a.frame = 1
                a.anim = 1
                
            end

        end
        return;
    end
    LE.ResumeLoading()
end)
local Net = require"Scenes.Sim.classes.Neuronet"
Controls.AddCommand(B[1], function ()
    print(npc[1].vel[1],npc[1].vel[2])
    Net.Print(npc[1].brain)
end)

Controls.AddCommand(B.r, 
    function()
        if flowers > 0  then
            for i = 1, flowers do
                local x, y = math.random(50,screen.w-50), math.random(50,screen.h-50)
                local a = objs[i]
                flosprite:CopySprite(a)
                a.pos = {x, y};
                
            end

        end
        if humans > 0 then

            for i = 1, humans do
                local a = npc[i]
                local x, y = math.random(50,screen.w-50), math.random(60,screen.h-60) --math.random(dx[1],dx[2]), math.random(dy[1],dy[2])
                a.Act = Human.Act
                a.pos, a.score = {x, y}, 0
                humarite:CopySprite(a)
                
            end
        end
        if zombies > 0 then 
            for i = humans+1, #npc do
                local a = npc[i]
                a.Draw = nil
            end
        end
    end
)


local function Npc_Controls(npc) -- adjusting directions for moving mobs
    if npc.vel[2] > 0 then
        npc.dir = 1 -- down
    end
    if npc.vel[1] > 0 then
        npc.dir = 2 -- right
    end
    if npc.vel[2] < 0 then
        npc.dir = 3 -- up
    end
    if npc.vel[1] < 0 then
        npc.dir = 4 -- left
    end
    if npc.vel[1] ~= 0 or npc.vel[2] ~= 0 then
        npc:PlayAnim(npc.dir, true, 20, 4)
    end
end


local int = LE.int
local DrawSprite = Graphics.DrawSprite
local DrawSpriteSheet = Graphics.DrawSpriteSheet
local DrawCircle = Graphics.DrawCircle
local osize = {}
local function drawobj(a)
    local ax, ay = a.pos[1] + flosprite.scale[1], a.pos[2]
    DrawCircle(false, ax, ay, 18)
    DrawSprite(a)
end

local function drawnpc(a)
    local ax, ay = 0, 0
    if a.dir == 1 then -- down
        ax, ay = a.pos[1] + humarite.w , a.pos[2] + 0.7*humarite.h
    elseif a.dir == 2 then -- right
        ax, ay = a.pos[1] + humarite.w*1.5 , a.pos[2] + 0.5*humarite.h
    elseif a.dir == 3 then -- up
        ax, ay = a.pos[1] + humarite.w , a.pos[2] + 0.2*humarite.h
    elseif a.dir == 4 then -- left
        ax, ay = a.pos[1] + 0.5*humarite.w , a.pos[2] + 0.5*humarite.h
    end
    DrawSpriteSheet(a)
    DrawCircle(false, ax, ay, 12)
end 

local function drawenemy(a)
    local ax, ay = 0, 0
    if a.dir == 1 then -- down
        ax, ay = a.pos[1] + humarite.w , a.pos[2]
    elseif a.dir == 2 then -- right
        ax, ay = a.pos[1] + humarite.w*1.5 , a.pos[2] - 0.3*humarite.h
    elseif a.dir == 3 then -- up
        ax, ay = a.pos[1] + humarite.w , a.pos[2] - 0.3*humarite.h
    elseif a.dir == 4 then -- left
        ax, ay = a.pos[1] + 0.5*humarite.w , a.pos[2] - 0.3*humarite.h
    end
    DrawSpriteSheet(a)
    DrawCircle(false, ax, ay, 12)
end

function Sim.Load(Sim)
    
    print"Load Start"
    player = new(
        {
            speed = 150;
            Stop = Mob.Stop;
            pos = {0,0,0};
            vel = {0,0}
        }, Mob)
    main.pos = player.pos
    -- interface.pos = player.pos
    text:Load(true)
    interface:AddDrawable(text)
    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time()) --initialize randomness
    
        
    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = {}, {}, {}
    Sprite.newSimple(flosprite)
    flosprite:Load("Scenes/Sim/res/Plants/Flower.png",true)
    flosprite.color = green
    flosprite.scale = Graphics.GetSize(flosprite)
    -- flosprite.Draw = drawobj
    

    Sprite.newSheet(humarite)
    humarite:Load("Scenes/Sim/res/Human/Human", false)
    humarite.color = blue
    humarite.w, humarite.h = humarite.src.w, humarite.src.h
    humarite.Draw = drawnpc


    Sprite.newSheet(zombarite)
    zombarite:Load("Scenes/Sim/res/Zomb/Zombie", false)
    zombarite.color = red
    zombarite.w, zombarite.h = zombarite.src.w, zombarite.src.h
    -- zombarite.Draw = drawenemy
    
--------------------------------------------------CREATING MOBS-------------------------------------------------------------------
    
    npc.length = 0
    if flowers > 0 then
        for i = 1, flowers do

            local x, y = math.random(50,screen.w-50), math.random(50,screen.h-50)
            local a = {
                __name = "flower";
            }
            Sprite.newSimple(a)
            flosprite:CopySprite(a) -- need to copy Sprite to object for that to be used
            a.pos = {x, y};
            objs[i] = a
            
            main:AddDrawable(a)
            
        end
        print"flowers"
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

    
    Audio.LoadSound(music,"Scenes/Sim/res/Test.ogg")
    Listener.pos = player.pos
    print"Load End"
    
    coroutine.yield()
    for i = 1, 1000000 do
        print(i)
    end 
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

local Npc_Update, Obj_Update;
local fmt = string.format
function Sim:Update()
    -- print"Update"
    player:Stop()
    if not paused then

        Npc_Update()
        Obj_Update()
    end
    text.value = fmt("1 haha\n fps: %f dt: %f rad: %2.4f\nspeed: %2.4f\nv1: %2.4f\nv2: %2.4f\nscore: %2.4f",
                    fps, dt, 1, npc[1].speed, npc[1].vel[1], npc[1].vel[2], 1)
end


local CtC = Collide.CtC
local dx, dy = {50,screen.w-50}, {60,screen.h-60}
function Zombie.Act(a)
    local b;

    if a.score <= 0 then

        a:PlayAnim(5, false, 15, 5)
        return;
    end
    local ax, ay;
    if a.dir == 1 then -- down
        ax, ay = a.pos[1] + a.origin[1]*a.size[1] , a.pos[2] + a.origin[2]*a.size[2]
    elseif a.dir == 2 then -- right
        ax, ay = a.pos[1] + a.size[1] , a.pos[2]
    elseif a.dir == 3 then -- up
        ax, ay = a.pos[1] + a.origin[1]*a.size[1] , a.pos[2] - 0.5*a.size[2]
    elseif a.dir == 4 then -- left
        ax, ay = a.pos[1] - a.size[1] , a.pos[2] - a.origin[2]*a.size[2]
    end
    for j = 1, #npc do
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
    
    a:RMoveTo( dx, dy)
end

local function Human_Act(a)
    local b;
    local ax, ay = a.pos[1] + a.size[1] + 10, a.pos[2] + a.size[2]*a.origin[2]
    if a.over then
        a.found = nil
    end
    for j = 1, flowers do
        b = objs[j]
        if b.Draw then
            local bx, by = b.pos[1] + flosprite.scale[1], b.pos[2]
            if a.dir == 1 then -- down
                ax, ay = a.pos[1] + humarite.w , a.pos[2] + 0.7*humarite.h
            elseif a.dir == 2 then -- right
                ax, ay = a.pos[1] + humarite.w*1.5 , a.pos[2] + 0.5*humarite.h
            elseif a.dir == 3 then -- up
                ax, ay = a.pos[1] + humarite.w , a.pos[2] + 0.2*humarite.h
            elseif a.dir == 4 then -- left
                ax, ay = a.pos[1] + 0.5*humarite.w , a.pos[2] + 0.5*humarite.h
            end
            if CtC(ax, ay , 25, bx, by, 18) then
                local n = #objs
                a.over = true
                a.found = nil
                a.score = a.score + 1
                b.Draw = nil
                flocount = flocount - 1
                break
            end
            if CtC(ax, ay, a.sight, bx, by, 25) and not a.found then
                a.rx, a.ry = b.pos[1], b.pos[2]
                a.found = b
            end
            
        end
        
    end
    for i = humans+1, #npc do
        b = npc[i]
        local bx, by = b.pos[1] + zombarite.scale[1], b.pos[2]
        if CtC(ax, ay, a.sight, bx, by, 25) then
            local x, y = ax-bx, ay-by
            a.rx, a.ry = ax-x, ay-y
            a.found = nil
        end
    end
    a:PlayAnim(a.dir, true, 10, 4)
    a:RMoveTo( dx, dy)
    -- text.value = fmt('flowers: %s\n fps: %s dt: %s\n %s %f %f', flocount, tostring(fps), tostring(dt), tostring(a), a.pos[1], a.pos[2])
end
local new_gen = false
function Npc_Update()
    local len = #npc
    local a;
    -- if new_gen then
    --     for i = #Human.top-1, 1, -1  do
    --         local h = Human.top[i]
    --         local h2 = Human.top[i+1]
            
    --     end
    --     Human.died = 0
    -- end
    for i = 1, len do
        a = npc[i]
        if a.Draw then 
            Npc_Controls(a)
            a:Act(objs)
        else
           new_gen = true
            -- a:reset(math.random(50,screen.w-50), math.random(0,screen.h-60))
            -- a.Draw = humarite.Draw
        end
    end
end

function Obj_Update()
    local len = #objs
    for i = 1, len do
        local a = objs[i]
        if not a.Draw then
            a.pos[1], a.pos[2] = math.random(50,screen.w-50), math.random(0,screen.h-60)
            
            a.Draw = flosprite.Draw
        end
    end
end

function Sim:KeyPress(key, down)
    
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
        Listener:Update()
    end
end
local line = {120,45, 34,56, 300, 230}
Graphics.SetColor(red[1], red[2], red[3], red[4])
function Sim:Draw()
    main:Draw()
    interface:Draw()
    -- Graphics.DrawLines(line)
    Graphics.DrawLine( 300, 300, 200,200)
    Graphics.DrawCircle(false, 20 , 20, 12)
end


print"Sim.root End"

return Sim