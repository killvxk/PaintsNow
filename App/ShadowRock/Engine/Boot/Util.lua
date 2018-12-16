-- Boot/Util.lua
local Util = {}

if UtilLoaded then
	return
end

UtilLoaded = true

-- Import All Modules and functions in System to global table for typedlua routing ...
local function ImportSymbols(env, key, collection)
	if collection.__delegate__ then
		print("Symbol imported for module " .. key)
		for k, v in pairs(collection) do
			env[k] = v
			if type(v) == "table" then
				ImportSymbols(env, k, v)
			end
		end
	end
end

ImportSymbols(_ENV, "System", System)

assert(SnowyStream)

-- EnableTypedlua = true

-- Optional lua libraries
local luaBridgeMod = RayForce.CreateObject("Lua53.dll!NativeLua")
local luaBridge = nil
if luaBridgeMod then
	luaBridge = RayForce.QueryInterface(luaBridgeMod)
end

-- save extra space introduced by typedlua
local orgSetmetatable = setmetatable
setmetatable = function (obj, meta)
	-- fold continous indexing
	local p = meta.__index
	if type(p) == "table" and p == p.__index then
		meta = p
	end

	return orgSetmetatable(obj, meta)
end

local function Expand(t, newLine, depth)
	if type(t) == "table" and depth ~= 0 then
		local convert = {}
		for i, v in pairs(t) do
			if type(v) == "nil" then
				v = "nil"
			end

			m = Expand(v, false, depth - 1)
			table.insert(convert, newLine and m or ("[" .. i .. "] = " .. m))
		end

		if newLine then
			return table.concat(convert, '\n')
		else
			return "{ " .. table.concat(convert, ', ') .. " }"
		end
	else
		return tostring(t)
	end
end

Util.ObjectExpandDepth = 1
print = function (...)
	local args = { ... }
	if #args ~= 0 then
		return Print(Expand(args, true, Util.ObjectExpandDepth + 1))
	end
end

string.startswith = function (s, p)
	return string.sub(s, 1, string.len(p)) == p
end

string.endswith = function (s, p)
	return string.sub(s, string.len(s) - string.len(p) + 1) == p
end

string.split = function (str, delimiter)
	if str == nil or str == '' or delimiter == nil then
		return nil
	end
	
	local result = {}
	for match in (str .. delimiter):gmatch("(.-)" .. delimiter) do
		table.insert(result, match)
	end

	return result
end

function SortedPairs(input, comp)
	local keys = {}
	for k in pairs(input) do
		table.insert(keys, k)
	end

	table.sort(keys, comp)
	local i = 0

	return function ()
		i = i + 1
		local v = keys[i]
		return v, input[v]
	end
end

local exts = { "", ".lua" }
local descriptions = {}

if EnableTypedlua then
	exts = { "", ".tl", ".tlm", ".lua" }
	io = io or {}
	local orgOpen = io.open
	local orgClose = io.close
	local orgSearcher = package.searchpath

	local simfile = {}
	simfile.__index = simfile

	function simfile:read(s)
		local c = self.content
		self.content = nil
		return c
	end

	function simfile:close()
	end
	-- io operations only for tld usage
	io.open = function (f, m)
		-- print("Opening file " .. f)	
		local desc = descriptions[f]
		if type(desc) == "string" then
			local file = { content = desc }
			-- print("Extern Module " .. f)
			setmetatable(file, simfile)
			return file
		end

		-- override all tld requests
		-- print("Try Load resource: " .. f)
		local content = SnowyStream.QuickTextResource(f)
		if content then
			local file = { content = content }
			-- print("Buildin Module " .. f)
			setmetatable(file, simfile)
			return file
		end
	end

	io.close = function (f)
		return type(f) ~= "table" and oldclose(f)
	end

	typedlua = nil
end

