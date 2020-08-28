
local im = require"imlua"



local Sprite = {
    
    origin = {0.5,0.5};
    anim = 0;
    color = {1,1,1,1};
}

function Sprite:Load( fold)
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

        if not self.pic then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = self.pic:GetOpenGLData()
        
        
        gl.TexImage2D(0, self.pic:Depth(), self.pic:Width(), self.pic:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        
        self.w = self.src.w
        self.h = self.src.h
        
        
    else
        local pix , err = im.ImageCreate(self.w, self.h, im.BYTE, im.RGB)
        self.pix = pix
        self.src = 'Raw'
    end
    -- gldata will be destroyed when the image object is destroyed
end



function Sprite:Draw ()
    if not self.src  then error("NO, Source image!") end
    local x, y = self.pos[1] - self.w*self.origin[1], self.pos[2] - self.h*self.origin[2]
    
    
    gl.PushMatrix()
        gl.Color(self.color[1], self.color[2], self.color[3])
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

function Sprite:DrawSheet( par)
    local pos, idx = par.pos, par.angle
    if self.src == "" then error("NO, Source Image!") end
    local x, y = pos[1] - self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    idx = self.src.anim[idx]
    
    gl.PushMatrix()

        gl.Color(self.color[1], self.color[2], self.color[3])
        gl.Translate(x,y,0) 
        gl.Scale(self.w, self.h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')
                
            gl.Enable('BLEND')

            gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
            gl.BindTexture(gl.TEXTURE_2D,self.pix[1])
            gl.Begin(gl.QUADS)
                gl.TexCoord(idx[1], idx[2]); gl.Vertex(0, 0)
                gl.TexCoord(idx[3], idx[2]); gl.Vertex(-1, 0)
                gl.TexCoord(idx[3], idx[4]); gl.Vertex(-1, 1)
                gl.TexCoord(idx[1], idx[4]); gl.Vertex(0, 1)
            gl.End()

            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
                
        gl.PopMatrix()

    gl.PopMatrix()

end

function Sprite:DrawAnim(par, anim)

    
    local pos, idx = par.pos, par.angle
    if self.src == "" then error("NO, Source Image!") end
    local x, y = pos[1] - self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    idx = self.src.anim[idx]
    local aframe = anim[par.frame]
    gl.PushMatrix()

        gl.Color(self.color[1], self.color[2], self.color[3])
        gl.Translate(x,y,0) 
        gl.Scale(self.src.w, self.src.h, 0)
        gl.PushMatrix()
    
            gl.Enable('TEXTURE_2D')
                
            gl.Enable('BLEND')

            gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
            gl.BindTexture(gl.TEXTURE_2D,self.pix[1])
            gl.Begin(gl.QUADS)
                gl.TexCoord(idx[1] + idx[3]*(aframe-1), idx[2]); gl.Vertex(0, 0)
                gl.TexCoord(idx[3]*aframe, idx[2]);              gl.Vertex(-1, 0)
                gl.TexCoord(idx[3]*aframe, idx[4]);              gl.Vertex(-1, 1)
                gl.TexCoord(idx[1] + idx[3]*(aframe-1), idx[4]); gl.Vertex(0, 1)
            gl.End()

            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
                
        gl.PopMatrix()

    gl.PopMatrix()

end

function Sprite:Delete()
    if package.loaded[self.fold] then
        package.loaded[self.fold] = nil   
        self.src = nil
    end
    self.pic:Destroy()
end




return OOP.class(Sprite, "Sprite")
