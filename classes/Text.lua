



local Text = {
    value = 'Some text.';
    font = Fonts.."arial.ttf";
    size = 12;
    pos = {0,0};
    color = {1,1,1,1};
    Load = Graphics.LoadText;
    Draw = Graphics.DrawText;
}

function Text.newText(value, size, font)
    local text = {
        value = value or 'Some text.';
        font = font or Fonts.."arial.ttf";
        size = size or 12;
        pos = {0,0};
        color = {1,1,1,1};
        Load = Graphics.LoadText;
        Draw = Graphics.DrawText;
    }
    return text
end


return OOP.class('Text', Text)