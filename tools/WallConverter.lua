--WallConverter
require"OOP"
local angle = 0
local w, h = 1,1
local pipe = OOP.const{
    hor = string.char(205);
    hordown = string.char(203);
    horup = string.char(202);
    ver = string.char(186);
    verleft = string.char(185);
    verright = string.char(204);
    leftdown = string.char(187);
    leftup = string.char(188);
    rightdown = string.char(201);
    rightup = string.char(200);
    four = string.char(206);
}
local wallsign, image = io.read(), io.read() 
print(wallsign, image)
local file = io.open(image,"r")
local pix = file:read("*a")
file:close()
_, h = pix:gsub("%c","")
h = h + 1
w = _:len()//h
print(w,h)
pix = pix:gsub("()"..wallsign.."()", function(s)
    local y = s//w
    local searchpath = "["..wallsign..","..pipe.hor..","..pipe.ver..","..pipe.four..","..pipe.rightdown..","..pipe.rightup..","..pipe.leftdown..","..pipe.leftup.."]"
    local sides = {
        (pix:sub(s-1,s-1):match(searchpath) and s-1 > y*w)and 1 or 0;--1
        (pix:sub(s+1,s+1):match(searchpath) and s-1 < (y+1)*w)and 1 or 0;--2
        (pix:sub(s-w-1,s-w-1):match(searchpath) and s-w-1 > 0)and 1 or 0;--3
        (pix:sub(s+w+1,s+w+1):match(searchpath) and s+w+1 < pix:len())and 1 or 0--4
    }
    if sides[1] == 1 and sides[3] == 1 and sides[2] == 1 and sides[4] == 1  then
        return pipe.four
    elseif sides[1] == 0 and sides[3] == 0 and sides[2] == 0 and sides[4] == 0  then
        return pipe.four
    elseif sides[1] == 1 and sides[4] == 1 and sides[2] == 1 then
        return pipe.hordown
    elseif sides[3] == 1 and sides[1] == 1 and sides[4] == 1 then
        return pipe.verleft
    elseif sides[3] == 1 and sides[2] == 1 and sides[4] == 1 then
        return pipe.verright
    elseif sides[1] == 1 and sides[3] == 1 and sides[2] == 1 then
        return pipe.horup
    elseif sides[1] == 1 and sides[3] == 1 then
        return pipe.leftup
    elseif sides[1] == 1 and sides[4] == 1 then
        return pipe.leftdown
    elseif sides[2] == 1 and sides[3] == 1 then
        return pipe.rightup
    elseif sides[2] == 1 and sides[4] == 1 then
        return pipe.rightdown
    elseif sides[1] == 1 or sides[2] == 1 then
        return pipe.hor
    elseif sides[3] == 1 or sides[4] == 1 then
        return pipe.ver
    end
end)
file = io.open(image,"w")
file:write(pix)
file:close()
print(pic)