require"modules/OOP"
require"modules/Image"
Char = {
    player = false,
    pos = {1,1},
    speed = 1,
    Str = setmetatable({},Image),
    
}

OOP.class(Char)
return Char