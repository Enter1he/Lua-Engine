
local im = require"imlua"
local function Load(self, fold)
    if fold then
        self.src = require(fold)
        package.loaded[fold] = nil
        -- Typical Texture Generation Using Data From file
        local pix, err = im.FileImageLoad(self.src.pix)
        self.pix = gl.GenTextures(2)  -- Create The Texture

        
        gl.BindTexture('TEXTURE_2D', self.pix[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'DECAL')

        if not pix then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pix:GetOpenGLData()
        gl.PixelStore(gl.UNPACK_ALIGNMENT, 1)
        
        gl.TexImage2D(0, pix:Depth(), pix:Width(), pix:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        
        self.w = self.src.w
        self.h = self.src.h
        
        pix:Destroy()
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

local function Draw (self, pos, angle)
    if self.src == "" then error("NO, Source Image!") end
    local x, y = pos[1] + self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    local TexPos = self.src.TPos[angle]
    gl.PushMatrix()

        gl.Rotate(0,0, angle)
        gl.Translate(x,y,0) 
        gl.Scale(self.w, self.h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')
            
            gl.BindTexture(gl.TEXTURE_2D, self.pix[1])

                gl.Begin(gl.QUADS)
                    gl.TexCoord(0, 1); gl.Vertex(0, 0)
                    gl.TexCoord(1, 1); gl.Vertex(1, 0)
                    gl.TexCoord(1, 0); gl.Vertex(1, -1)
                    gl.TexCoord(0, 0); gl.Vertex(0, -1)
                gl.End()
            
            gl.Disable('TEXTURE_2D') 

        
        gl.PopMatrix()

    gl.PopMatrix()

end

local function DrawSheet(self, pos, TexPos)
    if self.src == "" then error("NO, Source Image!") end
    local x, y = pos[1] + self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    TexPos = self.src.TPos[TexPos]
    
    gl.PushMatrix()

    
        gl.Translate(x,y,0) 
        gl.Scale(self.w, self.h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')
            
            gl.BindTexture(gl.TEXTURE_2D,self.pix[1])

                gl.Begin(gl.QUADS)
                    gl.TexCoord(TexPos[3], TexPos[2]); gl.Vertex(0, 0)
                    gl.TexCoord(TexPos[1], TexPos[2]); gl.Vertex(1, 0)
                    gl.TexCoord(TexPos[1], TexPos[4]); gl.Vertex(1, -1)
                    gl.TexCoord(TexPos[3], TexPos[4]); gl.Vertex(0, -1)
                gl.End()
            
            gl.Disable('TEXTURE_2D') 

        
        gl.PopMatrix()

    gl.PopMatrix()

end

local Sprite = {
    w = 0;
    h = 0;
    src = "";
    origin = {0.5,0.5};
    anim = 0;
    Load = Load;
    Anim = Anim;
    Draw = Draw;
    DrawSheet = DrawSheet;
}


return OOP.class(Sprite, "Sprite")
