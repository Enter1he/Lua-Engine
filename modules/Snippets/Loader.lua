loader ={}

loader.Load = function(map)
    local file = io.open(map,"r")
    local field = file:read("*a")
    file:close()
    local _, ch = field:gsub("%c","")
    ch = ch + 1
    local cw = _:len()//ch
    return {field, cw, ch}
end


return loader