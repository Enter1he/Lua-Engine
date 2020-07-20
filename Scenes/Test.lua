
local Render = require"modules.Render"
local Actor = require"modules.Actor"
local function Draw(self)
    self.mRender:Draw({self.player})
    self.sRender:Draw({self.npc})
    
end
local function Update(self, key)
    local player = self.player; local render = self.mRender; local render2 = self.sRender
    local npc = self.npc
    Controls.Key(player,key)
    
    player:Vel_Movement()
    npc:Patrol({5,20}, {5, 400})
    -- render:CenterAt(player.pos[1], player.pos[2])
    -- render2:CenterAt(player.pos[1], player.pos[2])
    --print(render.pos[1], render.pos[2] )
end
local function Load(self, engine)
    self.player.body:Load"Resources/St_Ava/Avatar1"
    self.npc.body = self.player.body
    
    self.player.speed = 100
    self.npc.speed = 100
    self.mRender:Load()
    self.sRender:Load()
    self.mRender.vel = self.player.vel
end
local Scene = {
    player = new({pos = {0,0}}, Actor);
    mRender = new({}, Render);
    sRender = new({pos = {0,0}}, Render);
    npc = new({pos = {35,35}}, Actor);

    Load = Load;
    Draw = Draw;
    Update = Update;
}


return Scene