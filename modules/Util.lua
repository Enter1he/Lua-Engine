
local Util = {}


local type = type
local sub = string.sub
function Util.CheckArgs (pattern, ...)
    if type(pattern) ~= "string" then
        return error"pattern has wrong type!"
    end
    local arg = {...}
    local ret = true
    for i = 1, #arg do
        local a = sub(pattern,i,i)
        local b = sub(type(arg[i]),1,1)
        ret = ret and (a==b)
        if not ret then
            return false
        end
    end
    return ret;
end
local rand = math.random
function Util.RPos(dx, dy)
    return rand(dx[1], dx[2]), rand(dy[1], dy[2])
end
return Util;