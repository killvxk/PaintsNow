local Editor = {}
local Widget = require("Engine/UserInterface/Widget")
local Luna = require("Engine/UserInterface/Luna")

local function Swap(t : float2)
	local g = t[1]
	t[1] = t[2]
	t[2] = g
end

local function UpdateRange(widget : Widget, w : float4)
	if widget.Attribs.ShiftDown then
		if w[3] < widget.Attribs.SelectRange[1] then
			widget.Attribs.SelectRange[1] = w[3]
			widget.Attribs.SelectRowCol[1] = { w[1], w[2] }
		end

		if w[3] > widget.Attribs.SelectRange[2] then
			widget.Attribs.SelectRange[2] = w[3]
			widget.Attribs.SelectRowCol[2] = { w[1], w[2] }
		end
	elseif widget.Attribs.ClickDown then
		widget.Attribs.SelectRange[2] = w[3]
		widget.Attribs.SelectRowCol[2] = { w[1], w[2] }
		--[[
		if widget.Attribs.SelectRange[1] > widget.Attribs.SelectRange[2] then
			-- swap
			Swap(widget.Attribs.SelectRange)
			Swap(widget.Attribs.SelectRowCol)
		end
		]]
	else
		widget.Attribs.SelectRange[1] = w[3]
		widget.Attribs.SelectRange[2] = w[3]
		widget.Attribs.SelectRowCol[1] = { w[1], w[2] }
		widget.Attribs.SelectRowCol[2] = { w[1], w[2] }
	end
end

local function ApplySelection(widget : Widget)
	-- Modify text color
	-- reverse color
	-- print("Range " .. widget.Attribs.SelectRange[1] .. " To " .. widget.Attribs.SelectRange[2])
	LunaWidget.SetWidgetTextRangeOption(widget.Handle, widget.Attribs.SelectRange, {
		ReverseColor = false,
		Color = { 1, 1, 0, 1 }
	})
end

local function UpdateCursor(widget : Widget)
	LunaWidget.SetWidgetTextCursorOption(widget.Handle, widget.Attribs.SelectRange and widget.Attribs.SelectRange[2] or 0, {
		Color = { 1, 1, 1, 1 },
		Char = widget.Attribs.CursorChar or '|',
		ReverseColor = false
	})
end

local function OnClick(widget : Widget, origin : Widget, down : boolean, x : number, y : number)
	if down then
		widget.Attribs.DownPos = { x, y }
		-- clear range selection
		local w = LunaWidget.LocateWidgetText(widget.Handle, { x, y }, false)
		UpdateRange(widget, w)
		ApplySelection(widget)
		UpdateCursor(widget)
		-- print("Locate " .. w[1] .. ", " .. w[2] .. " [" .. w[3] .. "]")
	else
		if widget.Attribs.SelectRange[1] > widget.Attribs.SelectRange[2] then
			-- swap
			-- print("SWAP")
			Swap(widget.Attribs.SelectRange)
			Swap(widget.Attribs.SelectRowCol)
		end
	end

	widget.Attribs.ClickDown = down
	return true
end


local function OnMove(widget : Widget, origin : Widget, down : boolean, x : number, y : number)
	if widget.Attribs.ClickDown then
		-- select text area
		local index = LunaWidget.LocateWidgetText(widget.Handle, { x, y }, false)
		-- print("INDEX " .. index[3])
		ApplySelection(widget)
		UpdateRange(widget, index)
		UpdateCursor(widget)
	end

	return true
end

local moveCommand : { string : float2 } = {
	["LEFT"] = { 0, -1 },
	["RIGHT"] = { 0, 1 },
	["UP"] = { -1, 0 },
	["DOWN"] = { 1, 0 }
}

local function HandleMove(widget : Widget, origin : Widget, delta : float2)
	local org : float2 = widget.Attribs.SelectRowCol[1] or { 0, 0 }
	local n : float4 = (LunaWidget.LocateWidgetText(widget.Handle, { org[1] + delta[1], org[2] + delta[2] }, true))
	UpdateRange(widget, n)
	UpdateCursor(widget)
	return true
end

local function OnKeyboard(widget : Widget, origin : Widget, key : string, down : boolean)
	-- if got selected. replace selection
	if key == "SHIFT" then
		widget.Attribs.ShiftDown = down
		return false
	end


	if down and #widget.Attribs.SelectRange == 2 then
		local text = widget.Attribs.Text
		if key == "ENTER" then
			key = "\n"
		elseif key == "ESC" then
			Luna.RemoveFocus(widget)
			return true
		elseif key == "BACKSPACE" then
			if widget.Attribs.SelectRange[2] == widget.Attribs.SelectRange[1] and widget.Attribs.SelectRange[1] ~= 0 then	
				widget.Attribs.SelectRange[1] = widget.Attribs.SelectRange[1] - 1
			end

			key = ""
		elseif key == "DELETE" then
			if widget.Attribs.SelectRange[2] == widget.Attribs.SelectRange[1] and widget.Attribs.SelectRange[2] ~= #text then	
				widget.Attribs.SelectRange[2] = widget.Attribs.SelectRange[2] + 1
			end
			key = ""
		end

		if string.len(key) <= 1 then
			local target = string.sub(text, 1, widget.Attribs.SelectRange[1]) .. key .. string.sub(text, 1 + widget.Attribs.SelectRange[2])
			local pos = widget.Attribs.SelectRange[1] + string.len(key)
			widget.Attribs.SelectRange[1] = pos
			widget.Attribs.SelectRange[2] = pos
			widget.Attribs.Text = target

			UpdateRange(widget, { 0, pos, pos, 0})
			ApplySelection(widget)
			UpdateCursor(widget)
			-- Luna.RemoveFocus(widget)
			return true
		else
			local cmd = moveCommand[key]
			if cmd then
				return HandleMove(widget, origin, cmd)
			end
		end
	end
end

local function OnFocus(widget : Widget, origin : Widget, enable : boolean)
	widget.Attribs.CursorChar = enable and '|' or ''
	UpdateCursor(widget)
end

function Editor.New(object : { any : any })
	object.Attribs.CursorChar = '|'
	object.Attribs.Padding = { 5, 10, 5, 10 }
	object.Attribs.SelectRange = { 0, 0 }
	object.Attribs.SelectRowCol = { 0, 0 }
	object.Events = object.Events or {}
	object.Events.OnClick = object.Events.OnClick or OnClick
	object.Events.OnMove = object.Events.OnMove or OnMove
	object.Events.OnKeyboard = object.Events.OnKeyboard or OnKeyboard
	object.Events.OnFocus = object.Events.OnFocus or OnFocus

	return object
end

return Editor
