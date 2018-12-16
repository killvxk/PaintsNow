-- Build script
-- PaintDream (paintdream@paintdream.com)
--

local commandLine = { ... }
local QuickCompile = load(System.SnowyStream.QuickTextResource("Engine/Boot/QuickCompile.lua"))()
print("CommandLine: " .. table.concat(commandLine, " "))
local forceRecompile = commandLine[1] == "true"
QuickCompile:CompileRecursive("Engine/", forceRecompile)
QuickCompile:CompileRecursive("Runtime/Class/", forceRecompile)
QuickCompile:CompileRecursive("Script/", forceRecompile)

-- Then pack resources
local Packer = require("Engine/Packer.lua")
Packer:Import("Assets/", "Packed/")
print("Build finished.")