local screen = _en.screen
print(LE.Lalloc)
local order, list = LE.Lalloc(1), LE.Lalloc(1)


local Layer = {
    [list] = {};
    pos = {0,0};
    size = {1,1};
    
}

local LDraw = Graphics.DrawLayer
local half = 0.5


function Layer:CenterAt(x, y)
    local _ENV = self
    pos[1], pos[2] = x - screen.w*half, y - screen.h*half
end

function Layer:ReloadList()
    self[list] = {}
end


function Layer:AddDrawable(drawable) 
    local l = self[list]
    local i = #l + 1
    l[i] = drawable
    drawable[order] = i
end


function Layer:RemoveDrawable(drawable)
    local i = drawable[order]
    local l = self[list]
    local n = #l
    
    l[n], l[i] = l[i], l[n]
    if not l[i] then
        for k,v in pairs(l) do
            print(k,v)
        end
        print(i, l[i], drawable)
        return;
    end
    l[i][order] = i
    l[n][order] = nil
    l[n] = nil
end

function Layer:isDrawable(drawable)
    return drawable[order]
end


function Layer:Draw()
    LDraw(self, self[list])
end


function Layer.new(new)
    new = new or {}
    local _ENV = new


    pos = pos or {0,0}
    size = size or {1,1}
    blend = blend or 1
    new[list] = new[list] or {}
    Draw = Layer.Draw
    CenterAt = Layer.CenterAt
    AddDrawable = Layer.AddDrawable
    RemoveDrawable = Layer.RemoveDrawable
    ReloadList = Layer.ReloadList
    isDrawable = Layer.isDrawable

    return _ENV
end


return OOP.class('Layer', Layer)