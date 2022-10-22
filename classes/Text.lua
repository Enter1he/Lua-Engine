



local Text = {
    value = 'Some text.';
    font = Fonts.."arial.ttf";
    size = 12;
    pos = {0,0};
    color = {1,1,1,1};
    Load = Graphics.LoadText;
    Draw = Graphics.DrawText;
}

function Text.newText(t, value, size, font)
    t = t or {}
    
    t.value = t.value or Text.value;
    t.font = t.font or Text.font;
    t.size = t.size or 12;
    t.pos = t.pos or {0,0};
    t.color = t.color or {1,1,1,1};
    t.alpha = t.alpha or nil
    t.Load = Text.Load;
    t.Draw = Text.Draw;
    return t
end


return OOP.class('Text', Text)