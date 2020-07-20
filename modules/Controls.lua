

local function ControlCheckU(key, codeU)
    local codeL =  utf8.char(codeU + 32)
    codeU = utf8.char(codeU) 
    return (key:match(codeU) or key:match(codeL))
end
local function ForKeys(player, key)
    -- Just for example
    player.vel[1] = 0
    player.vel[2] = 0
    
    if ControlCheckU(key, iup.K_D) then
        player.vel[1] = 1
        player.angle = 1
    elseif ControlCheckU(key, iup.K_A) then
        player.vel[1] = -1
        player.angle = 3
    end
    if ControlCheckU(key, iup.K_W) then
        player.vel[2] = 1
        player.angle = 4
    elseif ControlCheckU(key, iup.K_S) then
        player.vel[2] = -1
        player.angle = 2
    end
    
end


local function ForMouseMotion(player, x,y)
    local px, py = player.pos[1]*8, player.pos[2]*30
    local dx, dy = px-x,  py - y 
    
    player.body.angle = math.deg(math.atan(dy,dx))
end


local function ForMouseButons()

end

local Controls = {
    Button = ForMouseButons;
    Motion = ForMouseMotion;
    Key = ForKeys;
}
return Controls