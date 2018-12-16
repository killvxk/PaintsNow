-- Luna.tl
-- By PaintDream (paintdream@paintdream.com)
-- 2015-6-22
--

local Luna = {}
Luna.__index = Luna

local Core = require("Engine/Core")
local Math = require("Engine/Data/Math")
local Widget = require("Engine/UserInterface/Widget")
Widget.Luna = Luna
Luna.MetaRoot = { __mode = "k" }

function Luna.HotUpdate(data : {})
end

local CreateNullWidget = function () : Widget? return nil end
Luna.Root = (setmetatable({}, Luna.MetaRoot))
Luna.Name = "Luna"
Luna.Focus = (CreateNullWidget())
Luna.MoveFocus = (CreateNullWidget())
Luna.ScreenSize = { 0.0, 0.0, 960.0, 540.0 }
local lockFocus : { integer : Widget? } = {}
Luna.LockFocus = lockFocus

function Luna.Present(widget)
	Luna.Root[widget] = widget
	return widget
end

function Luna.Locate(x : number, y : number) : Widget?
	local widget : Widget? = CreateNullWidget()
	local curDepth = -10000
	for k, v in pairs(Luna.Root) do
		if v and k.Attribs.Show then -- top window
			local w, depth = k:LocateWindowEx(x, y)
			if w and depth then
				if depth >= curDepth then
					curDepth = depth
					widget = w
				end
			end
		end
	end

	--print(" STATUS " .. (widget and "YES" or "NO"))
	return widget
end

function Luna.RemoveFocus(widget : Widget)
	local focus = Luna.Focus
	if focus == widget then
		if focus then
			if focus.Events.OnFocus then
				focus.Events.OnFocus(focus, false)
			end

			Luna.Focus = nil
		end
	end
end

function Luna.PushLockFocus(widget)
	table.insert(Luna.LockFocus, widget)
end

function Luna.PopLockFocus()
	if #lockFocus ~= 0 then
		table.remove(Luna.LockFocus)
	end
end

function Luna.UpdateFocus(focus, widget, handler)
	local change = false

	if widget ~= Luna.Focus then
		change = true
		handler(focus, false)
	end

	Luna.Focus = widget

	-- change focus control

	if change then
		handler(Luna.Focus, true)
	end

	return Luna.Focus
end

function Luna.Top()
	return pairs(Luna.Root)
end

function Luna.Pin(widget, size, pinType)
	size = size or widget.Attribs.Pinned
	pinType = pinType or widget.Attribs.PinType or ""

	assert(size)
	assert(pinType)

	local pos = {
		string.find(pinType, "left") and 0 or string.find(pinType, "right") and 2 or 1,
		string.find(pinType, "top") and 0 or string.find(pinType, "bottom") and 2 or 1
	}

	--print("POS = " .. pos[1] .. "   " .. pos[2])

	local target : any = { 0, 0, 0, 0 }
	local s : any = size
	local p : any = pos
	local cc : any = Luna.ScreenSize
	for i = 1, 4 do
		local xy = 2 - (i % 2)
		--print("SCREEN = " .. screenSize[xy] .. "  SIZE = " .. size[xy])
		target[i] = (cc[xy + 2] - s[xy]) / 2 * p[xy] + math.floor((i - 1) / 2) * s[xy]
		--print("TARGET " .. i .. " = " .. target[i])
	end


	widget.Attribs.Rect = target
	widget.Attribs.Pinned = size
	widget.Attribs.PinType = pinType

	return widget
end

