require"luagl"
require"imlua"
local im = im
local gl = gl
_G.im = nil
_G.gl = nil
local ft2 = require"Fonting"
Sprite = require"classes.Sprite"


local function InitMain(width, height)
    gl.Viewport(0, 0, width, height)
    
    gl.MatrixMode('PROJECTION')
    gl.LoadIdentity()

    gl.Ortho(0, _en[2].w, 0, _en[2].h, -1,1);

    gl.MatrixMode('MODELVIEW')
    gl.LoadIdentity()
end

local function DrawMain(CurrentScene)
    
    gl.ClearColor(0,0,0,0)
    gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT')
    
    CurrentScene:Draw(gl)
    
    gl.Flush()
    
end

local function MapMain()
    gl.PixelStore(gl.UNPACK_ALIGNMENT, 1)
end



local Drawings = {
    DrawSprite = require"DrawSprite";
    LoadText = 1;
    DrawText = 1;
    LoadTextListed = 1;
    DrawTextListed = 1;
    LoadSprite = 1;
    DrawSpriteAnim = 1;
    LoadSpriteSheet = 1;
    DrawSpriteSheetAnim = 1;
    CopySprite = 1;
}

-----------------------------------------------------------------TEXT--------------------------------------------------

function Drawings:LoadText(font)
    self.font = font or self.font
    self.face = ft2.LoadFont(self.font, self.size)
    self.tex = gl.GenTextures(128)
    
    for i = 0, 127 do
        gl.BindTexture('TEXTURE_2D', self.tex[i+1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','LINEAR')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','LINEAR')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE', 'DECALS')

        gl.TexImage2D(0, gl.RGBA, self.face[i][3], self.face[i][4], 0, gl.LUMINANCE_ALPHA, gl.UNSIGNED_BYTE, self.face[i][0])
        
    end

    ft2.DestroyFace(self.face)
end

local function Letter(self, n, pn, pos)
    local px,py = self.px, self.py
    local face = self.face
    local kernx;
    if pn then
        local width = self.face[pn][1]
        kernx = (width == 0) and self.size or width
    else
        kernx = 0
    end
    
    
    local x, y = px + kernx , self.pos[2]
    local w, h, w2, h2 = self.face[n][1], self.face[n][2], self.face[n][3], self.face[n][4]
    local dx, dy = w/w2, h/h2
    local top = face[n][5]
    
    
    self.px, self.py = x, self.pos[2] + top 
    y = self.py + (pn==10 and self.size or 0)
    gl.PushMatrix()
        gl.Color(self.color[1], self.color[2], self.color[3])
        gl.Translate(x,y,0) 
    
        gl.Enable('TEXTURE_2D')

        gl.Enable('BLEND')
        
        gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
        gl.BindTexture(gl.TEXTURE_2D, self.tex[n+1])
        gl.Begin(gl.QUADS)
            gl.TexCoord(0, dy); gl.Vertex(0, -h)
            gl.TexCoord(dx, dy); gl.Vertex(w, -h)
            gl.TexCoord(dx, 0); gl.Vertex(w, 0)
            gl.TexCoord(0, 0); gl.Vertex(0, 0)
        gl.End()

        gl.Disable('BLEND')

        gl.Disable('TEXTURE_2D') 
        
    
    gl.PopMatrix()
end



function Drawings:DrawText()
    local text, pos = self.value, self.pos
    self.px, self.py = pos[1],pos[2]
    text:gsub("().",
    function(s)
        
        local char, pchar = text:byte(s), (s>0) and text:byte(s-1)
        
        if char then
            
            Letter(self, char, pchar, s)
        end
        return text:sub(s,s)
    end
    )
    
end

function Drawings:LoadTextListed()
    self.font = font or self.font
    self.face = ft2.LoadFont(self.font, self.size)
    self.list = gl.GenLists(128)
    self.tex = gl.GenTextures(128)
    
    for i = 0, 127 do
        local y = self.face[i][5]
        local w, h, w2, h2 = self.face[i][1], self.face[i][2], self.face[i][3], self.face[i][4]
        local dx, dy = w/w2, h/h2

        gl.BindTexture('TEXTURE_2D', self.tex[i+1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','LINEAR')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','LINEAR')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE', 'DECALS')
        
        gl.TexImage2D(0, gl.RGBA, self.face[i][3], self.face[i][4], 0, gl.LUMINANCE_ALPHA, gl.UNSIGNED_BYTE, self.face[i][0])
        
        
        gl.NewList(self.list+i, 'COMPILE')
        gl.PushMatrix()
            gl.Translate(0,y,0)
            
            gl.Begin(gl.QUADS)
                gl.TexCoord(0, dy); gl.Vertex(0, -h)
                gl.TexCoord(dx, dy); gl.Vertex(w, -h)
                gl.TexCoord(dx, 0); gl.Vertex(w, 0)
                gl.TexCoord(0, 0); gl.Vertex(0, 0)
            gl.End()

        gl.PopMatrix()

        gl.EndList()
    end

    ft2.DestroyFace(self.face)
end

function Drawings:DrawTextListed()
    gl.PushMatrix()
    
    gl.Color(self.color[1], self.color[2], self.color[3])
    
    local text, pos = self.value, self.pos
    local px, py = pos[1], pos[2]
    
    local function s(s)
        
        local char, pchar = text:byte(s), (s>1) and text:byte(s-1)
        if pchar then
            px = (pchar==32) and self.face[37][1] or self.face[pchar][1]
            py = (pchar==10 and -self.size or 0)
            if pchar == 10 then
                px = -px
            end
        end
        if char ~= 10 then
            
            gl.Translate(px, py, 0)
            gl.Enable('TEXTURE_2D')

            gl.Enable('BLEND')
            
            gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
            gl.BindTexture(gl.TEXTURE_2D, self.tex[char+1]) 
            gl.CallList(self.list+char)
            
            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
        end
        return text:sub(s,s)
    end
    text:gsub("().", s)
    
    gl.PopMatrix()
end





-------------------------------------------SPRITES-----------------------------------------------------------------

function Drawings:LoadSprite(fold)
    if fold then
        
        local pic, err = im.FileImageLoad(fold)
        self.tex = gl.GenTextures(1)  -- Create The Texture
       
        gl.BindTexture('TEXTURE_2D', self.tex[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'MODULATE')

        if not pic then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pic:GetOpenGLData()
        gl.TexImage2D(0, pic:Depth(), pic:Width(), pic:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        
        self.w = pic:Width()
        self.h = pic:Height()
        pic:Destroy()
    end
end

function Drawings:LoadSpriteSheet(fold)
    if fold then
        if package.loaded[fold] then   
            self.src = package.loaded[fold]
        else 
            self.src = require(fold)
        end
        -- Typical Texture Generation Using Data From file
        
        local pic, err = im.FileImageLoad(fold..self.src.pix)
        self.tex = gl.GenTextures(1)  -- Create The Texture
        self.list = gl.GenLists(self.src.numf or 1)
        
        gl.BindTexture('TEXTURE_2D', self.tex[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'MODULATE')

        if not pic then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pic:GetOpenGLData()
        gl.TexImage2D(0, pic:Depth(), pic:Width(), pic:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        local anim = self.src.anim
        
        for i = 1, #anim do
            local n = i-1
            local a = anim[i]
            local la = 8
            for j = 1, 7 do
                
                gl.NewList(self.list+j+n*la , gl.COMPILE)
                gl.PushMatrix()

                    gl.Scale(self.src.w, self.src.h, 0)
                
                    gl.Enable('TEXTURE_2D'); gl.Enable('BLEND');

                    gl.BlendFunc('SRC_COLOR', 'ONE')
                    gl.BindTexture(gl.TEXTURE_2D,self.tex[1])
                    gl.Begin(gl.QUADS)
                        gl.TexCoord(a[1] + a[3]*(j-1), a[2]); gl.Vertex(0, 0)
                        gl.TexCoord(a[3]*j, a[2]);            gl.Vertex(-1, 0)
                        gl.TexCoord(a[3]*j, a[4]);            gl.Vertex(-1, 1)
                        gl.TexCoord(a[1] + a[3]*(j-1), a[4]); gl.Vertex(0, 1)
                    gl.End()

                    gl.Disable('BLEND'); gl.Disable('TEXTURE_2D'); 
                            
                gl.PopMatrix()
                gl.EndList()
            end
        end
        self.w = self.src.w
        self.h = self.src.h
        pic:Destroy()

    end
end

function Drawings:DrawSpriteSheetAnim()
    
    local pos= self.pos
    local x, y = pos[1] - self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    gl.PushMatrix()
        gl.Color(self.color[1], self.color[2], self.color[3])
        gl.Translate(x,y,0)
        gl.CallList(self.list + self.frame + self.anim )
    gl.PopMatrix()

end

function Drawings:DrawSpriteAnim(anim)

    local pos, idx = self.pos, self.angle
    if self.src == "" then error("NO, Source Image!") end
    local x, y = pos[1] - self.w*self.origin[1], pos[2] + self.h*self.origin[2]
    
    idx = self.src.anim[idx]
    local aframe = anim[self.frame]
    
    gl.PushMatrix()

        gl.Color(self.color[1], self.color[2], self.color[3])
        gl.Translate(x,y,0)
        gl.Scale(self.src.w, self.src.h, 0)
    
            gl.Enable('TEXTURE_2D')
                
            gl.Enable('BLEND')

            gl.BlendFunc('SRC_COLOR', 'ONE')
            gl.BindTexture(gl.TEXTURE_2D,self.tex[1])
            gl.Begin(gl.QUADS)
                gl.TexCoord(idx[1] + idx[3]*(aframe-1), idx[2]); gl.Vertex(0, 0)
                gl.TexCoord(idx[3]*aframe, idx[2]);              gl.Vertex(-1, 0)
                gl.TexCoord(idx[3]*aframe, idx[4]);              gl.Vertex(-1, 1)
                gl.TexCoord(idx[1] + idx[3]*(aframe-1), idx[4]); gl.Vertex(0, 1)
            gl.End()

            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
                
    gl.PopMatrix()

end

function Drawings:CopySprite(s)
    self.tex = s.tex;
    self.color = s.color;
    self.list = s.list;
    self.w, self.h = s.w, s.h;
end

return {
    InitMain = InitMain;
    DrawMain = DrawMain;
    MapMain = MapMain;
    Drawing = Drawings;
}