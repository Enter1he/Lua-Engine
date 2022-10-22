
local Commands = {

}

B = {
    space = 32;
    exclam = 33;
    hashsign = 34;
    dollar = 36;
    percent = 37;
    ampersand = 38;
    apostrophe = 39;
    parentleft = 40;
    parentright = 41;
    asterisk = 42;
    plus = 43;
    comma = 44;
    minus = 45;
    period = 46;
    slash = 47;
    [0] = 48;
    [1] = 49;
    [2] = 50;
    [3] = 51;
    [4] = 52;
    [5] = 53;
    [6] = 54;
    [7] = 55;
    [8] = 56;
    [9] = 57;
    colon = 58;
    semicolon = 59;
    less = 60;
    equal = 61;
    greater = 62;
    question = 63;
    at = 64;
    A = 65;
    B = 66;
    C = 67;
    D = 68;
    E = 69;
    F = 70;
    G = 71;
    H = 72;
    I = 73;
    J = 74;
    K = 75;
    L = 76;
    M = 77;
    N = 78;
    O = 79;
    P = 80;
    Q = 81;
    R = 82;
    S = 83;
    T = 84;
    U = 85;
    V = 86;
    W = 87;
    X = 88;
    Y = 89;
    Z = 90;
    bracketleft = 91;
    backslash = 92;
    bracketright = 93;
    circum = 94;
    underscore = 95;
    grave = 96;
    a = 97;
    b = 98;
    c = 99;
    d = 100;
    e = 101;
    f = 102;
    g = 103;
    h = 104;
    i = 105;
    j = 106;
    k = 107;
    l = 108;
    m = 109;
    n = 110;
    o = 111;
    p = 112;
    q = 113;
    r = 114;
    s = 115;
    t = 116;
    u = 117;
    v = 118;
    w = 119;
    x = 120;
    y = 121;
    z = 122;
    braceleft = 123;
    bar = 124;
    braceright = 125;
    tilde = 126;
    esc = 65307;
    enter = 13;
    bs = 8;
    insert = 65379;
    delete = 65535;
    tab = 9;
    home = 0xFF50;
    up = 0xFF52;
    pgup = 0xFF55;
    left = 0xFF51;
    middle = 0xFF0B;
    right = 0xFF53;
    End = 0xFF57;
    down = 0xFF54;
    pgdn = 0xFF56;
    pause = 0xFF13;
    print = 0xFF61;
    menu = 0xFF67;
    acute = 0x00B4;
    ccedilla = 0x00E7;
    diaeresis = 0x00A8;
    f1 = 0xFFBE;
    f2 = 0xFFBF;
    f3 = 0xFFC0;
    f4 = 0xFFC1;
    f5 = 0xFFC2;
    f6 = 0xFFC3;
    f7 = 0xFFC4;
    f8 = 0xFFC5;
    f9 = 0xFFC6;
    f10 = 0xFFC7;
    f11 = 0xFFC8;
    f12 = 0xFFC9;
    lshift = 0xFFE1;
    rshift = 0xFFE2;
    lctrl = 0xFFE3;
    rctrl = 0xFFE4;
    lalt = 0xFFE9;
    ralt = 0xFFEA;
    scroll = 0xFF14;
    num = 0xFF7F;
    caps = 0xFFE5;
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

new(B, {
    __newindex = function (a, k)
        error("can't change button codes!!")
    end
})

