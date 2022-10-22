local Ring = NewScene{name = "Ring"}

local In = {
    upn = B.w;
    upr = B.w + B.d;
    upl = B.w + B.a;
    rgt = B.d;
    lft = B.a;
    dwn = B.s;
    dwr = B.s + B.d;
    dwl = B.s + B.a;
    lpn = B[4];
    mpn = B[5];
    hpn = B[6];
    lkc = B[1];
    mkc = B[2];
    hkc = B[3];
}

local Bprior = {
    "hkc", "hpn", "mkc",
    "mpn", "lkc", "lpn", 

    "dwn", "upn", "rgt", "lft"
}

Controls.AddCommand(B.esc, LE.Close)
local left = string.char(27)
local right = string.char(26)
local up = string.char(24)
local upl = up..left
local upr = up..right
local dwn = string.char(25)
local dwr = up..right
local dwl = up..left


local Buff = {}
do
    local sBuff = Buff
    local prev;
    for i = 1, 12 do
        prev = Buff
        Buff[1] = ""
        Buff.next = {}
        Buff = Buff.next
        Buff.prev = prev
    end
    prev = Buff
    Buff[1] = ""
    Buff.next = sBuff
    Buff = sBuff
    Buff.prev = prev
end

local function PrintBuffer(buff)
    
    print"----------------Buffer_Start---------------------"
    print("i", "dir", "btn", "address")
    local sbuff,i = buff, 0
    repeat 
        print(i, buff[1], buff)
        buff = buff.prev
        i = i + 1
    until buff == sbuff
end

local function ClearBuffer(buff)
    
    local sbuff = buff
    repeat
        buff[1] = ""
        buff = buff.prev
    until buff == sbuff
end

local function AddInput(buff, code)
    buff[1] = code
    buff = buff.next
end
local function NextBuff(buff)
    
    buff = buff.next
    buff[1] = false
end
local plus = "+"
local buffertime, bufferlim = 0, 30;
function Ring:KeyPress(k,down)
    
    local seq = "";
    local key = "";
    local inpt = "";
    local i;
    for i = 1, 6 do
        key = Bprior[i]
        inpt = In[key]
        if k[inpt] then
            seq = seq..key
            down = true
        end
    end
    key = ""
    if k[In.upn] and k[In.lft] then
        key = "upl"
        down = true
    elseif k[In.upn] and k[In.rgt] then
        key = "upr"
        down = true
    elseif k[In.dwn] and k[In.lft] then
        key = "dwl"
        down = true
    elseif k[In.dwn] and k[In.rgt] then
        key = "dwr"
        down = true
    end
    if key == "" then
        for i = 7, 10 do
            key = Bprior[i]
            inpt = In[key]
            if k[inpt] then
                down = true
                break
            end
           key = ""
        end 
    end
    if down then
        
        inpt = key..(seq~="" and (key~="" and "+"..seq or seq) or "" )
        
        if Buff[1] ~= inpt  then
            
            
            
            
            print(inpt)
            
            Buff = (Buff[1]~="")and Buff.next or Buff
            Buff[1] = ""
            
            Buff[1] = inpt
            PrintBuffer(Buff)
        end
        
        buffertime = 0
    else
        
        buffertime = (buffertime < bufferlim) and (buffertime + 1) or buffertime
    end
    if buffertime == bufferlim then
        ClearBuffer(Buff)
    end
    
end




function Ring:KeyRelease(c)
    
    for i = 1, 10 do
        local key = Bprior[i]
        local inpt = In[key]
        if c == inpt then
            
            -- Buff[1] = inpt
            
            Buff = Buff.next
            Buff[1] = ""
        end
    end
    
end

function Ring:Load()
    PrintBuffer(Buff)
end

function Ring:Update(dt)
end

return Ring