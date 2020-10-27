new = require"New" -- used in class creation, so I made it shorter
undef = require"Undef"

local metaDef = {
    __tostring = function() return "undef" end; 
    __name = "undef";
    __call = EmptyFunc;
    __index = EmptyFunc;
    __newindex = EmptyFunc;
}

debug.setmetatable(undef, metaDef)

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
local function class(name, o) --table
    o.__index = o 
    o.__name = name or '<unnamed>'
    return o
end

local function enum(...) --basically needs strigs only
    local arg = {...}
    local e = {}
    for i = 1, #arg do
        e[arg[i]] = i
    end
    return e
end

local function Gnum(...)
    local arg = {...}
    local e = _G
    for i = 1, #arg do
        e[arg[i]] = i
    end
end

local OOP = {
    class = class;
    inherit = inherit;
    methods = methods;
    enum = enum;
    Gnum = Gnum;
}

return OOP