local dt = _en.dt


local Mob = {
    pos = {0,0};
    vel = {0,0};
    over = true;
    to = 1;
    speed = 1;
    sight = 1;
}

function Mob:newMob()

    self.pos = {0,0}
    self.vel = {0,0}
    self.over = true
    self.to = 1
    self.Stop = Mob.Stop
    self.isMoving = Mob.isMoving
    self.Vel_Move = Mob.Vel_Move
    self.MoveTo = Mob.MoveTo

end

function Mob:newPlayer()

    self.pos = {0,0}
    self.vel = {0,0}
    self.speed = 10
    self.Stop = Mob.Stop
    self.isMob = Mob.isMob
    self.Vel_Move = Mob.Vel_Move
    
end

function Mob:Stop()
    self.vel[1] = 0; self.vel[2] = 0;
end

function Mob:Liberate()
    self.vel = {0,0}
end

function Mob:isMoving()
    return self.vel[1] ~= 0 or self.vel[2] ~= 0
end

function Mob:Tile_Move()
    local s = self.speed
    
end

function Mob:Vel_Move()
    local sdt = self.speed * dt
    self.pos[1] = self.pos[1] + self.vel[1] * sdt
    self.pos[2] = self.pos[2] + self.vel[2] * sdt
end

function Mob:MoveTo( x, y)
    if self.over == false then
        local ox, oy = self.pos[1], self.pos[2]
        local sdt = self.speed * dt
        
        
        if oy > y then self.vel[2] = -1; 
        elseif oy < y then self.vel[2] = 1;  end

        if ox > x then self.vel[1] = -1; 
        elseif ox < x then self.vel[1] = 1; end
        
        self:Vel_Move()

        ox, oy = self.pos[1], self.pos[2]
        local c1 = (self.vel[1] <= 0 and ox <= x) or (self.vel[1] >= 0 and ox >= x)
        local c2 = (self.vel[2] <= 0 and oy <= y) or (self.vel[2] >= 0 and oy >= y)
        
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

function Mob:RMoveTo(from, to)
    if self.over == false then
        
        local ox, oy = self.pos[1], self.pos[2]
        if not self.rx then
            self.rx = math.random(from[1], from[2])
            self.ry = math.random(to[1], to[2])
        end
        local x, y = self.rx, self.ry
        local sdt = self.speed * dt
        
        self.vel[1], self.vel[2] = 0, 0



        if oy > y then self.vel[2] = -1; 
        elseif oy < y then self.vel[2] = 1;  end

        if ox > x then self.vel[1] = -1; 
        elseif ox < x then self.vel[1] = 1; end
        
        self:Vel_Move()

        ox, oy = self.pos[1], self.pos[2]
        local c1 = (self.vel[1] <= 0 and ox <= x) or (self.vel[1] >= 0 and ox >= x)
        local c2 = (self.vel[2] <= 0 and oy <= y) or (self.vel[2] >= 0 and oy >= y)
        
        
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

function Mob:Patrol(s_pos, f_pos)
    if self.to == 1 then
        
        self.to = self:MoveTo( s_pos[1], s_pos[2]) and 1 or 2
        
    elseif self.to == 2 then
        
        self.to = self:MoveTo(f_pos[1], f_pos[2]) and 2 or 1
        
    end
end

function Mob:RandomSearch(from, to)
    if not self.over then
        
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
        ox, oy = self.pos[1], self.pos[2]
        local c1 = (self.vel[1] <= 0 and ox <= x) or (self.vel[1] >= 0 and ox >= x)
        local c2 = (self.vel[2] <= 0 and oy <= y) or (self.vel[2] >= 0 and oy >= y)
        
        
        self.pos[1] = c1 and x or self.pos[1]
        self.pos[2] = c2 and y or self.pos[2]
        self.over = c1 and c2
        return true
    else
        if self.c.length > 0 then
            local n = self.to
            self.rx = self.c[n].pos[1]
            self.ry = self.c[n].pos[2]
            self.c.length = self.c.length - 1
        else
            self.rx = math.random(from[1], from[2])
            self.ry = math.random(to[1], to[2])
            
        end
        self.over = false

        return false
    end
end

function Mob:Path(args)
    
    if self.to <= #args then
        local i = self.to
        self.to = self:MoveTo(args[i][1], args[i][2]) and i or (i + 1)
    end
    
end


OOP.inherit(Mob, Sprite)



return OOP.class('Mob', Mob)