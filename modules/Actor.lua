abs = math.abs
local Sprite = require"modules.Sprite"
local walkable = require"interfaces.walkable"
local function Draw(self)
    self.body:DrawSheet(self.pos, self.angle)
end



local Actor = {
    --fields
    angle = 1;
    body = setmetatable({},Sprite);

    --methods
    Draw = Draw;
    
}
OOP.inherit(Actor, walkable)


return OOP.class(Actor, "Char")