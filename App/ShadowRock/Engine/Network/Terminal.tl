--[[
	Terminal.tl: Simple Network Terminal Wrapper for EchoLegend APIs.
	Author: PaintDream (paintdream@paintdream.com)
]]
local Core = require("Engine/Core")

interface Connection
	Handle : any
	Src : string
	Dst : string
end

local Terminal = {}
local Peers : { any : Connection } = {}

-- uniform global dispatcher
local dispatcher : NsEchoLegend__WorkDispatcher? = EchoLegend.OpenDispatcher()
-- currently we do not deactivate it manually, just let gc do it.
EchoLegend.ActivateDispatcher(dispatcher)

Terminal.Peers = Peers

function Terminal.StateHandler(connection : NsEchoLegend__Connection, isConnect : boolean, src : string, dst : string)
	return true
end

function Terminal.DataHandler(connection : NsEchoLegend__Connection, data : string, method : string?, header : string?, extraData : string?)
	return true
end

function Terminal:Write(connection : NsEchoLegend__Connection, data : string)
	return EchoLegend.WriteConnection(connection, data)
end

function Terminal:Read(connection : NsEchoLegend__Connection, data : string)
	return EchoLegend.ReadConnection(connection, data)
end

-- Broadcast data to all connected terminals.
function Terminal:Broadcast(data : string)
	for k in pairs(self.Peers) do
		self:Write(k, data)
	end
end

-- Add a connection terminal to current terminal
function Terminal:AddConnection(connection : NsEchoLegend__Connection)
	EchoLegend.GetConnectionInfo(connection, function (src : string, dst : string)
		self.Peers[connection] = {
			Handle = connection,
			Src = src,
			Dst = dst,
		}
	end)
end

-- Remove a connection terminal from current terminal
function Terminal:RemoveConnection(connection : NsEchoLegend__Connection)
	if connection then
		self.Peers[connection] = nil
	end
end

function Terminal:Query()
	return self.Peers
end

-- Handle and dispatch http events, it's an internal function.
function Terminal:EventHandlerHttp(connection : NsEchoLegend__Connection, status : string)
	if status == "Connected" then
		EchoLegend.GetConnectionInfo(connection, function (src : string, dst : string)
			if self.StateHandler(connection, true, src, dst) then
				self:AddConnection(connection)
			end
		end)

		EchoLegend.ReadConnectionHttpRequest(connection, function (uri : string, method : string, header : string, data : string)
			local code, data, reason, header = self.DataHandler(connection, uri, method, header, data)
			if code and data and reason and header then
				EchoLegend.WriteConnectionHttpResponse(connection, code, data, reason, header)
			end
		end)

		return true
	elseif status == "Read" then
		EchoLegend.ReadConnectionHttpRequest(connection, function (uri : string, method : string, header : string, data : string)
			local code, data, reason = self.DataHandler(connection, uri, method, header, data)
			if code and data and reason then
				EchoLegend.WriteConnectionHttpResponse(connection, code, data, reason)
			end
		end)

		return true
	elseif status == "Close" or status == "Error" then
		self.StateHandler(connection, false, "", "")
		self:RemoveConnection(connection)
		return false
	end
end

function Terminal:EventHandler(connection : NsEchoLegend__Connection, status : string)
	if status == "Connected" then
		EchoLegend.GetConnectionInfo(connection, function (src : string, dst : string)
			if self.StateHandler(connection, true, src, dst) then
				self:AddConnection(connection)
			end
		end)

		return true
	elseif status == "Read" then
		EchoLegend.ReadConnection(connection, function (data : string)
			self.DataHandler(connection, data)
		end)

		return true
	elseif status == "Close" or status == "Error" then
		self.StateHandler(connection, false, "", "")
		self:RemoveConnection(connection)
		return false
	end
end

function Terminal:HttpRequest(host : string, path : string, method : string, options : { string : string }, header : { string : string }, callback : (string)->())
	local connection : NsEchoLegend__Connection? = EchoLegend.OpenConnection(dispatcher, host, true, function (connection : NsEchoLegend__Connection, status : any)
		local statusCode = tonumber(status) or 0
		if status == "Connected" then
			local url = EchoLegend.MakeURL("", "", path, options, "")
			EchoLegend.WriteConnectionHttpRequest(connection, url, method, header, "")
		elseif type(status) == "integer" and statusCode >= 200 and statusCode <= 499 then
			EchoLegend.ReadConnectionHttpRequest(connection, function (uri : string, method : string, header : string, data : string)
				EchoLegend.DeactivateConnection(connection)
				print("Data received")
				callback(data)
			end)
		end
	end, false)

	if connection then
		EchoLegend.ActivateConnection(connection)
	end

	return connection
end

function Terminal:Connect(ip : string)
	local instance = Core.Instance()
	local connection : NsEchoLegend__Connection = EchoLegend.OpenConnection(dispatcher, ip, false, function (connection : NsEchoLegend__Connection, status : string)
		local success = self:EventHandler(connection, status)
		if status == "Connected" then
			Core.Post(instance, success)
		end
	end, true)

	-- establishing
	EchoLegend.ActivateConnection(connection)

	local success = Core.StepListen()
	if success then
		return connection
	else
		EchoLegend.DeactivateConnection(connection)
		return nil
	end
end

function Terminal:Disconnect(connection : NsEchoLegend__Connection)
	self.Peers[connection] = nil
	EchoLegend.DeactivateConnection(connection)
end

function Terminal.New(ip : string, 
			dataHandler : any,
			stateHandler : (any, boolean, string, string)->(boolean),
			http : boolean)
	local instance = Core.Instance()
	local eventHandler = http and Terminal.EventHandlerHttp or Terminal.EventHandler
	local terminal = setmetatable({ IP = ip, Peers = {}, DataHandler = dataHandler, StateHandler = stateHandler }, { __index = Terminal })

	local listener : NsEchoLegend__Listener? = EchoLegend.OpenListener(dispatcher, ip, http, function (listener, event)
		if event == "Connected" then
			EchoLegend.GetListenerInfo(listener, function (ip)
				print("Listener created on IP: " .. ip)
			end)
			Core.Post(instance)
		end
	end, function (listener : NsEchoLegend__Listener, connection : NsEchoLegend__Connection)
		-- do not add connection here
		-- wait for actual event
		-- EchoLegend.WriteConnection(connection, "Hello")
		-- print("NEW CONNECTION")
		eventHandler(terminal, connection, "Connected")
	end, function (connection, status)
		--print("SERVER GOT" .. status)
		eventHandler(terminal, connection, status)
	end, true)

	if listener then
		EchoLegend.ActivateListener(listener)
		print("Listener activated")
		return terminal
	else
		print("Create listener failed!")
		return nil
	end
end

return Terminal