local Scroll = {}
local Widget = require("Engine/UserInterface/Widget")
local Luna = require("Engine/UserInterface/Luna")

local function GetRange(widget : Widget)
	local range = widget.Attribs.ScrollSize
	local size = widget.Attribs.Rect
	for i = 1, 2 do
		range[i] = range[i] - (size[i + 2] - size[i])
	end

	return range
end

function Scroll.SetScrollOffsetByRatio(widget : Widget, ratio : float2)
	local pos = widget.Attribs.ScrollOffset
	local range = GetRange(widget)
	pos[1] = range[1] * ratio[1]
	pos[2] = -range[2] * ratio[2]

	widget.Attribs.ScrollOffset = pos
end

local function CreateWheelHandler(vertical : boolean, vec : number, acc : number)
	return function (widget, origin, dir, x, y)
		if widget.Attribs.ScrollVelocity then
			widget.Attribs.ScrollVelocity = vec
			widget.Attribs.ScrollDirection = dir
			return
		end

		widget.Attribs.ScrollVelocity = vec
		local range = GetRange(widget)
		local index = vertical and 2 or 1

		-- print("NEW")
		
		HeartVioliner.Push(Luna.Queue, 25, function ()
			local v = widget.Attribs.ScrollVelocity
			v = v - acc
			local dir = widget.Attribs.ScrollDirection
			if v > 0 then
				widget.Attribs.ScrollVelocity = v
				-- print("TRIG" .. v)
				local pos = widget.Attribs.ScrollOffset
				pos[2] = -pos[2]
				-- print("Range " .. range[index])
				pos[index] = math.max(math.min(pos[index] + (dir and -v or v), range[index]), 0)
				pos[2] = -pos[2]
				widget.Attribs.ScrollOffset = pos
				-- print("HI" .. pos[index])
				local ratio = range[index] == 0 and 0 or pos[index] / range[index]
				widget:PostEvent("OnScroll", origin, vertical, index == 2 and -ratio or ratio)
				-- print("CONTINUE")
				return true
			else
				widget.Attribs.ScrollVelocity = nil
				widget.Attribs.ScrollDirection = nil
				-- print("END")
			end
		end)
	end
end

function Scroll.New(object : Widget, vertical : boolean, vec : number, acc : number)
	object.Events.OnWheel = CreateWheelHandler(vertical, vec, acc)
	return object
end

return Scroll