local function searchpath(name, filter, ...)
	local isTld = string.find(filter, ".tld;")
	if descriptions[name] then
		if isTld then
			return name
		else
			return nil, "No tld file for Buildin module " .. name
		end
	end

	for i, v in ipairs(isTld and { ".tld" } or exts) do
		local c = name .. v
		if SnowyStream.FileExists(c) then
			return c
		end
	end

	return nil, "Failed to open file: " .. name	
end

package.searchpath = searchpath

function require(name, ...)
	-- print("Requiring " .. name)
	local mod = package.loaded[name]
	if mod then
		return mod
	end

	path, msg = searchpath(name, "")
	if not path then
		print("Module " .. name .. " not found.")
		return nil
	end

	local content = SnowyStream.QuickTextResource(path)
	if content then
		-- Pass current _ENV to sub module
		-- local code = typedlua.compile(content, name, false)
		-- print(code)
		local loadproc = load
		local lowerPath = string.lower(path)
		if EnableTypedlua then
			if string.endswith(lowerPath, ".tl") or string.endswith(lowerPath, ".tlm") then
				loadproc = typedlua.loadstring
			end
		end

		local chunk, errMsg = loadproc(content, name, "t", _ENV)
		if chunk then
			mod = chunk(name)
		else
			print("Load module " .. name .. " failed!")
			print("Compiler log: " .. errMsg)
		end
	elseif luaBridge then
		mod = luaBridge.Require(GetFullPath(path))
	end

	package.loaded[name] = mod
	return mod
end

