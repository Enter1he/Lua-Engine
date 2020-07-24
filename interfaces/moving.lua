
local function Vel_Movement(self)
    local sdt = self.speed * _en.dt
    self.pos[1] = self.pos[1] + self.vel[1] * sdt
    self.pos[2] = self.pos[2] + self.vel[2] * sdt
end

local function MoveTo(self, x, y)
    if self.over == false then
        local ox, oy = self.pos[1], self.pos[2]
        local sdt = self.speed * _en.dt
        
        self.vel[1], self.vel[2] = 0, 0



        if oy > y then self.vel[2] = -1; self.angle = 2;
        elseif oy < y then self.vel[2] = 1; self.angle = 4; end

        if ox > x then self.vel[1] = -1; self.angle = 3
        elseif ox < x then self.vel[1] = 1; self.angle = 1 end
        
        self:Vel_Movement()

        local c1 = (self.vel[1] <= 0 and ox + self.vel[1] * sdt <= x) or (self.vel[1] >= 0 and ox + self.vel[1] * sdt >= x)
        local c2 = (self.vel[2] <= 0 and oy + self.vel[2] * sdt <= y) or (self.vel[2] >= 0 and oy + self.vel[2] * sdt >= y)
        
        
        self.pos[1] = c1 and x or self.pos[1]
        self.pos[2] = c2 and y or self.pos[2]
        self.over = c1 and c2

        
        return true
    elseif self.over == true then
        
        self.over = false
        return self.over
    end
end

local function RandomMoveTo(self, from, to)
    if self.over == false then
        
        local ox, oy = self.pos[1], self.pos[2]
        local x, y = self.rx, self.ry
        local sdt = self.speed * _en.dt
        
        self.vel[1], self.vel[2] = 0, 0



        if oy > y then self.vel[2] = -1; self.angle = 2;
        elseif oy < y then self.vel[2] = 1; self.angle = 4; end

        if ox > x then self.vel[1] = -1; self.angle = 3
        elseif ox < x then self.vel[1] = 1; self.angle = 1 end
        
        self:Vel_Movement()

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
        self.rx = math.random(from, to)
        self.ry = math.random(from, to)
        self.over = false
        return self.over
    end
end

local function Patrol(self, s_pos, f_pos)
    if self.to == 1 then
        
        self.to = self:MoveTo( s_pos[1], s_pos[2]) and 1 or 2
        
    elseif self.to == 2 then
        
        self.to = self:MoveTo(f_pos[1], f_pos[2]) and 2 or 1
        
    end
end

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