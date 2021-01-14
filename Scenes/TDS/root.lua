local Mob = Mob
local sqrt, atan2, deg = math.sqrt, math.atan2, math.deg
local StoXY = StoXY

local sW, sH, aH = _en.screen.w, _en.screen.h


local player = { 
    pos = {60,50};
    origin = {0.5, -0.5};
    angle = 0;
    
    frame = 1;
    rate = 0;
    af = 0;
    speed = 100;
    walk = false;
    Draw = Graphics.DrawSpriteSheet;
    Load = Graphics.LoadSpriteSheet;
    Vel_Move = Mob.Vel_Move;
    Stop = Mob.Stop;
};

local text = new(
{
    pos = {60, 50};
    size = 20;
    font = "Teletactile-3zavL.ttf";
    Draw = Graphics.DrawText;
    Load = Graphics.LoadText;
}, Text)


local flower = new({
    pos = {60, 50};
    Draw = Graphics.DrawSprite;
    Load = Graphics.LoadSprite;
}, Item);

Controls.AddCommand(65307, os.exit)



local step;
local function walk()
    local r =  player.rate
    local c = 3
    if player.walk then
        if r < c then
            player.rate = player.rate + 1
        elseif r == c then
            player.rate = 0
            if player.af < 3 then
                player.af = player.af + 1
            else 
                player.af = 0
            end
            player.frame = player.anim + player.af
        end
    else
        player.af = 0
        player.frame = player.anim
    end
    
end

local TDS = NewScene{
    
}

function TDS:Load()
    print("text.value")
    new(player, Mob)
    
    text:Load()
    flower:Load("Scenes/TDS/res/Flower/Flower.png")
    player:Load("Scenes/TDS/res/player/TopDownMen", -1)
    step = player.src.anim
    player.size[1]= player.size[1] * 2
    player.size[2] = player.size[2] * 2
    text.value = "Hello World"
    
end

function TDS:Update()
    
    player:Stop()
    -- if player.frame < 11 then
    --     player.frame = player.frame + 1
    -- end
    walk()
    player.walk = false
end

function TDS:Draw(gl)
    
    player:Draw()
    text:Draw()
    flower:Draw()
    
end

function TDS:KeyPress(key, down)
    
    player:Stop()
    
    if key[83] or key[115] then
        player.vel[2] = 1
        player.anim = 15
    end
    if key[65] or key[97] then
        player.vel[1] = -1
        player.anim = 5
    end
    if key[68] or key[100] then
        player.vel[1] = 1
        player.anim = 10
    end
    if key[87] or key[119] then
        player.vel[2] = -1
        player.anim = 0
        
    end
    player.walk = player:isMoving()
    player:Vel_Move()
    

end

function TDS:Motion(x,y)

    
end

return TDS