local default = NewScene{
    name = "default"
}
local text = Text.newText({},"Loading...", 30)
local pos;
local main = Layer.new()

function default:Load()
    text.pos = {30, 40}
    text:Load()
    pos = text.pos
    main:AddDrawable(text)
end

function default:Update()
    text.value = fps
    pos[1] = pos[1] + dt*50
    
end

function default:Draw()
    main:Draw()
end

return default