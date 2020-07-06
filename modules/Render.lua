
local Render = {
    res = {640,480};
    field = {"", 0, 0};
    canvas = new({w = 640, h = 480},Sprite);
    buffer = 0;
}
Render.asp = {16/Render.res[1],16/Render.res[2]}

Render.Load = function(self)
    self.canvas:Load()
    
    
end

Render.Draw = function(self, Chars)
    local res = self.res
    local asp = self.asp 
    local ox, oy = -1, -1
    --local w,h = self.field[2]+1, self.field[3]
    
    Chars[1]:Draw()
    
end

Render.__index = Render

return Render