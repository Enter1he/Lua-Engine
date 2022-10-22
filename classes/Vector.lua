
local type = type 
local Vector;

Vector = {
    __add = 
    function(a,b)
        local o = {}
        for i = 1, 4 do
            o[i] = (a[i] or 0) + (b[i] or 0)
        end
        return Vector(o)
    end;
    __sub =
    function(a,b)
        local o = {}
        for i = 1, 4 do
            o[i] = (a[i] or 0) - (b[i] or 0)
        end
        return Vector(o)
    end;
    __mul = 
    function(a,b)
        local ta, tb = type(a), type(b)
        local o;
        if ta == 'number' then
            for i = 1, 4 do
                o[i] = (a[i] or 0)*b
            end
        end
        if tb == 'number' then
            for i = 1, 4 do
                o[i] = (a[i] or 0)*b
            end
        end
        return Vector(o)
    end;
    __unm =
    function(a)
        local o = {}
        for i = 1, #a do
            o[i] = -a[i]
        end
        return Vector(o)
    end;
    __tostring = 
    function(t)
        local res = ""
        for i = 1, #t do
            if t[i] then
                res = res..t[i].." "
            else
                break
            end
        end
        return res
    end;
}


return OOP.makeCallable("Vector",Vector)