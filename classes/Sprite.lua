

local Sprite = {
    origin = {-0.5,-0.5};
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

function Sprite:SimpleSprite()
    self.origin = {-0.5,-0.5}
    self.color = {1,1,1,1}
    self.angle = 0

    self.Load = Graphics.LoadSprite
    self.Draw = Graphics.DrawSprite
end

function Sprite:SpriteSheet()
    self.origin = {-0.5,-0.5}
    self.color = {1,1,1,1}
    self.angle = 0
    self.anim = 1
    self.frame = 1
    self.rate = 0
    self.af = 0
    
    self.Load = Graphics.LoadSpriteSheet
    self.Draw = Graphics.DrawSpriteSheetAnim
    self.PlayAnim = Sprite.PlayAnim
end



return OOP.class('Sprite', Sprite)
