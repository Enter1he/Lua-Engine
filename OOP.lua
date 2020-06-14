OOP = {}

local set = function( table, key, value)
    if table[key] and not string.find(key,"__") then
        rawset(table, key, value)
    end
end

local get = function(self,table, key)
    return self[key]
end

OOP.class = function(cont)
    cont.__index = cont
    cont.__newindex = set
    return cont
end

OOP.const = function(cont)
    cont.__index = cont
    cont.__newindex = nil
    return cont
end


return OOP