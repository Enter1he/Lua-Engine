
local Sprite = require"modules.Sprite"

local function Draw(self)
    self.body:Draw(self) --Looks strange, isn't it?
end
Char = {
    player = false;
    pos = {1,1};
    angle = 1;
    vel= {0,0};
    vec = {1,-1};
    speed = 1;
    body = setmetatable({},Sprite);
    Draw = Draw;
    --something new adds here!!!
}

Char.__index = Char
return Char
