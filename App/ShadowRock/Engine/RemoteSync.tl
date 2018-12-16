local RemoteSync = {}
RemoteSync.__index = RemoteSync

local CreateNullWeaver = function () : NsGalaxyWeaver__Weaver? return nil end
RemoteSync.Weaver = (CreateNullWeaver())
RemoteSync.Address = ""

function RemoteSync:OnConnectionCallback(...)

end

function RemoteSync:OnWeaverCallback(...)

end

function RemoteSync.New(address : string)
	assert(address)
	print("Starting remote sync ...")
	local weaver = GalaxyWeaver.CreateWeaver(address)
	if weaver then
		local object = setmetatable({ Weaver = weaver }, { __index = RemoteSync })
		GalaxyWeaver.SetWeaverRpcCallback(weaver, function (...)
			object:OnWeaverCallback(...) 
		end)

		GalaxyWeaver.SetWeaverConnectionCallback(weaver, function (...)
			object:OnConnectionCallback(...)
		end)

		GalaxyWeaver.StartWeaver(weaver)
		print("RemoteSync succeeded")
		return object
	else
		print("Unable to create remote sync.")
	end
end

function RemoteSync:Restart()
	GalaxyWeaver.StartWeaver(self.Weaver)
end

function RemoteSync:Stop()
	GalaxyWeaver.StopWeaver(self.Weaver)
end

function RemoteSync:Verbose()
	print ("Weaver address: " .. self.Address)
end

return RemoteSync