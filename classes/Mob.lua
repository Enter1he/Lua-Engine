

local living = require"interfaces.living"




local Mob = {}

OOP.inherit(Mob, living)
OOP.inherit(Mob, Sprite)



return OOP.class('Mob', Mob)