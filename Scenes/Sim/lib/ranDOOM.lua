-- ranDOOOM.lua
-- a lua library for generating numbers with DOOM-style table algorithm


local rDm = {}

local seed = os.time()

local function randomseed(sd)
    seed = sd
end

local unpack = unpack;
if not unpack then
    unpack = table.unpack
end

local random = math.random
local min, max = function()end, function()end --some private variables' names 

--[[
    ranDOOM.ShuffleTable(t: table, rand: function(m, n: integer))
    Shuffles already created ranDOOM table using 'rand' function.
    The function should be like math.random from Lua standard library.
]]
function rDm.ShuffleTable(t, rand)
    rand = rand or random
    local n = t[max]
    local m = t[min]
    for i = m, n do
        local pos = rand(m,n)
        t[pos], t[i] = t[i], t[pos]
        
    end
end

--[[
    ranDOOM.genTable (m, n: integer)
    generates ranDOOM table from minimum ('m') and maximum ('n') values
]]
function rDm.genTable(m,n)
    local funcname = "ranDOOM.genTable"
    local t = {}
    if type(n) ~= "number" or type(m) ~= "number" then
        if math.type(n) ~= "integer" or math.type(m) ~= "integer" then
            error(funcname.." accepts only integers!")
        end
    end
    if m > n then
        print(debug.traceback())
        error(funcname.." 1st value can't be bigger than 2nd!!")
    end

    for i = m, n do
        t[i] = i
    end
    t[min] = m
    t[max] = n
    rDm.ShuffleTable(t, random)
    return t
end

--[[
    ranDOOM.genTableN (N, m,n: integer)
    generates ranDOOM table of values with indicies from 1 to N and minimum ('m') to maximum ('n') values
    in each index
]]
function rDm.genTableN(N, m,n)
    local funcname = "ranDOOM.genTableN"
    local t = {}
    if type(n) ~= "number" or type(m) ~= "number" or type(N) ~= "number" then
        if not math.type then
            N,m,n = math.floor(N), math.floor(m), math.floor(n)
        else
            if math.type(n) ~= "integer" or math.type(m) ~= "integer" or math.type(N) ~= "integer" then
                error(funcname.." accepts only integers!")
            end
        end
    end
    if m >= n then
        error(funcname.." 1st value can't be bigger than 2nd!!")
    end

    for i = 1, N do
        t[i] = random(m,n)
    end
    t[min] = 1
    t[max] = N
    rDm.ShuffleTable(t, random)
    return t
end

--[[
    ranDOOM.genTableNV (arg: table)
    generates ranDOOM table of values with indicies from 1 to N by using special table arg
    The arg itself should be created in form such as:
    arg = {
        [m] = 1..int(max);
        ...
        [n] = 1..int(max);
    }
    where m is the smallest index of entry and n is a biggest one with their respectative values
    meaning the amount of indicies in ranDOOM table with said key as a value

    example:
    arg = {[3] = 3, [5] = 5}

    resulting table will be
    t = {3, 3, 3, 5, 5, 5, 5, 5} -- if not shuffled
]]
function rDm.genTableNV(arg)
    local t = {}
    local N = 0
    local vals, nums = {}, {}
    for i,v in pairs(arg) do
        vals[#vals + 1] = i
        nums[#nums + 1] = v
        N = N + v
    end
    local n = 0
    local j = 1
    for i = 1, N do
            if n < nums[j] then
                n = n + 1            
            else
                j = j + 1
                n = 0
            end
            t[i] = vals[j]
    end
    t[min] = 1
    t[max] = N
    rDm.ShuffleTable(t, random)
    return t
end

--[[
    ranDOOM.print ( t : table )
    prints the contents of ranDOOM table
]]
function rDm.print(t)
    local funcname = "ranDOOM.print"
    if type(t) ~= "table" then
        error(funcname.." expected table!!")
    end
    local str = string.format("min %d, max %d ", t[min], t[max])
    for i = t[min], t[max] do
        str = str .. (tostring(t[i]) .. " ")        
    end
    print(str)
end

--[[
    rDm.makeRand (t : table, n, start : integer )
    Creates new randomizing function based on table 't',
    number of indicies 'n' and starting position 'start'
]]
function rDm.makeRand(t, start)
    start = start or 1
    local funcname = "ranDOOM.makeRand"
    if not t[min] or not t[max] then
        error(funcname .. " no required fields in table! Have you passeed a ranDOOM generated table?")
    end
    
    if type(start) == "number" and math.type(start) ~= "integer" then
        error(funcname.." starts from integer position")
    end
    
    if start < t[min] and start > t[max] then
        error(funcname.." can't start from outside of table!!")
    end
    local t = t or rDm.genTable(t[min],t[max])
    local i = start or 1
    return function() -- rand function generated
        
        if i < t[max] then
            i = i + 1
        else
            i = t[min]
        end
        
        return t[i]
    end
end

--[[
    ranDOOM.saveTable(t: table, folder: string)
    Saves ranDOOM table into file at folder.
]]
function rDm.saveTable(t, folder)
    local funcname = "ranDOOM.saveTable"
    if type(t) ~= "table" then
        error(funcname.." accepts only tables to save")
    end
    if type(folder) ~= "string" then
        error(funcname.." folder should be a string value")
    end
    
    local f = io.open(folder, "w+")
    if not f then
        error(funcname.." error opening file to save")
    end
    f:write(string.format("return {\n"))
    
    for i = t[min], t[max], 1 do
        f:write(t[i]..", ")
    end
    
    f:write(string.format("\n};"))
    f:close()
end

return rDm;