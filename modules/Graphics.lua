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

    gl.Ortho(0, _en.screen.w, _en.screen.h, 0, 1,0);

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
    
    LoadText = EmptyFunc;
    DrawText = EmptyFunc;
    LoadTextListed = EmptyFunc;
    DrawTextListed = EmptyFunc;
    LoadSprite = EmptyFunc;
    DrawSprite = require"DrawSprite";
    LoadSpriteSheet = EmptyFunc;
    DrawSpriteSheetAnim = EmptyFunc;
    CopySprite = EmptyFunc;
}

-----------------------------------------------------------------TEXT--------------------------------------------------



function Drawings.LoadText(text)
    text.font = font or text.font
    text.face = ft2.LoadFont(text.font, text.size)
    text.list = gl.GenLists(128)
    text.tex = gl.GenTextures(128)
    
    for i = 0, 127 do
        
        local w, h, w2, h2 = text.face[i][1], text.face[i][2], text.face[i][3], text.face[i][4]
        local y = -text.face[i][5]
        local dx, dy = w/w2, h/h2

        gl.BindTexture('TEXTURE_2D', text.tex[i+1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','LINEAR')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','LINEAR')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE', 'DECALS')
        
        gl.TexImage2D(0, gl.RGBA, text.face[i][3], text.face[i][4], 0, gl.LUMINANCE_ALPHA, gl.UNSIGNED_BYTE, text.face[i][0])
        
        
        gl.NewList(text.list+i, 'COMPILE')
        gl.PushMatrix()
            gl.Translate(0,y,0)
            
            gl.Begin(gl.QUADS)
                gl.TexCoord(0, dy); gl.Vertex(0, h)
                gl.TexCoord(dx, dy); gl.Vertex(w, h)
                gl.TexCoord(dx, 0); gl.Vertex(w, 0)
                gl.TexCoord(0, 0); gl.Vertex(0, 0)
            gl.End()

        gl.PopMatrix()

        gl.EndList()
    end

    ft2.DestroyFace(text.face)
end

function Drawings.DrawText(text)
    gl.PushMatrix()
    
    gl.Color(text.color[1], text.color[2], text.color[3])
    
    local value, pos = text.value, text.pos
    local px, py = pos[1], pos[2]
    
    local function s(s)
        
        local char, pchar = value:byte(s), (s>1) and value:byte(s-1)
        if pchar then
            px = (pchar==32) and text.face[37][1] or text.face[pchar][1]
            py = (pchar==10 and text.size or 0)
            if pchar == 10 then
                px = -px
            end
        end
        if char ~= 10 then
            
            gl.Translate(px, py, 0)
            gl.Enable('TEXTURE_2D')

            gl.Enable('BLEND')
            
            gl.BlendFunc('SRC_COLOR', 'ONE_MINUS_SRC_COLOR')
            gl.BindTexture(gl.TEXTURE_2D, text.tex[char+1]) 
            gl.CallList(text.list+char)
            
            gl.Disable('BLEND')

            gl.Disable('TEXTURE_2D') 
        end
        return value:sub(s,s)
    end
    value:gsub("().", s)
    
    gl.PopMatrix()
end





-------------------------------------------SPRITES-----------------------------------------------------------------

function Drawings.LoadSprite(sprite, fold)
    if fold then
        
        local pic, err = im.FileImageLoad(fold)
        sprite.tex = gl.GenTextures(1)  -- Create The Texture
       
        gl.BindTexture('TEXTURE_2D', sprite.tex[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'MODULATE')

        if not pic then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pic:GetOpenGLData()
        gl.TexImage2D(0, 4, pic:Width(), pic:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        
        sprite.w = pic:Width()
        sprite.h = pic:Height()
        pic:Destroy()
    end
end


function Drawings.LoadSpriteSheet(sprite, fold, blend)
    if fold then
        local src;
        if package.loaded[fold] then   
            src = package.loaded[fold]
        else 
            src = require(fold)
        end
        blend = blend or 'none'
        -- Typical Texture Generation Using Data From file
        
        local pic, err = im.FileImageLoad(fold..sprite.src.fmt)
        sprite.tex = gl.GenTextures(1)  -- Create The Texture
        sprite.list = gl.GenLists(sprite.src.numf or 1)
        
        gl.BindTexture('TEXTURE_2D', sprite.tex[1])
        gl.TexParameter('TEXTURE_2D','TEXTURE_MAG_FILTER','NEAREST')
        gl.TexParameter('TEXTURE_2D','TEXTURE_MIN_FILTER','NEAREST')
        gl.TexEnv( 'TEXTURE_ENV',  'TEXTURE_ENV_MODE',  'MODULATE')

        if not pic then
            error(im.ErrorStr(err))
        end

        local gldata, glformat = pic:GetOpenGLData()
        gl.TexImage2D(0, 4, pic:Width(), pic:Height(), 0, glformat, gl.UNSIGNED_BYTE, gldata)
        local anim = src.anim
        sprite.w = src.w
        sprite.h = src.h

        local SRC, ONE = (blend == 'alpha') and 'SRC_ALPHA' or 'SRC_COLOR', (blend == 'alpha') and 'ONE_MINUS_SRC_ALPHA' or 'ONE_MINUS_SRC_COLOR'
        for i = 1, #anim do
            local a = anim[i]
            local la = a[5] or 1
            local n = (i-1)*la-1
            
            for j = 1, la do 
                
                gl.NewList(sprite.list+j+n , gl.COMPILE)
                    if blend ~= 'none' then
                        gl.Enable('TEXTURE_2D'); gl.Enable('BLEND');
                        gl.BlendFunc(SRC, ONE)
                    end
                    gl.BindTexture(gl.TEXTURE_2D, sprite.tex[1])
                    gl.Begin(gl.QUADS)
                        gl.TexCoord(a[1] + a[3]*(j-1), a[2]); gl.Vertex(0, 0)
                        gl.TexCoord(a[3]*j, a[2]);            gl.Vertex(1, 0)
                        gl.TexCoord(a[3]*j, a[4]);            gl.Vertex(1, -1)
                        gl.TexCoord(a[1] + a[3]*(j-1), a[4]); gl.Vertex(0, -1)
                    gl.End()
                    if blend ~= 'none' then
                        gl.Disable('BLEND'); gl.Disable('TEXTURE_2D'); 
                    end
                gl.EndList()
            end
        end
        
        pic:Destroy()

    end
end

-- DRAWSPRITE FUNCTION IS LOADING FROM MODULE on line: 48
local pos = {0,0}

function Drawings.DrawSpriteSheetAnim(sprite)
    
    sprite.pos = sprite.pos or pos
    local x, y, offx, offy = sprite.pos[1], sprite.pos[2], sprite.w*sprite.origin[1], sprite.h*sprite.origin[2]
    
    gl.PushMatrix()
        gl.Color(sprite.color[1], sprite.color[2], sprite.color[3])
        gl.Translate(x,y,0)
        gl.Rotate(-sprite.angle, 0,0,1)
        gl.Translate(offx, -offy,0)
        gl.Scale(sprite.w, sprite.h, 0)
        gl.CallList( sprite.list + sprite.frame )
    gl.PopMatrix()

end


function Drawings.CopySprite(sprite, s)
    sprite.tex = s.tex;
    sprite.color = s.color;
    sprite.list = s.list;
    sprite.w, sprite.h = s.w, s.h;
end

return {
    InitMain = InitMain;
    DrawMain = DrawMain;
    MapMain = MapMain;
    Drawing = Drawings;
}