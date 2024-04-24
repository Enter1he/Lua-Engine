local Sprite = require"classes/Sprite"

local Batch ={

}


local Graphics = Graphics
local setmetatable = setmetatable

Batch.__gc = Graphics._BatchGC

function Batch.new(new)
    new = new or {}
    local _ENV = new
    pos = pos or {0,0,0}
    origin = origin or {0.5,0.5}
    color = color or {1,1,1,1}
    size = size or {1,1}
    angle = angle or 0
    anim = anim or 1
    frame = frame or 1
    rate = rate or 1
    visible = visible or true
    
    Load = Load or Graphics.LoadBatch
    Draw = Draw or Graphics.DrawBatch
    Add = Add or Graphics.BatchAdd
    Show = Show or Graphics.BatchShow
    PlayAnim = Sprite.PlayAnim
    CopySprite = Sprite.CopySprite
    GetSize = Graphics.GetSize
    isSprite = Sprite.isSprite
    
    return setmetatable(new, Batch)
end

return Batch