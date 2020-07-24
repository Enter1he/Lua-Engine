local Controls = require"modules.Controls"
local Layer = require"modules.Layer"
local Char = require"modules.Char"

local npccount = 100


local function Load(self, engine)
    self.player.body:Load"Resources/St_Ava/Avatar1"
    math.randomseed(os.time())
    for i = 1, npccount do
        local rand = math.random
        self.npc[i] = new(
            {
                pos = {
                    rand(50, _en.screen.w-50),
                    rand(50, _en.screen.h-50)
                };
                body = self.player.body;
                speed = 100;
            }, Char
        )
    end
    
    
    self.player.speed = 100
    
    self.mLayer:Load()
    self.sLayer:Load()
    self.mLayer.vel = self.player.vel
end
local function Update(self, key)
    local player = self.player; local render = self.mLayer; local render2 = self.sLayer
    local npc = self.npc
    Controls.Key(player,key)
    
    player:Vel_Movement()
    for i = 1, npccount do
        npc[i]:RandomMoveTo(50,_en.screen.h+50)
    end
    render:CenterAt(player.pos[1], player.pos[2])
    render2:CenterAt(player.pos[1], player.pos[2])
    --print(render.pos[1], render.pos[2] )
end
local function Draw(self)
    self.mLayer:Draw({self.player})
    self.sLayer:Draw(self.npc)
    
end

local function Delete(self)
    self.player.body:Delete()
end
local Scene = {
    player = new({pos = {_en.screen.w*0.5,_en.screen.h*0.5}}, Char);
    mLayer = new({}, Layer);
    sLayer = new({pos = {0,0}}, Layer);
    npc = {};

    Load = Load;
    Draw = Draw;
    Update = Update;
    Delete = Delete;
}


return Scene