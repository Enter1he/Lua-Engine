abs = math.abs
local Sprite = require"modules.Sprite"
local moving = require"interfaces.moving"
local function Draw(self)
    self.body:DrawSheet(self.pos, self.angle)
end



local Char = {
    --fields
    body = new({},Sprite);
    
    --methods
    Draw = Draw;
    
}
OOP.inherit(Being, moving)


return OOP.class(Char, "Char")