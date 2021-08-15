local Color; 
Color = {
    __add = 
    function(a,b)
        local o = {}
        for i = 1, 4 do
            o[i] = (a[i] or 0) + (b[i] or 0)
            if o[i] > 1 then 
                o[i] = 1
            elseif o[i] < 0 then
                o[i] = 0
            end
            
        end
        
        return Color(o)
    end;
    __sub = 
    function(a,b)
        local o = {}
        for i = 1, 4 do
            o[i] = (a[i] or 0) - (b[i] or 0)
            
        end
        
        return Color(o)
    end;
    __name = "Color";
}
return OOP.makeCallable(Color.__name, Color)