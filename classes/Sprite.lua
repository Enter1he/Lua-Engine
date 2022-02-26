local Graphics = Graphics

local Sprite = {
    size = {1,1};
    origin = {0.5,0.5};
    anim = 1;
    color = {1,1,1,1};
    frame = 0;
    rate = 0;
    angle = 0;
    visible = true;
}




function Sprite:PlayAnim( anim, loop, rate, len)
    local r = self.rate
    if anim > #self.src.anim then 
        return error("No such animation")
    end
    if self.anim ~= anim then
        self.frame = 1
        
    end
    self.anim = anim
    self.rate = self.rate + 1
    if r >= rate then
        local af = self.frame
        af = af + 1
        af = af > len and (loop and 1 or len) or af
        self.rate = 0
        self.frame = af
        return;
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
    visible = visible or true

    Load = Load or Graphics.LoadSprite
    Draw = Draw or Graphics.DrawSprite
    CopySprite = Sprite.CopySprite
    isSprite = Sprite.isSprite

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
    visible = visible or true
    
    Load = Load or Graphics.LoadSpriteSheet
    Draw = Draw or Graphics.DrawSpriteSheet
    PlayAnim = Sprite.PlayAnim
    CopySprite = Sprite.CopySprite
    GetSize = Graphics.GetSize
    isSprite = Sprite.isSprite
    
    return new
end

function Sprite:CopySprite(copy)
    copy._sprite = self._sprite
    copy.size = self.size
    copy.origin = self.origin
    copy.color = self.color
    copy.src = self.src
    copy.Draw = self.Draw
    copy.Load = self.Load
    copy.visible = self.visible
end

function Sprite:isSprite(spr)
    local t = type(spr)
    if t == 'userdata' then
        return self._sprite == spr;
    elseif t == 'table' then
        return self._sprite == spr._sprite;
    end

    return nil
end



return OOP.class('Sprite', Sprite)
