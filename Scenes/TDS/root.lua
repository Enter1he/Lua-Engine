print("TDS.root")
local Mob = Mob
local sqrt, atan2, deg = math.sqrt, math.atan2, math.deg

local  screen = _en.screen
local w, h = screen.asize()
local sw, sh = screen.w, screen.h
local ax, ay = (sw/w), (sh/h)
local sW, sH, aH = _en.screen.w, _en.screen.h

local flower = {
    pos = {100, 100};
    origin = {0.5, 0.5}
}

local player = { 
    pos = {100, 100};
    origin = {0.5, 0.5};
    size = {1,1};
    vel = {0,0,0};
    speed = 100;
    angle = 0;
    
};
Mob.newMob(player)
Sprite.newSheet(player)
Sprite.newSimple(flower)
player.color = {1,1,1,1};

local flowers = Batch.new()
local men = Batch.new()

local text = Text.newText
{   
    value = "angle";
    pos = {100, 100};
    size = 20;
    color = {1,1,1,1}
    -- font = "Scenes/TDS/res/Teletactile-3zavL.ttf";
}
print(table.unpack(text.color))
print("text")


Controls.AddCommand(B.esc, LE.Close)

Controls.AddCommand(B.e, function()
    player.angle = player.angle + 1
end)

Controls.AddCommand(B.q, function()
    player.angle = player.angle - 1
end)

local step;


local TDS = NewScene{
    
}
local l = Layer.new{
    blend = 0;
}

function TDS:Load()
    
    text:Load()
    print("size!")
    flower:Load("Scenes/TDS/res/Flower/Flower.png")
    player:Load("Scenes/TDS/res/player/TopDownMen")
    
    player.color[4] = 0.5 
    men:Load(player)

    men:Add(150, 20, 2)

    Graphics.BatchShow(men)

    flowers:Load(flower)
    flower.color[4] = 0.5
    -- flowers.angle = 30
    local x, y = 100,50
    for i = 1, 10 do
        
        flowers:Add(x, y)
        x = x+20
        if x > sW then
            x = 0
            y = y + 22
        end
    end
    
    
    l:AddDrawable(flower)
    l:AddDrawable(men)
    
    l:AddDrawable(flowers)
    l:AddDrawable(text)
    l:AddDrawable(player)
end

local fade = 1
function TDS:Update()
    text.value = fps
    player:Stop()
    -- print(player.pos[1])
    if player.walk then
        player:PlayAnim(player.anim, true, 10, 4)
    
    end
    -- player.color[4] = fade

    fade = (fade > 0 )and (fade - dt) or 0 
    flower.angle = flower.angle + dt*100
    
end

function TDS:Draw()
    Graphics.DrawRect(false, 150, 20, 150 + 40, 20 +80)
    -- Graphics.DrawRect(false, flower.pos[1], flower.pos[2], 20, 20)
    Graphics.DrawQuad(false, {30, 20, 30, 40, 40, 40, 40, 20})
    
    l:Draw()    
end

function TDS:KeyPress(key, down)
    -- player.angle = deg(atan2(x*ax - player.pos[1], y*ay - player.pos[2]))
    player:Stop()
    
    if key[B.S] or key[B.s] then
        player.vel[2] = 1
        player.anim = 1
    end
    if key[B.A] or key[B.a] then
        player.vel[1] = -1
        player.anim = 3
    end
    if key[B.D] or key[B.d] then
        player.vel[1] = 1
        player.anim = 2
    end
    if key[B.W] or key[B.w] then
        player.vel[2] = -1
        player.anim = 4
        
    end
    
    player.walk = player:isMoving()
    if not player.walk then 
        player.frame = 0
    end
    player:Vel_Move()
    
end

function TDS:KeyRelease(key)
    
end

function TDS:Motion(x,y)

    
end

return TDS