Stoxy = function(s)
    local i = s:find("x")
    return tonumber(s:sub(1,i-1)), tonumber(s:sub(i+1))
end
return Stoxy