local Color; 
Color = {
    __add = 
    function(a,b)
        local o = {}
        for i = 1, 4 do
            local n = (a[i] or 0) + (b[i] or 0)
            if n > 1 then 
                n = 1
            elseif n < 0 then
                n = 0
            end
            o[i] = n
        end
        
        return Color(o)
    end;
    __sub = 
    function(a,b)
        local o = {}
        for i = 1, 4 do
            local n = (a[i] or 0) - (b[i] or 0)
            if n > 1 then 
                n = 1
            elseif n < 0 then
                n = 0
            end
            o[i] = n
        end
        
        return Color(o)
    end;
    __name = "Color";
}
return OOP.makeCallable(Color.__name, Color)