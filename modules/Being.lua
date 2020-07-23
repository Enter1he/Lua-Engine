abs = math.abs
local Sprite = require"modules.Sprite"
local living = require"interfaces.living"
local function Draw(self)
    self.body:DrawSheet(self.pos, self.angle)
end



local Being = {
    --fields
    body = new({},Sprite);
    
    --methods
    Draw = Draw;
    
}
OOP.inherit(Being, living)


return OOP.class(Being, "Being")