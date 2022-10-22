require("modules/OOP")

local Color = {
   __add = function(a,b)
      return Color{a[1]+b[1], a[2]+b[2], a[3]+b[3], a[4]+b[4] }
   end,
   __sub = function(a,b)
      return Color{a[1]+b[1], a[2]+b[2], a[3]+b[3], a[4]+b[4] }
   end
};



OOP.makeCallable("Color", Color)

Colors = {
   white = Color{ 1, 1, 1, 1 },
   black = { 0, 0, 0, 1 },
   red = { 1, 0, 0, 1 },
   green = { 0, 1, 0, 1 },
   blue = { 0, 0, 1, 1 },
   yellow = { 1, 1, 0, 1 },
   purple = { 1, 0, 1, 1 },
   none = { 0, 0, 0, 0 },
}



return Color
