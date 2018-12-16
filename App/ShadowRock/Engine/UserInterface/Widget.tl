-- Widget.tl
-- By PaintDream (paintdream@paintdream.com)
-- import LunaWidget defs
-- require "Runtime/Interface"
-- require "Runtime/LunaWidget"

local Widget = {}
Widget.__index = Widget

local Core = require("Engine/Core")
local MetaAttribs = require("Engine/UserInterface/MetaAttribs")
local MetaEvents = require("Engine/UserInterface/MetaEvents")
local MetaNodes = require("Engine/UserInterface/MetaNodes")

local UIConfigTemplate : { any : any } = {
	Title = { TextAlignCenter = true },
	Plain = { TextAlignCenter = false },
	Language = { },
	Border = { },
	Button = { },
	Tag = { },
	Frame = { },
	Progress = { },
	Slider = { },
	Switch = { },
	Loading = { },
	TextBox = { },
	SliderVertical = { }
}

setmetatable(UIConfigTemplate.Language, {
	__index = function (tab, key)
		return rawget(tab, key) or key
	end
})

local config : any = Core.Copy(UIConfigTemplate)
Widget.UIConfig = config

-- Module static constants
local virtRoot : { any : any } = {}
local InitWidgetHandle = function() : NsLunaWidget__Widget? return nil end
local InitAny = function () : { any : any } return {} end
local InitWidget = function () : any return nil end
local InitMapper = function (input) : { any : any } return {} end

Widget.Name = "Widget"
Widget.Luna = (InitAny())
Widget.Parent = (InitWidget())
Widget.Handle = (InitWidgetHandle())
Widget.Nodes = (InitMapper(MetaNodes))
Widget.Events = (InitMapper(MetaEvents))
Widget.Attribs = (InitMapper(MetaAttribs))
Widget.Depth = 0
Widget.Order = 1

Widget.ReservedTable = {
	["__host"] = true,
	["__index"] = true,
	["__newindex"] = true,
	["Parent"] = true,
	["Handle"] = true,
	["Order"] = true,
	["Nodes"] = true,
	["Attribs"] = true,
	["Events"] = true,
}

-- HotUpdate support
function Widget.HotUpdate(widget : self)
	-- Patch new metatables to old widgets
	Core.Patch(widget.ReservedTable, Widget.ReservedTable)
end

function Widget.IsReserved(name)
	return Widget.ReservedTable[name]
end

-- Begin Widget functions
function Widget:SetParent(parent : self)
	self.Parent = parent
	self.Luna.Root[self] = nil
	local handle : NsLunaWidget__Widget? = nil
	if parent then
		handle = parent.Handle
	end

	LunaWidget.SetWidgetParent(self.Handle, handle, false)
end

function Widget:ClearChildren()
	self.Nodes:Clear()
end

function Widget:ResortChildren()
	return self.Nodes:Resort()	
end

function Widget:LockFocus()
	
end

function Widget:Replace(k : string, v : self)
	local g = self.Nodes[k]
	if g then
		-- Detach from parent
		g:SetParent(nil)
		self.Nodes[k] = nil
	end

	-- Assign new sub widget
	self.Nodes[k] = v
	self:ResortChildren()
	return v
end

function Widget.New(prototype) : self
	local object = prototype or {}
	setmetatable(object, Widget)
	if not object.Handle then
		local handle : NsLunaWidget__Widget? = LunaWidget.CreateWidget()
		object.Handle = handle
		object.Events = (MetaEvents.New(object))
		object.Attribs = (MetaAttribs.New(object))
		object.Depth = object.Depth or 1
		object.Order = object.Order or 1

		object.Nodes = (MetaNodes.New(object))
		object.Attribs.Show = false
	end

	return object
end

MetaNodes.SetNewNodeCreator(function (object)
	return Widget.New(object)
end)

local function ParseSelector(selector : string)
	local ch = string.sub(selector, 1, 1) 
	if ch == "$" then
		return function (k, v)
			if v.Attribs[string.sub(selector, 1)] then
				return true
			end
		end
	else
		return function (k, v)
			if k == selector then
				return true
			end
		end
	end
end

local function CreateSelector(selector : string | (string?) -> (boolean))
	if type(selector) == "string" then
		local str = selector
		selector = function (x)
			return x.Attribs[str]
		end
	end

	return selector
end

