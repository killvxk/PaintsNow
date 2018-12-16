-- Entry.lua
-- Util bootstrap, do not use require here

EnableTypedlua = false
local loader, message = load(System.SnowyStream.QuickTextResource("Engine/Boot/Util.lua"))
if not loader then
	print("Load Util error: " .. message)
end

Util = loader(...)

-- Compiles typedlua scripts
local builder = load(System.SnowyStream.QuickTextResource("Build.lua"))
pcall(builder)

-- Enable Debug Console
Util.Console()

-- THEN START MAIN!
App = require("Script/Util/TestUI.lua")
-- App = require("Script/Main.lua")
App.Main()