require"luagl"
require "modules/OOP"
Render = {
    res = {640,480};
    field = {"", 0, 0};
}

Render.Draw = function(self, Chars)
    local res = self.res
    local w,h = self.field[2]+1, self.field[3]
    local tile = 1
    gl.MatrixMode("PROJECTION")
    gl.LoadIdentity()
    gl.Ortho(0, 1, 1, 0, -1.0, 1.0)
    gl.MatrixMode("MODELVIEW")
    gl.LoadIdentity()
    gl.PushMatrix()
    gl.Translate(0.1,0.5, 0)
    gl.Scale(0.03125, 0.03125, 1)
    gl.ClearColor(0,0,0,1)
    gl.Clear("DEPTH_BUFFER_BIT,COLOR_BUFFER_BIT")
    for i = 1, #Chars, 1 do
        if Chars[i].pos[1] >= 0 and Chars[i].pos[1] < self.res[1] then 
            Chars[i].Str:Draw(self.field[1], self.field[2], Chars[i].pos)
        end
    end
    gl.Color(1.0, 1.0, 1.0)
    _ = self.field[1]:gsub("[#]()", function(s)
        -- s = s -1
        local x, y = (s%w), (s//h)
        gl.Rect( x, y, x+1, y+1 )
        return "s"
    end)
    
end

OOP.class(Render)

return Render