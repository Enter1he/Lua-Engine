
local Sprite = require"modules/Sprite"
Char = {
    player = false;
    pos = {1,1};
    angle = 1;
    vel= {0,0};
    vec = {1,-1};
    speed = 1;
    body = setmetatable({},Sprite);
    --something new adds here!!!
}
local function Draw(self)
    self.body:Draw(self)
end
Char.Draw = Draw
Char.__index = Char
return Char