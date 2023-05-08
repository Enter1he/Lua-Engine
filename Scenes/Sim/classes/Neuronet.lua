local Neuron;
local Synapse;
--activation functions' prototypes
local sigmoid, square, linear, tanh, gaussian, ReLU;

-- loading math
local random = math.random;
local sqrt =  math.sqrt;
local cos = math.cos;
local log = math.log;
local exp = math.exp;
local pi = math.pi;
local abs = math.abs;
local rdm = require"Scenes.Sim.lib.ranDOOM"

local function array(a)
    local i = 1
    return function()
        return a[i]
    end
end

local rand_100 = rdm.makeRand(rdm.genTable(1,100), math.random(1,100))


local ranT = rdm.genTableNV{[1] = 16, [2] = 17, [3] = 17, [4] = 17, [5] = 16, [6] = 17}
rdm.ShuffleTable(ranT, rand_100)
local choose_6 = rdm.makeRand(ranT, rand_100())

local p4_t = rdm.genTableNV{[1] = 25, [2] = 25, [3] = 25, [4] = 25}
rdm.ShuffleTable(p4_t, rand_100)
local choose_4 = rdm.makeRand(p4_t, rand_100())

local p10_abs = rdm.genTableNV{
    [-10] = 9, [-9] = 10, [-8] = 9, [-7] = 10, [-6] = 10, [-5] = 9, [-4] = 10, [-3] = 10, [-2] = 10, [-1] = 10,
    [10] = 9,   [9] = 9,   [8] = 10, [7] = 9,   [6] = 10,  [5] = 10, [4] = 10,  [3] = 9,   [2] = 9,   [1] = 9, [0] = 9}
rdm.print(p10_abs)
local rand_10abs = rdm.makeRand(p10_abs, rand_100())

local p4_abs = rdm.genTableNV{
    [-4] = 11, [-3] = 11, [-2] = 11, [-1] = 11,
    [4] = 11,  [3] = 11,   [2] = 11,   [1] = 11, [0] = 12,
}
local rand_4abs = rdm.makeRand(p4_abs, rand_100())

local possib_3 = {[1] = 33, [2] = 33, [3] = 34}
local p3_t = rdm.genTableNV(possib_3)
rdm.ShuffleTable(p3_t, rand_100)
local choose_3 = rdm.makeRand(p3_t, rand_100())

local possib_l = {inp = 33, out = 33, hid = 34}
local l_t = rdm.genTableNV(possib_l)
rdm.ShuffleTable(l_t, rand_100)
local rand_l = rdm.makeRand(l_t, choose_3())

local memChoice;
local memChoiceS;
local memChoiceStr;

local listn, locan = function() end, function() end -- name of object in the net's list and name of object in some local lists 
local layn = function() end

math.randomseed(os.time())

local Net = {
    list = {};
    inp = {};
    out = {};
    hid = {};
    syns = {};
}

function Net.addInput(N, n)
    n[listn] = #N.list+1
    n[locan] = #N.inp+1
    n[layn] = "inp"
    N.inp[n[locan]] = n
    N.list[n[listn]] = n
end

function Net.addOutput(N, n)
    n[listn] = #N.list+1
    n[locan] = #N.out+1
    n[layn] = "out"
    N.out[n[locan]] = n
    N.list[n[listn]] = n
end

function Net.addHidden(N, n)
    n[listn] = #N.list+1
    n[locan] = #N.hid+1
    n[layn] = "hid"
    N.hid[n[locan]] = n
    N.list[n[listn]] = n
end

