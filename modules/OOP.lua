new = require"New" -- used in class creation, so I made it shorter
local function inherit(child, parent) --table, table
    for k,v in pairs(parent) do
        child[k] = v
    end
end
local function inherit_field(child, parent, field) --table, table, key_value
    if parent[field] then
        child[field] = parent[field]
    else
        error("No such field in "..parent.__name)
    end
end
local function methods(parent, ex)
    for k,v in pairs(parent) do
        ex[k] = type(v) == 'function' and v
    end
end
local function class(o, name) --table
    o.__index = o 
    o.__name = name
    return o
end



local OOP = {
    class = class;
    inherit = inherit;
    methods = methods;
    SetCallbacks = SetCallbacks;
}
return OOP