local Mob = Mob
local sqrt, atan2, deg = math.sqrt, math.atan2, math.deg


local sW, sH, aH = _en.screen.w, _en.screen.h

local flower = {
    pos = {100, 100}
}

local player = { 
    pos = {100,50};
    origin = {0.5, -0.5};
    size = {1,1};
    vel = {0,0,0};
    speed = 100;
    
};
Mob.newMob(player)
Sprite.newSheet(player)
Sprite.newSimple(flower)

local flowers = Batch.new()

local text = new(
{   
    value = "angle";
    pos = {60, 50};
    size = 20;
    font = "Scenes/TDS/res/Teletactile-3zavL.ttf";
    Draw = Graphics.DrawText;
    Load = Graphics.LoadText;
}, Text)



Controls.AddCommand(B.esc, LE.Close)



local step;


local TDS = NewScene{
    
}
local l = Layer.new{}

function TDS:Load()
    
    text:Load()
    flower:Load("Scenes/TDS/res/Flower/Flower.png")
    player:Load("Scenes/TDS/res/player/TopDownMen")
    
    flowers:Load(flower)
    local x, y = 0,10
    for i = 1, 70 do
        
        flowers:Add(x, y)
        x = x+10
        if x > sW then
            x = 0
            y = y + 22
        end
    end
    flowers:Show()
    -- l:AddDrawable(flower)
    -- l:AddDrawable(text)
    l:AddDrawable(player)
    l:AddDrawable(flowers)
end

function TDS:Update()
    
    player:Stop()
    
    if player.walk then
        player:PlayAnim(player.anim, true, 10, 4)
    
    end
    
    
end

function TDS:Draw()
    l:Draw()    
end

function TDS:KeyPress(key, down)
    
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