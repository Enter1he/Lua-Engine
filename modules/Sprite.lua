local TPos;
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
        
        
        
        self.w = pix:Width()
        self.h = pix:Height()
        TPos = {
            {36/self.w, 54/self.h, 0, 0};
            {92/self.w, 1, 46/self.w, 0.5};
            {1, 54/self.h, 90/self.w, 0};
            {92/self.w, 0.5, 45/self.w, 0};
        }
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

local function Draw (self, par)
    if self.src == "" then error("NO, Source Image!") end
    local x, y = par.pos[1]+ 1/4, par.pos[2] + 1/2
    local w, h = self.w, self.h
    
    local TexPos = TPos[par.angle]
    gl.PushMatrix()

        gl.Scale(36,54,0)
        gl.Translate(x,y,0) 

        gl.PushMatrix()
    
            gl.Enable('BLEND')
                gl.Enable('TEXTURE_2D')

                gl.BlendFunc('SRC_ALPHA','ONE_MINUS_SRC_COLOR')
                gl.BindTexture(gl.TEXTURE_2D,self.pix[1])

                    gl.Begin(gl.QUADS)
                        gl.TexCoord(TexPos[3], TexPos[2]); gl.Vertex(0, 0)
                        gl.TexCoord(TexPos[1], TexPos[2]); gl.Vertex(2, 0)
                        gl.TexCoord(TexPos[1], TexPos[4]); gl.Vertex(2, -2)
                        gl.TexCoord(TexPos[3], TexPos[4]); gl.Vertex(0, -2)
                    gl.End()
                
                gl.Disable('TEXTURE_2D') 

            gl.Disable('BLEND')
        
        gl.PopMatrix()

    gl.PopMatrix()

end

local Sprite = {
    w = 0;
    h = 0;
    src = "";
    anim = 0;
    Load = Load;
    Anim = Anim;
    Draw = Draw;
}

Sprite.__index = Sprite

return Sprite