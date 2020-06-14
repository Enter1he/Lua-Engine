--engine
require"iuplua"
require"iupluagl"
-- require"luagl"
require"modules/Char"
require"modules/Image"
require"modules/OOP"
require"modules/Render"
new = setmetatable
Engine = {}
iup.SetGlobal("UTF8MODE", "Yes")
Engine.Chars = {
    new({},Char);
}
Engine.Renders = {
    new({},Render)
}
local publ = {}
local mRender = Engine.Renders[1]
local player = Engine.Chars[1]
player.pos[1], player.pos[2] = 1, 1
player.Str:Load("maps/Im.txt")

Engine.output = iup.glcanvas{
    buffer="DOUBLE"; 
    rastersize = "256x240";
    
}

Engine.LoopTime = iup.timer{time = 10; run = "no"}

Engine.dialog = iup.dialog
{
    
        Engine.output;
    
    title="ASCII Console"; 
    size="640x480"; -- initial size
    icon=0; -- use the Lua icon from the executable in Windows
    
}


Engine.Load = function(self)
    publ = self
    mRender.field = require"modules/Loader".Load("maps/map.txt")
end


Engine.output.action = function(self)
    iup.GLMakeCurrent(self)
    
    mRender:Draw(publ.Chars)
    iup.GLSwapBuffers(self)
    gl.Flush()
end


Engine.output.resize_cb = function(self, width, height)
  iup.GLMakeCurrent(self)

  gl.Viewport(0, 0, width, 1.8*height//1)
  
  gl.MatrixMode('PROJECTION')
  gl.LoadIdentity()

  gl.MatrixMode('MODELVIEW')
  gl.LoadIdentity()

end

Engine.LoopTime.action_cb = function(self)
    iup.LoopStep()
    
    iup.Update(publ.output)
end

Engine.output.keypress_cb = function(self, key, press)
    local fw, fh = mRender.field[2], mRender.field[3]
    local w, h = player.Str.size[1], player.Str.size[2]
    local posX, posY = player.pos[1], player.pos[2]
    if press == 1 then
        if --[[move right]]key==iup.K_D or key==iup.K_d then
            if posX + w//2 + player.speed <= fw+1 then
                player.pos[1] = player.pos[1] + player.speed
                player.Str.angle = 0
                
            end
        elseif --[[move left]]key==iup.K_A or key==iup.K_a then
            if posX - w//2- player.speed >= -1 then
                player.pos[1] = player.pos[1] - player.speed
                player.Str.angle = 2
            end
        elseif --[[move down]]key==iup.K_S or key == iup.K_s then
            if posY + h//2 + player.speed <= fh then
                player.pos[2] = player.pos[2] + player.speed
                player.Str.angle = 1
            end
        elseif --[[move up]]key==iup.K_W or key == iup.K_w then
            if posY + h//2 - player.speed >= -1 then
                player.pos[2] = player.pos[2] - player.speed
                player.Str.angle = 3
            end
        elseif --[[interact]]key==iup.K_E or key == iup.K_e then
            
        elseif --[[action]]key==iup.K_E or key == iup.K_e then
        --elseif --[[button]]key==iup. or key == iup. then
        end 
    end
end

Engine.Loop = function(self)
    self.dialog:showxy(0,0)
    iup.SetFocus(self.output)
    self.LoopTime.run = "yes"
    if (iup.MainLoopLevel() == 0) then
        iup.MainLoop()
        iup.Close()
      end
end
OOP.class(Engine)

return Engine