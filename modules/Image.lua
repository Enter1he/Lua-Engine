--ASCII Image 
require"luagl"
require"modules/OOP"
Image = {
    size = {1,1};
    pix = "";
    angle = 0
}


Image.Load = function(self, fold)
    local file = io.open(fold,"r")
    local w, h = 1,1
    self.pix = file:read("*a")
    self.pix, h = self.pix:gsub("%c","")
    h = h + 1
    w = self.pix:len()//h
    file:close()
    self.size = {w,h}
end

Image.Draw = function(self, canvas, wsize, pos)
    
    local w,h = self.size[1], self.size[2] 
    
    gl.Color(1.0,0.0,0.0)
    self.pix:gsub("[^%s]()", function(s)
        s = s - 1
        local x, y = pos[1]+(s%w), pos[2]+(s//h)
        
        gl.Rect((x), (y), (x+1), (y+1))
        return "s"
    end)
end
OOP.class(Image)

return Image