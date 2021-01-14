
local player = new({walk = false}, Mob)
player:Player_Constructor()
Sprite.SpriteSheet(player)


local Base = NewScene{

}


function Base:Load()

    player.frame = 0
    player.color[1] = 0.1875; player.color[2] = 0.515625; player.color[3] = 0.2734375;
    player.pos[1] = 0; player.pos[2] = 40; player.speed = 100;
    player.origin[1] = 0; player.origin[2] = 0
    player:Load("Scenes/Solid Gear's Metal/res/Hard Python/Hard_Python", 0)
    player.size[1] = player.size[1] * 2
    player.size[2] = player.size[2] * 2
    
    

end

function Base:Update(dt)
    
    if player.walk then 
        player:PlayAnim(player.anim, true, 4, 1)
        play.walk = false
    else
        player.frame = player.anim - 1
    end
    
end


Controls.AddCommand(65307, Close)


function Base:KeyPress(key, down)

    player:Stop()
    
    if key[65] or key[97] then
        player.anim = 1
        player.vel[1] = -1
        
    end
    if key[68] or key[100] then
        player.anim = 4
        player.vel[1] = 1
        
    end
    if key[87] or key[119] then
        player.anim = 10
        player.vel[2] = -1
    end
    if key[83] or key[115] then
        player.anim = 7
        player.vel[2] = 1
    end
    player.walk = player:isMoving()
    player:Vel_Move()

end

function Base:Draw(gl)
    
    player:Draw()

end

return Base