function Luna.WaitForModalInput(widget : Widget)
	-- fade all other widgets
	widget:Display(true, -1)

	local fadeRatio = 0.5
	local tops : { integer : any } = {}
	for top, state in Luna.Top() do
		local v = Math.Vector(top.Attribs.BkColor) * fadeRatio
		top.Attribs.BkColor = Math.Vector(top.Attribs.BkColor) * fadeRatio
		table.insert(tops, top)
	end

	widget.Attribs.BkColor = Math.Vector(widget.Attribs.BkColor) / fadeRatio

	local instance = Core.Instance()
	local onclose = widget.Events.OnClose

	widget.Events.OnClose = function (self, option)
		print("CLOSE WIDGET")
		Core.Post(instance, option)
		if (onclose) then
			onclose(self, option)
		end
	end

	Luna.PushLockFocus(widget)

	return Core.Listen(instance, function (defer, selected)
		print("Dialog INPUT Captured")
		-- restore close handler
		widget.Events.OnClose = onclose
		widget.Attribs.BkColor = Math.Vector(widget.Attribs.BkColor) * fadeRatio

		for i, top in ipairs(tops) do
			top.Attribs.BkColor = Math.Vector(top.Attribs.BkColor) / fadeRatio
		end

		Luna.PopLockFocus()

		-- Luna.Display(widget, false, -1)
		return selected
	end)()
end


-- Register event handlers
local function MouseEventDispatcher(down : boolean, left : boolean, move : boolean, wheel : boolean, x : number, y : number)
	local widget : Widget? = Luna.Locate(x, y)
	local lockedWidget = Luna.LockFocus[#Luna.LockFocus] or nil

	-- block
	if lockedWidget and widget and not widget:IsChild(lockedWidget) then
		widget = lockedWidget	
	end

	Luna.MoveFocus = Luna.UpdateFocus(Luna.MoveFocus, widget, function (w : Widget?, state : boolean)
		if w then
			w:PostEvent("OnFly", w, state)
		end
	end)

	if widget then
		if down then
			Luna.Focus = Luna.UpdateFocus(Luna.Focus, widget, function (w : Widget, state : boolean)
				if w then
					w:PostEvent("OnFocus", w, state)
				end
			end)
		end

		widget:PostEvent(move and "OnMove" or (wheel and "OnWheel" or "OnClick"), widget, down, x, y)
	end

	return LunaWidget.NotifyMouseEvent(down, left, move, wheel, x, y)
end

local function KeyboardEventDispatcher(code : integer, key : string, down : boolean)
	local focus = Luna.Focus
	if focus then
		focus:PostEvent("OnKeyboard", focus, key, down)
	end

	return LunaWidget.NotifyKeyboardEvent(code)
end

local function ResizeEventDispatcher(size : float2)
	local x = size[1]
	local y = size[2]
	local newScreenSize = { 0.0, 0.0, x, y }
	for widget in Luna.Top() do
		local attribs = widget.Attribs
		if attribs.AlwaysMatchScreen then
			attribs.Rect = newScreenSize
		elseif attribs.PreserveLogicPosition then
			-- Get oringal rect and transform it to new position
			local original = attribs.Rect
			original[1] = newScreenSize[1] * original[1] / Luna.ScreenSize[1]
			original[2] = newScreenSize[2] * original[2] / Luna.ScreenSize[2]
			original[3] = newScreenSize[1] * original[3] / Luna.ScreenSize[1]
			original[4] = newScreenSize[2] * original[4] / Luna.ScreenSize[2]

			attribs.Rect = original
		end
	end

	Luna.ScreenSize = newScreenSize
end

Luna.EventDispatcherMouse = (IceSpring.CreateDispatch("Mouse", MouseEventDispatcher))
Luna.EventDispatcherKeyboard = (IceSpring.CreateDispatch("Keyboard", KeyboardEventDispatcher))
Luna.EventDispatcherResize = (IceSpring.CreateDispatch("Resize", ResizeEventDispatcher))

local size : float2 = System.GetScreenSize()
Luna.ScreenSize = { 0.0, 0.0, size[1], size[2] }
ResizeEventDispatcher({size[1], size[2]})

-- Create standard timer
Luna.Clock = (HeartVioliner.CreateClock(50, 0))
Luna.Queue = (HeartVioliner.CreateQueue())
HeartVioliner.Attach(Luna.Clock, Luna.Queue)
HeartVioliner.Listen(Luna.Queue, function (v : number, obj : () -> (boolean))
	if obj and obj() then
		HeartVioliner.Push(Luna.Queue, v + 25, obj)
	end
end)

HeartVioliner.Start(Luna.Clock)

return Luna