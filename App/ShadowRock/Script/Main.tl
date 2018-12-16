-- Main.lua
-- Application Entry Module
local Main = {}
-- import global debug table
Debug = require("Script/Util/Debug")
local RemoteSync = require("Engine/RemoteSync")

--[[
local function UserShell()
	local uiModule = require("Script/Activity/UserLogin")
	assert(uiModule)
	local uiState = Core.State()

	uiModule.Main(uiState)
end
]]


function Main.Main()
	print("Main initializing ...")
	local remoteSync = RemoteSync.New("LeavesWingScenePoster")
	-- mount debug objects
	Debug.Objects.RemoteSync = remoteSync

	-- Initialize necessary entities
	--[[
	local console = require("/Script/Control/Console").New()
	local webProfiler = require("/Script/Server/WebProfiler").New("127.0.0.1", 8080, "./Website")
	local server = requrie("/Script/Server/ServerRoutine").New("127.0.0.1", 10086)
	local entryActivity = require("/Script/Activity/UserLogin").New("/Archive/demo.archive")

	-- Pass excution to entryActivity
	entryActivity.Main()

	-- call exit manually to terminate
	return Exit()	
	]]
end

return Main