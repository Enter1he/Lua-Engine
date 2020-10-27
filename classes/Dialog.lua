

local Dialog = {
    CharFace = false; -- use Sprite for this
    Text = false; -- use Text here
    Back  = false; -- use Sprite for background image
    phrase = 1;
    branch = 1;
    exit = true;
    Controls = 
        function(it, key, down)
            if key == iup.K_CR then 
                it.exit = true
            end
            if key == iup.K_E or key == iup.K_e then
                it.phrase = it.phrase + 1
            end
        end;
    Update = 
        function(it)
            if not it.exit then
                local 
                a = it.CharFace and it.CharFace()
                a = it.Text and it.Text()
                a = it.Back and it.Back()
            end
        end;
}

return OOP.class('Dialog', Dialog)