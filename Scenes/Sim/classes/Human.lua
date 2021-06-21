
local Human;
Human = {

}

function Human.new(x,y)
    local a = {
        sight  = 200;
        dir = 1;
        score = 0;
        Act = Human.Act;
    };
    Sprite.newSheet(a)
    Mob.newMob(a, x, y, 100)
    return a;
end

function Human:Act()
end

return OOP.class("Human", Human) ;