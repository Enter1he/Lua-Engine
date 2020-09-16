

local function Load(self)
    
    self.asp = _en[2].w/_en[2].h
    self:CenterAt(self.pos[1] + _en[2].w, self.pos[2] + _en[2].h)
end


local function Draw(self, Objs)
    local x, y = self.pos[1], self.pos[2]
    for i=1, #Objs do
        gl.PushMatrix()
            
            gl.Translate(-x+0.5, -y+0.5, 0)
            Objs[i]:Draw()
            
        gl.PopMatrix()
    end
end

local function CenterAt(self, x, y)
    self.pos[1], self.pos[2] = x - _en[2].w*0.5, y - _en[2].h*0.5
end



local Layer = {
    
    pos = {0,0};
    scale = {32,32};
    Load = Load;
    Draw = Draw;
    CenterAt = CenterAt;
}




return OOP.class('Layer', Layer)