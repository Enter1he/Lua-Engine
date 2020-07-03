
Controls = {}
local ControlCheckU = function(key, codeU)
    local codeL =  utf8.char(codeU + 32)
    codeU = utf8.char(codeU) 
    return (key:match(codeU) or key:match(codeL))
end
Controls.Key = function(player, key, engine)
    
    local w, h = player.body.w, player.body.h
    local velX, velY = player.pos[1], player.pos[2]
    local delta = player.speed * engine.dt
    local charu, charl = string.char(iup.K_D), string.char(iup.K_d)
    
    player.vel[1] = 0
    player.vel[2] = 0

    
    if ControlCheckU(key, iup.K_W) then
        player.vel[2] = player.vec[1]
        player.angle = 4
    end
    if ControlCheckU(key, iup.K_S) then
        player.vel[2] = player.vec[2]
        player.angle = 2
    end
    if ControlCheckU(key, iup.K_D) then
        player.vel[1] = player.vec[1]
        player.angle = 1
    end
    if ControlCheckU(key, iup.K_A) then
        player.vel[1] = player.vec[2]
        player.angle = 3
    end

end

Controls.Motion = function(player, x,y)
    local px, py = player.pos[1]*8, player.pos[2]*30
    local dx, dy = px-x,  py - y 
    
    player.body.angle = math.deg(math.atan(dy,dx))
end

Controls.Button = function()

end