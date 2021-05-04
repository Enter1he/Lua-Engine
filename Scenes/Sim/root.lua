

local Collide = require"Collision"

local K_W, K_w = 87, 119
local K_A, K_a = 65, 97
local K_D, K_d = 68, 100
local K_S, K_s = 83, 115


local dt, screen =_en.dt, _en.screen

local flowers, humans, zombies, npc, objs, player, flosprite, humarite, zombarite;
local text = new({ value = "fr\nam\ne:", pos = {10, 50}, font = Fonts.."consola.ttf", size = 14, color = {1,1,1,1}}, Text)


local tointeger = math.ceil
local DrawSprite = Graphics.DrawSprite
local DrawSpriteSheet = Graphics.DrawSpriteSheet
local DrawCircle = Graphics.DrawCircle
local function drawobj(self)
    local ax, ay = self.pos[1] + self.size[1], self.pos[2]
    
    DrawSprite(self)
    DrawCircle(false, ax, ay, self.size[1],35)
end
local function drawnpc(self)
    local ax, ay = self.pos[1] + self.size[1]+10, self.pos[2] + self.size[2]*self.origin[2]
    
    DrawSpriteSheet(self)
    DrawCircle(false, ax, ay, 10,35)
end 


Controls.AddCommand(B.esc, Close)

Controls.AddCommand(B.z, ResumeLoading)


local Sim = NewScene{
    npc = {};
    objs = {};
    name = "Sim"

}

local std_origin = {0.5, 0.5}
local green = {0,2,0} + Color{1,1,0,1}
local blue = {0,0,1,1}
local red = {1,0,0,1}


local main = Layer.new()

local dx, dy = {50,screen.w-50}, {60,screen.h-60}



function Sim:Load()
    
    npc = self.npc
    objs = self.objs
    player = new(
        {
            Stop = Mob.Stop;
            pos = {0,0};
            vel = {0,0}
        }, Mob)
    player.speed = 100
    text:Load()
    main.pos = player.pos

    -- gen nums of flowers, humans, zombies
    math.randomseed(os.time())
    flowers = 100
    humans = 20
    zombies = 10

    --print("flowers: "..flowers, "humans: "..humans, "zombies: "..zombies) -- print number of mobs

    main:AddDrawable(text)
    ----------------------------------------------ADDING TEXTURES----------------------------------------------------------------
    flosprite, humarite, zombarite = {Draw = drawobj;}, {size = {32,55}; Draw = drawnpc}, {size = {32,55};}
    Sprite.newSimple(flosprite)
    flosprite:Load("Scenes/Sim/res/Plants/Flower.png")
    flosprite.color = green



    Sprite.newSheet(humarite)
    humarite:Load("Scenes/Sim/res/Human/Human", 0)
    humarite.color = blue



    Sprite.newSheet(zombarite)
    zombarite:Load("Scenes/Sim/res/Zomb/Zombie", 0)
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
            local n = i
            local x, y = math.random(50,screen.w-50), math.random(60,screen.h-60) --math.random(dx[1],dx[2]), math.random(dy[1],dy[2])
            local a = {
                __name = 'human';

            }
            Sprite.newSheet(a)
            humarite:CopySprite(a)
            Mob.newMob(a, x, y, 100)

            a.dir = 1
            a.score = 0
            npc[n] = a

            main:AddDrawable(a)
        end
        npc.length = npc.length + humans
    end


    coroutine.yield()

    if zombies > 0 then

        for i = 1, zombies do
            local n = npc.length + i
            local x, y = math.random(50,screen.w-50), math.random(0,screen.h-60)
            local a = {
                __name = 'zombie';

            }
            Sprite.newSheet(a)
            zombarite:CopySprite(a)
            Mob.newMob(a, x, y, 50)

            a.dir = 1
            a.score = 5
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
    -- if npc.core == zombarite.core then
    --     npc.dir = npc.dir + 1
    --     if npc.dir == 4 then
    --         npc.dir = 2
    --     elseif npc.dir == 2 then
    --         npc.dir = 4
    --     elseif npc.dir == 3 then
    --         npc.dir = 5
    --     elseif npc.dir == 5 then
    --         npc.dir = 3
    --     end
    -- end
end


local CtC = Collide.CtC
local function ZombieAct(a)
    local b;

    if a.score <= 0 then

        a:PlayAnim(5, false, 15, 5)

    else
        local ax, ay = a.pos[1] + a.origin[1]*a.size[1] , a.pos[2] - a.origin[2]*a.size[2]
        
        for j = 1, npc.length do
            b = npc[j]
            if b.core == humarite.core then

                local bx, by = b.pos[1] , b.pos[2] - b.origin[2]*b.size[2]
                if CtC(ax, ay, 20, bx, by, 12) then
                    zombarite:CopySprite(b)

                    b.__name = 'zombie'
                    b.speed = a.speed
                    b.score = 0
                    a.score = a.score - 1
                    humans = humans - 1
                    break
                end
            end
        end
        a:PlayAnim(a.dir, true, 15, 5)
        a:RMoveTo( dx, dy)
    end
end
local function HumanMove(a)
    a:PlayAnim(a.dir, true, 15, 5)
    a:RMoveTo( dx, dy)
end
local function HumanAct(a)
    local b;
    local ax, ay = a.pos[1] + a.size[1] + 10, a.pos[2] + a.size[2]*a.origin[2]

    for j = 1, flowers do
        b = objs[j]

        if b.core then
            local bx, by = b.pos[1] + b.size[1], b.pos[2]
            
            if CtC(ax, ay , 10, bx, by, b.size[1]) then
                a.over = true
                b.core = nil
                a.score = a.score + 1
                main:RemoveDrawable(b)
                break
            end
            if CtC(ax, ay, a.sight, bx, by, b.size[1]) then
                a.rx, a.ry = b.pos[1], b.pos[2]
            end
        end
    end
end
local function Npc_Update()
    local i,len = 0, npc.length
    local a;
    for i = 1, len do

        a = npc[i]



        Npc_Controls(a)
        if a.core == zombarite.core then
            ZombieAct(a)

        elseif a.core == humarite.core then
            
            HumanAct(a)
            HumanMove(a)
        end

    end
end

local function Npc_Update2()
    for i = 1, npc.length do
        local a = npc[i]
        local r, r1 = math.random(1,math.random(5)), math.random(1,math.random(5))
        a:RVel_Move(r,r1, dx, dy)
        HumanAct(a)
    end
end

function Sim:Update()

    text.value = string.format('frame_t: %s ms\n fps: %s', tostring(ft), tostring(fps))
    if not paused then

        Npc_Update()
        
    end
    if humans == 0 then
        print"Changing scene..."
        main:ReloadList()
        ChangeScene(1)
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




function Sim:Draw()
    
    
    main:Draw()
    Graphics.color = red
    Graphics.DrawRect(false, 300, 300, 400, 400)
end



return Sim