
local player = new({walk = false}, Mob)
player:Player_Constructor()
Sprite.SpriteSheet(player)


local Base = NewScene{

}


function Base:Load()

    player.frame = 0
    player.color[1] = 0.1875; player.color[2] = 0.515625; player.color[3] = 0.2734375;
    player.pos[1] = 40; player.pos[2] = 40; player.speed = 200;
    
    player:Load("Scenes/Solid Gear's Metal/res/Hard Python/Hard_Python", 'color')
    player.w = player.w * 2; player.h = player.h * 2

end

function Base:Update(dt)

    if player.walk then 
        player:PlayAnim(player.anim, true, 4, 2)
        player.walk = false
    else
        player.frame = player.anim
    end
end


Controls.AddCommand(iup.K_ESC, _Close)
--Controls.AddCommand(iup.K_TAB, function() player.frame = player.frame + 1 end)

function Base:KeyPress(key, down)

    player:Stop()
    
    if key[iup.K_A] or key[iup.K_a] then
        player.anim = 0
        player.vel[1] = -1
    end
    if key[iup.K_D] or key[iup.K_d] then
        player.anim = 3
        player.vel[1] = 1
    end
    if key[iup.K_W] or key[iup.K_w] then
        player.anim = 9
        player.vel[2] = -1
    end
    if key[iup.K_S] or key[iup.K_s] then
        player.anim = 6
        player.vel[2] = 1
    end
    player.walk = player:isMoving()
    player:Vel_Move()

end

function Base:Draw(gl)

    player:Draw()

end

return Base