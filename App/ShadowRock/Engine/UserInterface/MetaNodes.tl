local MetaNodes = {}
local initHost : any
MetaNodes.__host = initHost
MetaNodes.__index = MetaNodes

local interface Node
	SetParent : (Node?) => ()
end

function MetaNodes.NewNodeCreator(object : {}) : Node?
	assert(false, "SetNewNodeCallback must be called before creating nodes")
	local node : Node? = nil
	return node
end

function MetaNodes.SetNewNodeCreator(createRoutine)
	MetaNodes.NewNodeCreator = createRoutine
end

function MetaNodes:Clear()
	for k, v in pairs(self) do
		if k ~= "__host" then
			rawset(self, k, nil)
		end
	end

	LunaWidget.ClearWidgetChildren(self.__host.Handle)
end

function MetaNodes:Resort()
	if self == MetaNodes then
		return
	end

	local orderedList : { integer : any } = { }
	for k, v in pairs(self) do
		if k ~= "__host" then
			if not v.Order then
				error("Missing order of key = " .. k)
			end
			
			table.insert(orderedList, { k, v })
		end
	end

	-- print("Resorting ...")
	table.sort(orderedList, function (a, b)
		return a[2].Order < b[2].Order
	end)

	assert(self.__host)
	assert(self.__host.Handle)
	self:Clear()

	-- reinsert
	for k, v in ipairs(orderedList) do
		self[v[1]] = v[2]
	end
end

function MetaNodes.New(host, nodes) : { any : any }
	local newNodes = nodes or rawget(host, "Nodes") or {}
	newNodes.__host = host or newNodes.__host
	setmetatable(newNodes, MetaNodes)
	newNodes:Resort()
	return newNodes
end

-- __newindex metamethod
function MetaNodes.__newindex(object, key, value)
	assert(object.__host ~= value)
	if (key ~= "__newindex" and key ~= "__host") then
		-- assert(type(value) == "table", "The value of key " .. key .. " is not a valid table")
		local host = object.__host
		if (not value.__host) then
			-- create new 
			-- print("Create New at key " .. key)
			value = MetaNodes.NewNodeCreator(value)
			assert(type(value.Handle) == "userdata")
			-- Detach from root objects
			host.Luna.Root[host] = nil
			value.Parent = host
			LunaWidget.SetWidgetParent(value.Handle, host.Handle, false)
			-- value.Attribs.Show = true
			-- force update
			if (rawget(host.Attribs, "Rect")) then
				rawset(host.Attribs.Rect, "Rect", nil)
			end

			assert(type(value.Attribs) == "table")
			--PostEvent("OnNewNode", host, host, key, value)
		else
			error("Incorrect layer~!")
		end
	end

	assert(type(value.Handle) == "userdata")
	-- assert(getmetatable(value.Attribs) == MetaAttribs)

	return rawset(object, key, value)
end

return MetaNodes