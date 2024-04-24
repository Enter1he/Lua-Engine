-- Pos.t


struct Pos{
    x: double;
    y: double;
};

terra Pos:tostring()
    return tostring(self.x).." "..tostring( self.y);
end

return Pos