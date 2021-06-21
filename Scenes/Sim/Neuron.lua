local Neuron = {
	value = 0.0
}

local NLayer = {
	weight = 0.0;
	neurons = {};
}

local NNet = {
	layers = {};
}

local function NewNet(inp, hid, out)
	
end

local function create(inputs, outputs, hiddenlayers, neurons)
	inputs = inputs or 1
	outputs = outputs or 1
	hiddenlayers = hiddenlayers or ceil(inputs/2)
	neurons = neurons or ceil(inputs*(2/3)+outputs)
	--order goes network[layer][neuron][wieght]
	local network = setmetatable({},{__index = NeuralNetwork});
	network[1] = {}  --Input Layer
	for i = 1,inputs do
	  network[1][i] = {}
	end
	for i = 2,hiddenlayers+2 do --plus 2 represents the output layer (also need to skip input layer)
	  network[i] = {}
	  local neuronsInLayer = neurons
	  if i == hiddenlayers+2 then
		neuronsInLayer = outputs
	  end
	  for j = 1,neuronsInLayer do
		network[i][j] = {bias = math.random()*2-1}
		local numNeuronInputs = #(network[i-1])
		for k = 1,numNeuronInputs do
		  network[i][j][k] = math.random()*2-1 --return random number between -1 and 1
		end
	  end
	end
	return network
end
local prv = {}
local beg = {}


local function DeepLook(orig)--(table, table)
    local i = 0
    prv[i] = orig
    print(orig)
    while i >= 0 do
        for k, v in next, prv[i], beg[i] do
            
            if type(v) == 'table' then
                local c = {}
                beg[i] = k
				print(("	"):rep(i),tostring(k).." "..tostring(v))
                i = i + 1 --Updating depth

                prv[i] = v
                
				
                goto fin
            else
                print(("	"):rep(i),tostring(k).." "..tostring(v))
            end
        end
        i = i - 1
        ::fin::
    end
end
function checkT(T)
	print(tostring(T).." check")
	for k,v in pairs(T) do
		print(k,v)
	end
	print()
end

DeepLook(create(2,3,2,5))