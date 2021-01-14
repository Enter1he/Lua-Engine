

local Sprite = {
    size = {1,1};
    origin = {0.5,0.5};
    anim = 1;
    color = {1,1,1,1};
    frame = 1;
    rate = 0;
    angle = 0;
}

function Sprite:PlayAnim( start, loop, rate, len)

    local r = self.rate
    self.anim = self.anim == start and self.anim or start
    if r < rate then
        self.rate = self.rate + 1
    elseif r == rate then
        
        if self.af < len then
            self.af = self.af + 1
        else
            self.af = loop and 0 or len
        end
        self.rate = 0
        self.frame = self.anim + self.af
    end

end

function Sprite.newSimple(new)
    new.origin = {0.5,0.5}
    new.color = {1,1,1,1}
    new.size = {1,1}
    new.angle = 0

    new.Load = Graphics.LoadSprite
    new.Draw = Graphics.DrawSprite
    new.CopySprite = Sprite.CopySprite

    return new
end

function Sprite.newSheet(new)
    new.origin = {0.5,0.5}
    new.color = {1,1,1,1}
    new.size = {1,1}
    new.angle = 0
    new.anim = 1
    new.frame = 0
    new.rate = 0
    new.af = 0
    
    new.Load = Graphics.LoadSpriteSheet
    new.Draw = Graphics.DrawSpriteSheet
    new.PlayAnim = Sprite.PlayAnim
    new.CopySprite = Sprite.CopySprite
    
    return new
end

function Sprite:CopySprite(copy)
    copy.core = self.core
    copy.size = self.size
    copy.origin = self.origin
    copy.color = self.color
end

return OOP.class('Sprite', Sprite)
