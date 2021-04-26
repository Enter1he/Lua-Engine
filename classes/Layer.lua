local screen = _en.screen

local order, list = {}, {}


local Layer = {
    [list] = {};
    pos = {0,0};
    size = {1,1};
    Draw = Graphics.DrawLayer;
}



local half = 0.5

function Layer:CenterAt(x, y)
    local _ENV = self
    pos[1], pos[2] = x - screen.w*half, y - screen.h*half
end


function Layer:AddDrawable(drawable) 
    local list = self[list]
    local i = #list + 1
    list[i] = drawable
    drawable[order] = i
end


function Layer:RemoveDrawable(i)
    local list = self[list]
    local n = #list
    list[n], list[i] = list[i], list[n]
    list[i][order] = i
    list[n] = nil
end


function Layer.new(new)
    local _ENV = new or {}
    pos = pos or {0,0}
    size = size or {1,1}
    _ENV[list] = _ENV[list] or {}
    Draw = Layer.Draw
    CenterAt = Layer.CenterAt
    return _ENV
end


return OOP.class('Layer', Layer)