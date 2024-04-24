-- use.t
local function prinT(t) -- for debug
    for k, v in pairs(t) do
        print(k,v)
    end
end

local builtin = {
    _G  = true;
    _VERSION = true;
    assert = true;
    collectgarbage = true;
    dofile = true;
    error = true;
    getfenv = true;
    getmetatable = true;
    ipairs = true;
    load = true;
    loadfile = true;
    loadstring = true;
    module = true;
    next = true;
    pairs = true;
    pcall = true;
    print = true;
    rawequal = true;
    rawget = true;
    rawset = true;
    require = true;
    select = true;
    setfenv = true;
    setmetatable = true;
    tonumber = true;
    tostring = true;
    type = true;
    unpack = true;
    xpcall = true;
    terralib = true;
    coroutine = true;
    table = true;
    math = true;
    debug = true;
    string = true;
    os = true;
    package = true;
    io = true;
    gcinfo  = true;
    newproxy = true;
    int16   = true;
    vector = true;
    intptr = true;
    ptrdiff = true;
    global = true;
    Strict = true;
    bool  = true;
    int = true;
    symbol = true;
    bit  = true;
    double = true;
    long = true;
    jit = true;
    macro = true;
    ["terra"] = true;
    tuple = true;
    arg = true;
    array = true;
    niltype = true;
    operator = true;
    uint64 = true;
    sizeof = true;
    uint16 =true;
    int32  = true;
    arrayof = true;
    constant  = true;
    rawstring  = true;
    uint = true;
    opaque = true;
    int64 = true;
    uint32 = true;
    label = true;
    float = true;
    int8 = true;
    uint8 = true;
    unpacktuple = true;
    unpackstruct = true;
    vectorof = true;
}

return {
    name = "use";
    entrypoints = {"use"};
    keywords = {};
    statement = function(_,lex)
        lex:expect"use"
        local module, name, tab, stab
        if lex:matches(lex.name) then
            name = lex:expect(lex.name).value
            lex:ref(name)

            while lex:nextif"." do
                if not tab then
                    tab = {}
                    stab = tab
                end
                local n = lex:expect(lex.name).value
                tab[n] = {}
                tab = tab[n]
            end
            tab = stab
        else
            module = lex:luaexpr()(_G)
            local ty = type(module)
            if ty == "string" then
                module = require(module)
            elseif ty ~= "table" then
                error"only tables or strings excepted by 'use'!"
            end
        end
        return function(env_fn)
            local env = env_fn()
            local m = module or env[name]
            local k
            while tab do
                k = next(tab,nil)
                if not k then
                    break
                end
                m = m[k]
                tab = tab[k]
            end
            if type(m) == "boolean" then
                error"'use' only works with modules returning table!"
            end
            for k,v in pairs(m) do
                if rawget(_G, k) then
                    print("warning!: use overrides field '"..tostring(k).."' in global scope!")
                    
                end
                if builtin[k] then
                    print("changes for essential global field '"..tostring(k).."' ignored!")
                else
                    rawset(_G, k, v)
                end
            end
        end
    end;
}