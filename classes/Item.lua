local dead = require"interfaces.dead"

local item = {
    
}

OOP.inherit(item, dead)
OOP.inherit(item, Sprite)

return OOP.class('item', item)