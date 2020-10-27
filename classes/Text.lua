



local Text = {
    value = 'Some text.';
    font = Fonts.."arial.ttf";
    size = 12;
    pos = {0,0};
    color = {1,1,1,1};
    LoadList = Graphics.LoadTextListed;
}



return OOP.class('Text', Text)