new = setmetatable -- setmetatable used in class creation, so I made it shorter

local prv = {}
local beg = {}
local to = {}

local function DeepCopy(orig, copy)--(table, table)
    local i = 0
    prv[i] = orig
    to[i] = copy
    while i >= 0 do
        for k, v in next, prv[i], beg[i] do
            
            if type(v) == 'table' then
                local c = {}
                to[i][k] = c
                beg[i] = k

                i = i + 1 --Updating depth

                prv[i] = v
                to[i] = c

                goto fin
            else
                to[i][k] = v
            end
        end
        i = i - 1
        ::fin::
    end
end
    

local function ShallowCopy(child, parent) --table, table
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
local Callable = { 
    __call = 
    function(self,t)
        return setmetatable(t or {0,0}, self)
    end
}
local function constructor(name, o)
    o.__name = name or '<unnamed>'
    return setmetatable(o, Callable)
end

local function enum(...) --basically needs strings only
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

OOP = {
    class = class;
    makeCallable = constructor;
    inherit = ShallowCopy;
    inheritDeep = DeepCopy;
    methods = methods;
    enum = enum;
    Gnum = Gnum;
}
