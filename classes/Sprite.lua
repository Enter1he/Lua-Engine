local Graphics = Graphics

local Sprite = {
    size = {1,1};
    origin = {0.5,0.5};
    anim = 1;
    color = {1,1,1,1};
    frame = 0;
    rate = 0;
    angle = 0;
}

local af = Lalloc(1)


function Sprite:PlayAnim( anim, loop, rate, len)
    local r = self.rate
    self.anim = anim
    if r < rate then
        self.rate = self.rate + 1
    elseif r == rate then
        
        if self[af] < len then
            self[af] = self[af] + 1
        else
            self[af] = loop and 1 or self[af]
        end
        self.rate = 0
        self.frame = self[af]
    end

end

function Sprite.newSimple(new)
    new = new or {}
    local _ENV = new
    pos = pos or {0,0,0}
    origin = origin or {0.5,0.5}
    color = color or {1,1,1,1}
    size = size or {1,1}
    angle = angle or 0

    Load = Load or Graphics.LoadSprite
    Draw = Draw or Graphics.DrawSprite
    CopySprite = Sprite.CopySprite

    return new
end

function Sprite.newSheet(new)
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
    new[af] = new[af] or 1
    
    Load = Load or Graphics.LoadSpriteSheet
    Draw = Draw or Graphics.DrawSpriteSheet
    PlayAnim = Sprite.PlayAnim
    CopySprite = Sprite.CopySprite
    GetSize = Graphics.GetSize
    
    return new
end

function Sprite:CopySprite(copy)
    copy.core = self.core
    copy.size = self.size
    copy.origin = self.origin
    copy.color = self.color
    copy.src = self.src
    copy.Draw = self.Draw
    copy.Load = self.Load
end

return OOP.class('Sprite', Sprite)
