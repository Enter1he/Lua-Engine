abs = math.abs
local Sprite = require"classes.Sprite"
local living = require"interfaces.living"




local Mob = {
    --fields
    body = {};
    frame = 1;
    rate = 0;
    --methods
    
}
OOP.inherit(Mob, living)



return OOP.class(Mob, "Mob")