function Widget.ForeachEx(x, selector)
	assert(x)

	return function (t : {any : any}, kk : any)
		assert(t)
		local v = nil
		local k = kk
		repeat
			k, v = next(t, k)	
			if k == nil then
				return
			end
		until not Widget.IsReserved(k) and (not selector or selector(v))

		return k, v
	end, x.Nodes
end

local function TransSelector(func)
	return function (x, selector)
		assert(x)
		selector = CreateSelector(selector)
		return func(x, selector)	
	end
end

function Widget.MatchUpEx(x : self, selector)
	return function (t : self, k : self)
		local parent = k or t or x
		repeat
			parent = parent.Parent
			if parent == nil then return end
		until not selector or selector(parent)

		return parent, parent
	end
end


Widget.Foreach = (TransSelector(Widget.ForeachEx))
Widget.MatchUp = (TransSelector(Widget.MatchUpEx))

function Widget:MatchDownEx(x : {}, selector)
	return Core.Inspect(x, selector, Widget.Foreach)
end
Widget.MatchDown = (TransSelector(Widget.MatchDownEx))


local function Contain(rect : float4, x : number, y : number)
	if not rect then
		print(debug.traceback())
	end

	return x >= rect[1] and y >= rect[2] and x <= rect[3] and y <= rect[4]
end

local function PrintRect(rect : float4)
	print("(" .. tostring(rect[1]) .. ", " .. tostring(rect[2]) .. ", " .. tostring(rect[3]) .. ", " .. tostring(rect[4]) .. ")")
end

function Widget:DebugTrace()
	local p = self
	local n = 0
	while p ~= nil do
		local q = p.Parent
		local key = "<Root>"
		if q ~= nil then
			for k, v in pairs(q.Nodes) do
				if v == p then
					key = tostring(k)
				end
			end
		end

		local text = p.Attribs.Text == "" and "<Empty>" or p.Attribs.Text
		local rect : float4 = p.Attribs.Rect
		print("|" .. string.rep("--", n) .. key .. ": (" .. tostring(rect[1]) .. ", " .. tostring(rect[2]) .. ", " .. tostring(rect[3]) .. ", " .. tostring(rect[4]) .. "): " .. text)
		p = q
		n = n + 1
	end
end

function Widget:IsChild(parent : self)
	local p = self
	local cnt = 0
	while p and p ~= parent do
		p = p.Parent
		cnt = cnt + 1
	end

	-- print("CHECK CHILD " .. cnt .. " RES " .. ((p and p == parent) and "YES" or "NO"))
	return p and p == parent
end

