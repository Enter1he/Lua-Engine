local Mob = Mob
local sqrt, atan2, deg = math.sqrt, math.atan2, math.deg
local StoXY = StoXY
local dt = _en[1]
local sW, sH, aH = _en[2].w, _en[2].h


local player = { 
    pos = {30,30};
    origin = {0.5, 0.39};
    angle = 180;
    frame = 1;
    rate = 0;
    af = 0;
    speed = 100;
    walk = false;
    Draw = Graphics.DrawSpriteSheetAnim;
    Load = Graphics.LoadSpriteSheet;
    Vel_Move = Mob.Vel_Move;
    Stop = Mob.Stop;
};

local text = new(
{
    pos = {60, 50};
    Draw = Graphics.DrawTextListed;
}, Text)


Controls.AddKey(iup.K_W, EmptyFunc)
Controls.AddKey(iup.K_S, EmptyFunc)
Controls.AddKey(iup.K_A, EmptyFunc)
Controls.AddKey(iup.K_D, EmptyFunc)


Controls.AddCommand(iup.K_ESC, _Close)
Controls.AddCommand(iup.K_E, 
    function()
        player.frame = player.frame + 1
    end
)
Controls.AddCommand(iup.K_Q, 
    function()
        player.frame = player.frame - 1
    end
)


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
    
    new(player, Mob)
    
    text:LoadList()
    player:Load"Scenes/TDS/Resources/player/TopDownMen"
    step = player.src.anim
    player.w = player.w + player.w
    player.h = player.h + player.h
    text.value = tostring(iup.K_z)
end

function TDS:Draw(gl)
    
    player:Draw()
    text:Draw()
    
end

function TDS:Update()
    
    player:Stop()
    -- if player.frame < 11 then
    --     player.frame = player.frame + 1
    -- end
    walk()
    player.walk = false
end

function TDS:KeyPress(key, down)
    
    player:Stop()
    if key[iup.K_W] or key[iup.K_w] then
        player.vel[2] = -1
        player.anim = 1
        
    end
    if key[iup.K_S] or key[iup.K_s] then
        player.vel[2] = 1
        player.anim = 16
    end
    if key[iup.K_A] or key[iup.K_a] then
        player.vel[1] = -1
        player.anim = 6
    end
    if key[iup.K_D] or key[iup.K_d] then
        player.vel[1] = 1
        player.anim = 11
    end
    player.walk = player:isMoving()
    player:Vel_Move()
    

end

function TDS:Motion(x,y)

    
end

return TDS