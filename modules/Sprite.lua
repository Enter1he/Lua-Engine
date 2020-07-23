
local im = require"imlua"
local gl = gl

local function Load(self, fold)
    if fold then
        if package.loaded[fold] then   
            self.src = package.loaded[fold]
        else 
            self.src = require(fold)
            self.fold = fold
        end
        -- Typical Texture Generation Using Data From file
        local err;
        self.pic, err = im.FileImageLoad(self.src.pix)
        self.pix = gl.GenTextures(1)  -- Create The Texture

        
        gl.BindTexture('TEXTURE_2D', self.pix[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'MODULATE')

        if not pix then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pix:GetOpenGLData()
        gl.PixelStore(gl.UNPACK_ALIGNMENT, 1)
        
        gl.TexImage2D(0, pix:Depth(), pix:Width(), pix:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        
        self.w = self.src.w
        self.h = self.src.h
        
        
    else
        local pix , err = im.ImageCreate(self.w, self.h, im.BYTE, im.RGB)
        self.pix = pix
        self.src = 'Raw'
    end
    -- gldata will be destroyed when the image object is destroyed
end

local function Anim(self, fnum)
    if self.src == "" then error("NO, Source Image!") end
    self.anim = fnum
end

local function Draw (self, actor)
    if self.src == "" then error("NO, Source image!") end
    local x, y = actor.pos[1] - self.w*self.origin[1], actor.pos[2] - self.h*self.origin[2]
    
    
    gl.PushMatrix()

        gl.Rotate(0,0, angle)
        gl.Translate(x,y,0) 
        gl.Scale(self.w, self.h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')

            gl.Enable('BLEND')
            
            gl.BindTexture(gl.TEXTURE_2D, self.pix[1])
            gl.Begin(gl.QUADS)
                gl.TexCoord(0, 1); gl.Vertex(0, 0)
                gl.TexCoord(1, 1); gl.Vertex(1, 0)
                gl.TexCoord(1, 0); gl.Vertex(1, -1)
                gl.TexCoord(0, 0); gl.Vertex(0, -1)
            gl.End()

            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 

        
        gl.PopMatrix()

    gl.PopMatrix()

end

local function DrawSheet(self, pos, idx)
    local gl = gl
    _ENV = self
    if src == "" then error("NO, Source Image!") end
    local x, y = pos[1] - w*origin[1], pos[2] + h*origin[2]
    
    idx = src.anim[idx]
    
    gl.PushMatrix()

    
        gl.Translate(x,y,0) 
        gl.Scale(w, h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')
                
            gl.Enable('BLEND')

            gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
            gl.BindTexture(gl.TEXTURE_2D,pix[1])
            gl.Begin(gl.QUADS)
                gl.TexCoord(idx[3], idx[2]); gl.Vertex(0, 0)
                gl.TexCoord(idx[1], idx[2]); gl.Vertex(1, 0)
                gl.TexCoord(idx[1], idx[4]); gl.Vertex(1, -1)
                gl.TexCoord(idx[3], idx[4]); gl.Vertex(0, -1)
            gl.End()

            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
                
        gl.PopMatrix()

    gl.PopMatrix()

end

local function Delete(self)
    if package.loaded[self.fold] then
        package.loaded[self.fold] = nil   
        self.src = nil
    end
    self.pic:Destroy()
end

local Sprite = {
    pos = {0,0};
    angle = 1;
    w = 0;
    h = 0;
    src = "";
    origin = {0.5,0.5};
    anim = 0;
    
    Load = Load;
    Anim = Anim;
    Draw = Draw;
    DrawSheet = DrawSheet;
    Delete = Delete;
}


return OOP.class(Sprite, "Sprite")
