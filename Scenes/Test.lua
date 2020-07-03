require"modules/Render"
local Draw = function(self)
    self.mRender:Draw({self.player})
end
local Update = function(self, key, engine)
    local player = self.player
    Controls.Key(player,key, engine)
    player.pos[1] = player.pos[1] + player.vel[1] * player.speed * engine.dt
    player.pos[2] = player.pos[2] + player.vel[2] * player.speed * engine.dt
end
local Load = function(self)
    self.player.body:Load"Resources/St_Ava/Avatar1"
    self.player.speed = 10
    self.mRender:Load()
end
Scene = {
    player = new({}, Char);
    mRender = new({}, Render);
    Load = Load;
    Draw = Draw;
    Update = Update;
}


return Scene