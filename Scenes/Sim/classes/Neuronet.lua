local Neuron;
local Synapse;
--activation functions' prototypes
local sigmoid;
local square;
local linear;
local tanh;
local gaussian;
local ReLU;

-- loading math
local random = math.random;
local sqrt =  math.sqrt;
local cos = math.cos;
local log = math.log;
local exp = math.exp;
local abs = math.abs;

local function array(a)
    local i = 1
    return function()
        return a[i]
    end
end


local Net = {
    list = {};
    inp = {};
    out = {};
    hid = {};
}

function Net.addInput(a, n)
    a.inp[#a.inp+1] = n
    a.list[#a.list+1] = n
end

function Net.addOutput(a, n)
    a.out[#a.out+1] = n
    a.list[#a.list+1] = n
end

function Net.addHidden(a, n)
    a.hid[#a.hid+1] = n
    a.list[#a.list+1] = n
end

function Net.addSynapse(a, s)
    a.list[#a.list+1] = s
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
        addInput = Net.addInput;
        addOutput = Net.addOutput;
        addHidden = Net.addHidden;
        addSynapse = Net.addSynapse;
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
function Net.randomSynapses(N)
    for layer in Net.traverse(N) do
        if layer == N.out then
            break;
        end
        for i = 1, #layer do
            local l = layer[i]
            local s = Synapse.new()
            s.weight = random(1,4);
            N:addSynapse(s)
            s:connect(l, 1)
            local chosen =_connectable[random(1,2)] -- choosing between hidden or output layer
            if #N.hid == 0 then
                chosen = "out"
            end
            local choice = N[chosen][random(1, #N[chosen])] -- choosing from neurons of chosen layer
            s:connect(choice, 2)
        end    
    end
    
end

function Net.Print(N)
    print"Net.Print"
    for layer in Net.traverse(N) do
        print("layer",#layer)
        for i = 1, #layer do
            local l = layer[i]
            print("neuron: "..tostring(l).." with bias: "..tostring(l.bias).." has "..tostring(#l.Syns).." connections")
            for j = 1, #l.Syns do
                print(" and connected to "..tostring(l.Syns[j].neur[2]).." by weight "..tostring(l.Syns[j].weight))
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
    Syns = {};
    bias = 0;
    out = 0;
}


function sigmoid(x)
    return 2/(1+exp(-4.9*x))-1
end

function square(x)
    return x*x
end

function linear(x)
    return x
end

function tanh(x)
    return exp(2*x)-1 / exp(2*x)+1 
end

function ReLU(x)
    return x < 0 and 0 or x
end

function gaussian(mean)
    return sqrt(-2 * log(random()))*cos(2 * pi * random()) + mean
end

function Neuron.new(func)
    return {
        Syns = {};
        bias = 0;
        func = func;
        count = Neuron.count;
        rebias = Neuron.rebias
    }
end

function Neuron.rebias(a, v)
    a.bias = a.func(v)
end


function Neuron.count(a)
    local syns = a.Syns
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

function Synapse.connect(a, n, f)
    if a.neur[1] == n and a.neur[2] == n then
        error("Neuron can't link to itself!")
    end
    if a.neur[1] == n or a.neur[2] == n then
        error("Neuron already linked!")
    end
    a.neur[f] = n
    n.Syns[#n.Syns+1] = a
end

function Synapse.new()
    return {
        neur = {};
        weight = 0;
        connect = Synapse.connect;
    }
end

return Net;