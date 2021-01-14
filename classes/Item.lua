

local item = {
    pos = {0,0};
}

OOP.inheritDeep(item, Sprite)

function item.newItem(new)
    OOP.iherit(new, Sprite)
    new.pos = {0,0}

end

return OOP.class('item', item)