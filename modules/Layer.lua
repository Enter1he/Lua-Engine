

local function Load(self)
    
    self.asp = _en.screen.w/_en.screen.h
    self:CenterAt(self.pos[1] + _en.screen.w, self.pos[2] + _en.screen.h)
end


local function Draw(self, Chars)
    local x, y = self.pos[1], self.pos[2]
    for i=1, #Chars do
        gl.PushMatrix()
            
            gl.Translate(-x+0.5, -y+0.5, 0)
            Chars[i]:Draw()
            
        gl.PopMatrix()
    end
end

local function CenterAt(self, x, y)
    self.pos[1], self.pos[2] = x - _en.screen.w*0.5, y - _en.screen.h*0.5
end



local Layer = {
    
    pos = {0,0};
    scale = {32,32};
    Load = Load;
    Draw = Draw;
    CenterAt = CenterAt;
}




return OOP.class(Layer, "Layer")