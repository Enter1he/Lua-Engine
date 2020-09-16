local Buttons = {
}
local Commands = {

}

local function ForKeys( key, down)
    
    for keys in pairs(key) do
        if Buttons[keys] then
            Buttons[keys]()
        end
    end
end


local function ForMouseMotion(player, x,y)
    local px, py = player.pos[1]*8, player.pos[2]*30
    local dx, dy = px-x,  py - y 
    
    player.body.angle = math.deg(math.atan(dy,dx))
end

local function ForCommands(key)
    if Commands[key] then
        Commands[key]()
    end
end

local function AddButton(code, func)
    if code >= iup.K_A and code <= iup.K_Z then
        Buttons[code] = func
        Buttons[code+32] = func
    elseif code >= iup.K_a and code <= iup.K_z then
        Buttons[code] = func
        Buttons[code-32] = func
    else
        Buttons[code] = func
    end
end

local function AddCommand(code, func)
    Commands[code] = func
end


local function ForMouseButons()

end

 
Controls = {
    Button = ForMouseButons;
    Motion = ForMouseMotion;
    Key = ForKeys;
    Command = ForCommands;
    AddCommand = AddCommand;
    AddButton = AddButton;
}