if EnableTypedlua then
	typedlua = require("typedlua")
	tlchecker = require("typedlua.tlchecker")
	tlst = require("typedlua.tlst")
	tlparser = require("typedlua.tlparser")
	tlcode = require("typedlua.tlcode")

	-- register API prototypes

	local mapTypes = {
		["int"] = "number",
		["short"] = "number",
		["char"] = "number",
		["float"] = "number",
		["double"] = "number",
		["__int64"] = "number",
		["long"] = "number",
		["string"] = "string",
		["String"] = "string",
		["bool"] = "boolean",
		["vector"] = "{any:any}",
		["IScript__BaseDelegate"] = "any",
		["IScript__Request__Ref"] = "any",
		["IScript__Request__PlaceHolder"] = "any"
	}

	local function GetTypeName(t, regTypes)
		if type(t) == "string" then
			local m = mapTypes[t]
			assert(m, "Unrecognized type: " .. t)
			return m
		end

		local m = mapTypes[t.Type]
		if m then return m end

		local r = regTypes[t.Type]
		if not r then
			local stmt = {}
			t.Pointer = false -- cannot resolve userdata for :type def, use pointer instead.
			table.insert(stmt, (t.Pointer and "interface " or "typealias ") .. t.Type)
			regTypes[t.Type] = true
			if t.Pointer then
				table.insert(stmt, "\n")
				for k, v in SortedPairs(t.Fields) do
					table.insert(stmt, "\t" .. k  .. " : " .. GetTypeName(v, regTypes)  .. "\n")	
				end
				table.insert(stmt, "end\n")
			else
				table.insert(stmt, " = {\n")
				for k, v in SortedPairs(t.Fields) do
					table.insert(stmt, "\t" .. (type(k) == "string" and ("\"" .. k .. "\"") or k)  .. " : " .. GetTypeName(v, regTypes)  .. ",\n")	
				end
				-- table.insert(stmt, "\t\"__rawpointer\" : integer")
				table.insert(stmt, "}\n")
			end

			regTypes[t.Type] = table.concat(stmt)
		end

		return (t.List and "{ integer : " or "") .. t.Type .. (t.List and " }" or (t.Optional and "?" or ""))
	end

	local function RegisterModule(regTypes, mod, collection)
		local tld = {}
		local tl = {}
		
		local def = "function %s(%s)\n\tlocal ret %s= %s\nend\n"
		-- print("Inspect mod " .. mod)
		-- drop 'Module' tail if exists
		local mainType = mod:match("(.-)Module")

		local creation = ""
		for name, desc in SortedPairs(collection.Inspect()) do
			local paramsList = {}
			local declList = {}
			local callList = {}
			local isMemberFunc = false
			for index, t in ipairs(desc.Params) do
				local typename = GetTypeName(t, regTypes)
				local argname = "arg" .. tostring(index)
				table.insert(paramsList, typename)
				-- member function?
				if index == 1 and mainType and typename:find(mainType .. "?") then
					isMemberFunc = true
				else
					table.insert(declList, argname .. " : " .. typename)
					table.insert(callList, argname)
				end
			end

			-- print("Module item name " .. name)
			table.insert(tld, name .. ": (" .. table.concat(paramsList, ", ") .. ") -> (any)")
			-- table.insert(tld, "VERSION: string")

			if mainType then -- currently we only process '*Module' as classes
				local funcname = mainType .. (isMemberFunc and ":" or ".") .. name
				local retType = ""
				local orgname = mod .. "." .. name
				local body
				if isMemberFunc then
					if #callList == 0 then
						body = orgname .. "(self)"
					else
						body = orgname .. "(self, " .. table.concat(callList, ", ") .. ")"
					end

					body = body .. "\n\treturn ret"
				else
					retType = ": NsMythForest__" .. mainType .. "? " 
					body = orgname .. "(" .. table.concat(callList, ", ") .. ")"
					if name == "Create" then
						body = body .. "\n\treturn setmetatable(ret, { __index = " .. mainType .. "})"
					else
						body = body .. "\n\treturn ret"
					end
				end

				local stmt = def:format(funcname, table.concat(declList, ", "), retType, body)
				if name == "Create" then
					creation = stmt
				else
					table.insert(tl, stmt)
				end
			end
		end

		if creation then
			table.insert(tl, creation)
		end

		local tlcontent = nil
		if mainType then
			tlcontent = "local " .. mainType .. " = {}\n"
			tlcontent = tlcontent .. mainType .. ".__index = " .. mainType .. "\n\n"
			tlcontent = tlcontent .. table.concat(tl, "\n") .. "\n"
			tlcontent = tlcontent .. "return " .. mainType
		end

		-- print(table.concat(tl, "\n"))
		return table.concat(tld, "\n"), tlcontent
	end

	print("TypedLua: Fetching module defs...")
	local tld = {}
	-- Preload
	local extramods = {}
	local regTypes = {}
	local classdefs = {}
	
	local function RegisterModulesRecursive(modname, collection)
		if type(collection) == "table" then
			if type(collection.Inspect) == "function" then
				local tlddef, classdef = RegisterModule(regTypes, modname, collection)
				descriptions[modname] = tlddef
				classdefs[modname] = classdef
				table.insert(extramods, modname)
			end

			for k, v in pairs(collection) do
				-- RegisterModulesRecursive(modname .. "." .. k, v)
				RegisterModulesRecursive(k, v)
			end
		end
	end

	RegisterModulesRecursive("System", System)

	-- print("TypedLua: Defs loaded.")

	local concatRegs = {}
	for name, val in SortedPairs(regTypes) do
		table.insert(concatRegs, val)
	end

	-- add defination of interfaces
	descriptions["Interface"] = table.concat(concatRegs, "\n")
	table.insert(extramods, "Interface")

	local fileHeader = [[
-- PaintsNow Script Interface Description File (TypedLua Version)
-- This file is generated by LeavesWing in PaintsNow Suite. Please do not modify it manually.
--
]]
	local classHeader = [[
-- PaintsNow Script Interface Class Wrapper File (TypedLua Version)
-- This file is generated by LeavesWing in PaintsNow Suite. Please do not modify it manually.
--
]]
	local function WriteRuntimeFilesRecursive(modname, collection)
		if type(collection) == "table" and descriptions[modname] then
			local tlddef = fileHeader .. descriptions[modname]
			local tldPath = "Runtime/Description/".. modname .. ".tld"
			local content = SnowyStream.QuickTextResource(tldPath)
			if not content or content ~= tlddef then
				print("Update runtime defs: " .. tldPath .. ". [" .. string.len(tlddef) .. " Bytes]")
				local file = SnowyStream.CreateFile(tldPath, "wb")
				if file then
					SnowyStream.WriteFile(file, tlddef)
					SnowyStream.CloseFile(file)
				end
			end

			local classdef = classdefs[modname]
			if classdef then
				classdef = classHeader .. classdef
				local classPath = "Runtime/Class/" .. string.match(modname, "(.-)Module") .. ".tlm"
				local classContent = SnowyStream.QuickTextResource(classPath)
				if not classContent or classContent ~= classdef then
					print("Update runtime class: " .. classPath .. ". [" .. string.len(classdef) .. " Bytes]")
					local file = SnowyStream.CreateFile(classPath, "wb")
					if file then
						SnowyStream.WriteFile(file, classdef)
						SnowyStream.CloseFile(file)
					end
				end
			end

			for k, v in pairs(collection) do
				-- WriteRuntimeFilesRecursive(modname .. "." .. k, v)
				WriteRuntimeFilesRecursive(k, v)
			end
		end
	end

	WriteRuntimeFilesRecursive("System", System)
	WriteRuntimeFilesRecursive("Interface", {})

	tlchecker.extramods = extramods

	-- print("REQ: " .. table.concat(reqfile, "\n"))
	print("TypedLua: Trying to parse warming up files ...")
	local code = "System.Print('Hello, TypedLua!' .. string.rep('a', 3))"
	local chunkname = "Preload"
	local ast, err = tlparser.parse(code, chunkname, true, true)

	print("TypedLua: Type checking warning up files ...")

	local messages = tlchecker.typecheck(ast, code, chunkname, false, true, false)
	if type(messages) == "table" then
		print("Syntax checking  with " .. #messages .. " errors")
		for i, v in ipairs(messages) do
			print("" .. v.l .. "->" .. v.msg)
		end
	elseif type(message) == "string" then
		print("Compiler error at" .. path .. ":")
		print(messages)
		messages = {}
	end

	print("TypedLua: Type checking finished.")
end

function using(module, ...)
	if type(module) == "string" then
		return using(require(module, ...))
	else
		assert(type(module) == "table")
		for k, v in pairs(module) do
			if rawget(_ENV, k) ~= nil then
				error("Module.using(): Global variable " .. k .. " collision detected.", 1)
			end

			_ENV[k] = v
		end

		return module
	end
end

function Util.Async(realtime, func, done)
	return IceSpring.Post(function()
		if done then
			return done(func())
		else
			return func()
		end
	end, realtime)
end

function Util.Console(writer, ...)
	local args = table.pack(...)
	writer = writer or print
	local prefixes = { "", "return "}
	ListenConsole(function (command)
		-- print("EXEC COMMAND " .. command)
		for i, prefix in ipairs(prefixes) do
			local v, message = load(prefix .. command, "Console", "t", _ENV)
			if v then
				return writer(v(table.unpack(args)))
			end
		end
		
		writer("Invalid command: " .. command .. "\n" .. message)
	end)
end

function Util.StartTimer(delay)
	local timer = { alive = true }
	timer.clock = HeartVioliner.CreateClock(delay)
	timer.queue = HeartVioliner.CreateQueue()
	timer.delay = delay
	HeartVioliner.Attach(timer.clock, timer.queue)
	HeartVioliner.Listen(timer.queue, function(v, obj)
		if timer.alive and obj(v) then
			HeartVioliner.Push(timer.queue, v + delay, obj)
		end
	end)

	HeartVioliner.Start(timer.clock)
	return timer
end

function Util.PushTimerEvent(timer, event)
	HeartVioliner.Push(timer.queue, HeartVioliner.Now(timer.clock) + timer.delay, event)
end

function Util.PauseTimer(timer)
	HeartVioliner.Pause(timer.clock)
end

function Util.StopTimer(timer)
	timer.alive = false
	-- HeartVioliner.Stop(timer.clock)
end

print("Util initialized")

return Util
