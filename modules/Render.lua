

local function Load(self)
    
    self.asp = _en.screen.w/_en.screen.h
    self:CenterAt(self.pos[1] + _en.screen.w, self.pos[2] + _en.screen.h)
end


local function Draw(self, Chars)
    local x, y = self.pos[1], self.pos[2]
    
    gl.PushMatrix()
        
        gl.Translate(-x, -y, 0)
        Chars[1]:Draw()
        
    gl.PopMatrix()
    
end

local function CenterAt(self, x, y)
    self.pos[1], self.pos[2] = x - _en.screen.w*0.5, y - _en.screen.h*0.5
end



local Render = {
    
    pos = {0,0};
    scale = {32,32};
    Load = Load;
    Draw = Draw;
    CenterAt = CenterAt;
}




return OOP.class(Render, "Render")