function Widget:LocateWindowEx(x : number, y : number)
	--PrintRect(w.Attribs.Rect)
	if self.Attribs.Show and Contain(self.Attribs.ClippedRect, x, y) then
		local sub = self
		local depth = self.Attribs.Depth or -10000
		for k, v in self:Foreach() do
			local vv : self = v
			if vv then
				local s, d = vv:LocateWindowEx(x, y)
				-- print("TRY ..")
				-- PrintRect(v.Attribs.Rect)
				if s and d >= depth then
					sub = s 
					depth = d
				end
			end
		end

		-- print("OK contain " .. depth)
		-- PrintRect((sub or self..Attribs.Rect)
		return sub, depth
	end
end

function Widget:Print()
	PrintRect(self.Attribs.Rect)
end

function Widget:PostEvent(name : string, origin : self, ...)
	local pass = origin
	origin = origin or self

	local handler = self.Events[name]
	local handled = false
	if handler then
		handled = handler(self, origin, ...)
	end

	if not handled then
		local filter = self.Events.Filters
		if filter then
			local routine = filter[name]
			if routine then
				if routine(self, origin, ...) then
					return
				end
			end
		end

		if pass and self.Parent then
			self.Parent:PostEvent(name, origin, ...)
		end
	end
end

function Widget:Display(state : boolean, level : number?)
	level = level or 0

	if level ~= 0 then -- negetive number means inf
		level = level - 1
		for k, v in self:Foreach() do
			local s : self = v
			if s then
				s:Display(state, level)
			end
		end
	end

	self.Attribs.Show = state
	self:PostEvent(state and "OnOpen" or "OnClose", self)
end

-- Theme support 


-- Language
function Widget:LoadLanguage(lang)
	self.UIConfig.Language = lang
end

-- Themes

local function ConvertFrame(size, frame)
	-- print("FRAME " .. frame.x .. " - " .. frame.y)
	return { frame.x / size[1], 1.0 - (frame.y + frame.h) / size[2], (frame.x + frame.w) / size[1], 1.0 - frame.y / size[2] }
	-- return { 0.42, 0.3125, 0.625, 1.0 }
end

local function ConvertEdge(size, frame)
	return { (frame.edge and frame.edge.x or 20) / size[1], (frame.edge and frame.edge.y or 10) / size[2] }
end

local function Map(theme, id)
	local p = theme.Patches[id]
	return { Size = { p.frame.w, p.frame.h, p.frame.w, p.frame.h }, theme.Image, ConvertFrame(theme.Size, p.frame), ConvertEdge(theme.Size, p), 0, true, true }
end

local function ExpandTag(config, theme)
	config.Tag.Image = config.Tag.Image or {}
	config.Tag.Image.Arrow = Map(theme, "tag")
end

local function ExpandButton(config, theme)
	config.Button.Image = config.Button.Image or {}
	config.Button.Image.Hover = Map(theme, "button_hover")
	config.Button.Image.Normal = Map(theme, "button_normal")
	config.Button.Image.Pressed = Map(theme, "button_pressed")
end

local function ExpandFrame(config, theme)
	config.Frame.Image = config.Frame.Image or {}
	config.Frame.Image.Background = Map(theme, "message_background")
	config.Frame.Image.Frame = Map(theme, "message_frame")
end

local function ExpandProgress(config, theme)
	config.Progress.Image = config.Progress.Image or {}
	config.Progress.Image.Hint = Map(theme, "progress_hint")
	config.Progress.Image.Frame = Map(theme, "progress_frame")
	config.Progress.Image.Fill = Map(theme, "progress_fill")
	config.Progress.Image.Control = Map(theme, "progress_control")
end

local function ExpandSlider(config, theme)
	config.Slider.Image = config.Slider.Image or {}
	config.Slider.Image.Line = Map(theme, "slider_line")
	config.Slider.Image.Control = Map(theme, "slider_control")
end

local function ExpandSliderVertical(config, theme)
	config.SliderVertical.Image = config.SliderVertical.Image or {}
	config.SliderVertical.Image.Line = Map(theme, "slider_line_vertical")
	config.SliderVertical.Image.Control = Map(theme, "slider_control_vertical")
end

local function ExpandSwitch(config, theme)
	config.Switch.Image = config.Switch.Image or {}
	config.Switch.Image.On = Map(theme, "switch_on")
	config.Switch.Image.Off = Map(theme, "switch_off")
	config.Switch.Image.Checked = Map(theme, "checkbox_checked")
	config.Switch.Image.Unchecked = Map(theme, "checkbox_unchecked")
	config.Switch.Image.RadioChecked = Map(theme, "radio_checked")
	config.Switch.Image.RadioUnchecked = Map(theme, "radio_unchecked")
end

local function ExpandLoading(config, theme)
	config.Loading.Image = config.Loading.Image or {}
	config.Loading.Image.Frame = Map(theme, "loading_frame")
	config.Loading.Image.Fill = Map(theme, "loading_fill")
end

local function ExpandTextBox(config, theme)
	config.TextBox.Image = config.TextBox.Image or {}
	config.TextBox.Image.Field = Map(theme, "field")
	config.TextBox.Image.FieldHover = Map(theme, "field_hover")
	config.TextBox.Image.FieldRound = Map(theme, "field_round")
	config.TextBox.Image.FieldRoundHover = Map(theme, "field_round_hover")
	config.TextBox.Image.Search = Map(theme, "search")
	config.TextBox.Image.SearchHover = Map(theme, "search_hover")
end

local function Expand(config, theme)
	print("Expand theme ... ")
	config = theme.Presettings and Core.Merge(config, theme.Presettings) or config
	print("Done")

	for k, routine in ipairs({ ExpandTag, ExpandButton, ExpandFrame,
		ExpandProgress, ExpandSlider, ExpandSliderVertical, ExpandSwitch, ExpandLoading, ExpandTextBox }) do
		routine(config, theme)
	end

	return config
end

function Widget:LoadTheme(theme)
	self.UIConfig = Expand(Core.Copy(UIConfigTemplate), theme)
end


return Widget