function Net.addNeuron(N, n)
    n = n or Neuron.new(Neuron.funcs[choose_6()])
    local choice = memChoice(#N.syns)
    local s1 = N.syns[choice]
    local n2 = s1.neur[2]
    s1:connect(n, 2)
    local s2 = Synapse.new()
    s2:connect(n, 1)
    s2:connect(n2, 2)
    N:addHidden(n)
    s2[listn] = #N.list+1
    s2[locan] = #N.syns+1
    N.syns[s2[locan]] = s2
    N.list[s2[listn]] = s2
end


function Net.new(ins, hids, outs)
    ins = ins or 1
    hids = hids or 0
    outs = outs or 1
    local N = {
        list = {};
        inp = {};
        out = {};
        hid = {};
        syns = {};
        addInput = Net.addInput;
        addOutput = Net.addOutput;
        addHidden = Net.addHidden;
        addSynapseR = Net.addSynapseR;
        addNeuron = Net.addNeuron;
        print = Net.print;
    }
    for i = 1, ins do
        N:addInput(Neuron.new(sigmoid))
    end

    for i = 1, outs do
        N:addOutput(Neuron.new(sigmoid));
    end

    for i = 1, hids do
        N:addHidden(Neuron.new(sigmoid))
    end
    return N;
end

local _traversable = {"inp", "hid", "out"}
function Net.traverse(N)
    local type = 0
    return function ()
        type = type + 1
        return N[_traversable[type]]
    end
end



local _connectable = {"hid", "out"}
function Net.addSynapseR(N, s, l)
    if not s then
        s = Synapse.new()
        s[listn] = #N.list+1
        s[locan] = #N.syns+1
        N.list[s[listn]] = s
        N.syns[s[locan]] = s
    end
    s.weight = rand_4abs() + random();
    s:connect(l, 1)
    local chosen = memChoiceStr(_connectable, "hid") -- choosing between hidden or output layer
    if #N.hid == 0 then
        chosen = "out"
    end
    local layer = N[chosen]
    local choice = layer[memChoice(2)] -- choosing from neurons of chosen layer
    s:connect(choice, 2)
    s[listn] = #N.list+1
    s[locan] = #N.syns+1
    N.list[s[listn]] = s
    N.syns[s[locan]] = s
end

function Net.addSynapse(N, n1, n2)
    local s = Synapse.new()
    s[listn] = #N.list+1
    s[locan] = #N.syns+1
    N.list[s[listn]] = s
    N.syns[s[locan]] = s
    s:connect(n1, 1)
    s:connect(n2, 2)
end

function Net.randomSynapses(N)
    for layer in Net.traverse(N) do
        if layer == N.out then
            break;
        end
        for i = 1, #layer do
            local n = layer[i]
            N:addSynapseR(nil,n)
        end    
    end
end

local laynames = {"inp", "out", "hid"}

function chooseNeur(N, ign)
    local choice = memChoiceStr(laynames, ign or "out")
    local layer = N[choice]
    local neur, idx;
    if #layer < 1 then
        layer = N.inp
    end
    idx = memChoice(#layer)
    neur = layer[idx]
    if not ign then
        return neur, choice
    end
    return neur
end
function Net.randomizeSynapses(N)
    for i = 1, #N.syns do
        local a = N.syns[i]
        local neur, lname = chooseNeur(N)
        a:connect(neur, 1)
        neur = chooseNeur(N, lname)
        a:connect(neur, 2)
    end
end

local mem = {[3] = {rand = choose_3}, [4] = {rand = choose_4}, [6] = {rand = choose_6} }
function memChoice(ls)
    local args;
    local r = mem[ls] 
    if not r then
        r = {}
        r[mem] = rdm.genTable(1, ls)
        rdm.ShuffleTable(r[mem])
        r.rand = rdm.makeRand(r[mem], 1)
        if not mem[ls] then
            mem[ls] = r
        end
    end
    return r.rand()
end
function memChoiceS(ls, skip)
    local args;
    local key = (tostring(ls).."_"..tostring(skip))
    local r = mem[key]
    if not r then
        local ch = 100//(ls)
        args = {}
        for i = 1, ls do
            if i ~= skip then
                args[i] = ch   
            end
        end
        r = {}
        r[mem] = rdm.genTableNV(args)
        
        r.rand = rdm.makeRand(r[mem], 1)
        mem[key] = r
    end
    return r.rand()
end

-- gets array of strings returns choice
function memChoiceStr(args, skip)
    local ls = #args
    local key = (tostring(ls).."_"..skip)
    local r = mem[key]
    if not r then
        local ch = 100//ls
        local t = {}
        for i = 1, ls do
            if args[i] ~= skip then
                t[args[i]] = ch
            end
        end
        r = {}
        t = rdm.genTableNV(t)
        rdm.ShuffleTable(t)
        r.rand = rdm.makeRand(t,1)
        mem[key] = r
    end
    return r.rand()
end

local chances = {[1] = 30, [2] = 30, [3] = 30, [4] = 10}
local r_chances = rdm.makeRand(rdm.genTableNV(chances), rand_100())
local new_chances = {[1] = 80, [4] = 20}
local rn_chances = rdm.makeRand(rdm.genTableNV(new_chances), rand_100())
function Net.mutate(N)
    local layer = N[memChoiceStr(laynames, "out")]
    if #layer < 1 then
        layer = N.inp
    end
    local ls = #layer
    local action;
    if #N.syns >= 20 and #N.hid >= 10 then
        action = rn_chances()
    else
        action = r_chances()
    end
    if action == 1 then -- reweight
        if #N.syns > 0 then
            local choice = memChoice(#N.syns)
            local exp = rand_10abs()*0.001
            local s = N.syns[choice]
            s:reweight(exp)
        end
    elseif action == 2 then -- add Synapse
        if #N.syns < 20 then
            local choice = memChoice(ls)
            N:addSynapseR(nil, layer[choice])
        end
        
    elseif action == 3 then -- add Neuron
        if #N.hid < 10 then
            N:addNeuron()
        end
    elseif action == 4 then
        if #N.syns > 0 then
            local choice = memChoice(#N.syns)
            local s = N.syns[choice]
            local n1, n2 = s.neur[1], s.neur[2]
            for i = 1, #n1.syns do
                if n1.syns[i] == s then
                    table.remove(n1.syns, i)
                end
            end
            for i = 1, #n2.syns do
                if n2.syns[i] == s then
                    table.remove(n2.syns, i)
                end
            end
            table.remove(N.syns, choice)
            table.remove(N.list, s[locan])
        end
    end
    ::skip::
end

function Net.copy(N, rN)
    rN = Net.newBrain(#N.inp, #N.hid, #N.out)
    
    for l in Net.traverse(N) do
        if l ~= "inp" then
            for i = 1, #rN[l] do
                local n = rN[l][i]
                for j = 1, #n.syns do
                    local s = N[l][i].syns
                    local n1 = s.neur[1]
                    local inp = rN[n1[layn]][n1[locan]]
                    rN:addSynapse(inp, n)
                end
            end
        end
    end 
end

function Net.print(N)
    print"Net.Print"
    for layer in Net.traverse(N) do
        print("layer",#layer)
        for i = 1, #layer do
            local l = layer[i]
            print("neuron: "..tostring(l).." with bias: "..tostring(l.bias).." has "..tostring(#l.syns).." connections")
            for j = 1, #l.syns do
                print(" and connected to "..tostring(l.syns[j].neur[2]).." by weight "..tostring(l.syns[j].weight))
            end
        end
    end
end

function Net.count(N)
    for layer in Net.traverse(N) do
        for i = 1, #layer do
            local l = layer[i]
            l:count()
        end
    end            
end


Neuron = {
    syns = {};
    bias = 0;
    out = 0;
}


function sigmoid(x)
    return 2/(1+exp(-4.9*x))-1
end

function square(x)
    return sigmoid(x*x)
end

function absolute(x)
    return sigmoid(x > 0 and x or -x)
end

function sine(x)
    return sigmoid(x > 0 and x or -x)
end


function linear(x)
    return x
end

function tanh(x)
    return sigmoid(exp(2*x)-1 / exp(2*x)+1) 
end

function ReLU(x)
    return sigmoid(x < 0 and 0 or x)
end

function gaussian(mean)
    return sigmoid(sqrt(-2 * log(random()))*cos(2 * pi * random()) + mean)
end

Neuron.funcs = {
    [1] = sigmoid;
    [2] = absolute;
    [3] = sine;
    [4] = tanh;
    [5] = ReLU;
    [6] = gaussian;
}



function Neuron.new(func, bias)
    return {
        syns = {};
        active = true;
        bias = bias or 0;
        func = func;
        count = Neuron.count;
        rebias = Neuron.rebias;
        reweight = Neuron.reweight;
    }
end

function Neuron.rebias(a, v)
    a.bias = a.func(v)
end

function Neuron.reweight(a, val)
    local syns = a.syns
    local out = 0
    for i = 1,  #syns do
        local syn = syns[i]
        syn.weight = val
    end
end

function Neuron.count(a)
    local syns = a.syns
    local out = 0
    for i = 1,  #syns do
        local syn = syns[i]
        out = out + syn.neur[1].bias*syn.weight
    end
    a.bias = a.func(out)
end


Synapse = {
    neur = {
        [1] = false;
        [2] = false;
    };
    weight = 0;
}


function Synapse.print(s)
    print(neur)
    print(s.neur[1], s.neur[2], s.weight)
end

function Synapse.reweight(s, val)
    s.weight = s.weight + val
end

function Synapse.connect(a, n, f)
    if type(n) ~= 'table' then
        error"Neuron must be table!"
    end
    -- if a.neur[1] == n and a.neur[2] == n then
    --     print(debug.traceback())
    --     error("Neuron can't link to itself!")
    -- end
    -- if a.neur[1] == n or a.neur[2] == n then
    --     print(debug.traceback())
    --     error("Neuron already linked!")
    -- end
    a.neur[f] = n
    n.syns[#n.syns+1] = a
end

function Synapse.new()
    return {
        neur = {};
        weight = 0;
        connect = Synapse.connect;
        reweight = Synapse.reweight;
    }
end

return Net;