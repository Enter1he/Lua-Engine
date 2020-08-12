local dt = _en[1]




local living = {
    over = false;
    to = 1;
    pos = {0,0};
    vel = {0,0};
    angle = 1;
    speed = 1;
    
}

function living:Vel_Move()
    local sdt = self.speed * dt
    
    self.pos[1] = self.pos[1] + self.vel[1] * sdt
    self.pos[2] = self.pos[2] + self.vel[2] * sdt
end

function living:MoveTo( x, y)
    if self.over == false then
        local ox, oy = self.pos[1], self.pos[2]
        local sdt = self.speed * dt
        
        
        if oy > y then self.vel[2] = -1; self.angle = 2;
        elseif oy < y then self.vel[2] = 1; self.angle = 4; end

        if ox > x then self.vel[1] = -1; self.angle = 3
        elseif ox < x then self.vel[1] = 1; self.angle = 1 end
        
        self:Vel_Move()

        local c1 = (self.vel[1] <= 0 and ox + self.vel[1] * sdt <= x) or (self.vel[1] >= 0 and ox + self.vel[1] * sdt >= x)
        local c2 = (self.vel[2] <= 0 and oy + self.vel[2] * sdt <= y) or (self.vel[2] >= 0 and oy + self.vel[2] * sdt >= y)
        
        self.vel[1], self.vel[2] = 0, 0
        self.pos[1] = c1 and x or self.pos[1]
        self.pos[2] = c2 and y or self.pos[2]
        self.over = c1 and c2

        
        return true
    elseif self.over == true then
        
        self.over = false
        return self.over
    end
end

function living:RMoveTo(from, to)
    if self.over == false then
        
        local ox, oy = self.pos[1], self.pos[2]
        if not self.rx then
            self.rx = math.random(from[1], from[2])
            self.ry = math.random(to[1], to[2])
        end
        local x, y = self.rx, self.ry
        local sdt = self.speed * dt
        
        self.vel[1], self.vel[2] = 0, 0



        if oy > y then self.vel[2] = -1; self.angle = 2;
        elseif oy < y then self.vel[2] = 1; self.angle = 4; end

        if ox > x then self.vel[1] = -1; self.angle = 3
        elseif ox < x then self.vel[1] = 1; self.angle = 1 end
        
        self:Vel_Move()

        local c1 = (self.vel[1] <= 0 and ox + self.vel[1] * sdt <= x) or (self.vel[1] >= 0 and ox + self.vel[1] * sdt >= x)
        local c2 = (self.vel[2] <= 0 and oy + self.vel[2] * sdt <= y) or (self.vel[2] >= 0 and oy + self.vel[2] * sdt >= y)
        
        
        self.pos[1] = c1 and x or self.pos[1]
        self.pos[2] = c2 and y or self.pos[2]
        self.over = c1 and c2
        if self.over then
            self.rx = nil
            self.ry = nil
        end
        return true
    elseif self.over == true then
        self.rx = math.random(from[1], from[2])
        self.ry = math.random(to[1], to[2])
        self.over = false
        return self.over
    end
end

function living:Patrol(s_pos, f_pos)
    if self.to == 1 then
        
        self.to = self:MoveTo( s_pos[1], s_pos[2]) and 1 or 2
        
    elseif self.to == 2 then
        
        self.to = self:MoveTo(f_pos[1], f_pos[2]) and 2 or 1
        
    end
end

<<<<<<< Updated upstream:interfaces/moving.lua
local moving = {
    over = true;
    to = 1;
    pos = {0,0};
    vel = {0,0};
    angle = 1;
    speed = 1;
    MoveTo = MoveTo;
    Patrol = Patrol;
    Vel_Movement = Vel_Movement;
    RandomMoveTo = RandomMoveTo;
}



return OOP.class(moving, "moving")
=======
function living:Path(args)
    
    if self.to <= #args then
        local i = self.to
        self.to = self:MoveTo(args[i][1], args[i][2]) and i or (i + 1)
    end
    
end
return OOP.class(living, "living")
>>>>>>> Stashed changes:interfaces/living.lua
