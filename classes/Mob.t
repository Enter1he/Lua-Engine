-- Mob.t
import "macro"
local Pos = require"classes.Pos"
local select = terralib.select

struct Mob{
    pos : &Pos;
    vel : &Pos;
    speed : double;
    sight : double;
    to : int;
    over : bool;
};

macro Mob.Vel_Move(m, dt)
    return quote 
        do
            var m : &Mob = m
            var dt : double = dt
            m.pos.x = m.pos.x + m.vel.x*dt
            m.pos.y = m.pos.y + m.vel.y*dt
        end
    end
end

macro Mob.MoveTo(m, dt, x, y)
    return quote 
        do
            var m = [&Mob](m)
            var x, y = [double](x), [double](y)
            if not m.over then
                var ox, oy = m.pos.x, m.pos.y
                if oy > y then m.vel.y = -1; 
                elseif oy < y then m.vel.y = 1;  end

                if ox > x then m.vel.x = -1; 
                elseif ox < x then m.vel.x = 1; end
            
                m:Vel_Move(dt)
                
                ox, oy = m.pos.x, m.pos.y
                local c1 = (ox >= x - 1 and ox <= x + 1)
                local c2 = (oy >= y - 1 and oy <= y + 1)
                
                
                pos[1] = terralib.select(c1, x, ox)
                pos[2] = terralib.select(c2, y, oy)
                m.over = c1 and c2
            else
                m.over = false
            end
        end
    end
end

terra luaopen_Mob(L : &lua_State) : int
    
end