
local Commands = {

}



local function ForCommands(key)
    if Commands[key] then
        
        Commands[key]()
    end
end


local function AddCommand(code, func)
    
    Commands[code] = func
end

Controls = {
    name = "Controls";
    Command = ForCommands;
    AddCommand = AddCommand;
}