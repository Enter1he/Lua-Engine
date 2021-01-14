local default = NewScene{
    name = "default"
}
local text = Text.newText("Loading...", 30)
local pos;
function default:Load()
    text.pos = {30, 40}
    text:Load()
    pos = text.pos
end

function default:Update(dt)
    text.value = fps
    pos[1] = pos[1] + dt*50
    
end

function default:Draw()
    text:Draw()
end

return default