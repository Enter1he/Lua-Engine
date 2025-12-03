print("default scene start")
local default = NewScene{
    name = "default"
}
local text = Text.newText({},"Loading...", 30)
local pos;
local main = Layer.new()

function default:Load()
    print("default load")
    
    
end

function default:Update()
    -- text.value = fps
    -- pos[1] = pos[1] + dt*50
    
end

function default:Draw()
    print("default draw")
    -- main:Draw()
end

print("default end")
return default