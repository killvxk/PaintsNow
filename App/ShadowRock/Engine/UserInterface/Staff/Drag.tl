local Drag = {}
local Widget = require("Engine/UserInterface/Widget")
local Luna = require("Engine/UserInterface/Luna")

local interface Drag
	x, y, ox, oy : number
	object : Widget
end

local function FixOffset(self : Widget, drag : Drag, fix : string, x : number, y : number)
	local maxRect = self.Attribs.Rect
	local rect = drag.object.Attribs.Size

	local dx = x - drag.x
	local dy = y - drag.y
	if fix == "vertical" then
		dx = 0
	elseif fix == "horizontal" then
		dy = 0
	end

	--print("D = " .. dx .. ", " .. dy)
	local width = maxRect[3] - maxRect[1] - rect[1]
	local height = maxRect[4] - maxRect[2] - rect[2]
	dx = math.min(math.max(dx + drag.ox, 0), width)
	dy = math.min(math.max(dy + drag.oy, 0), height)

	return dx, dy, width, height
end

function Drag.SetPositionByRatio(widget : Widget, dragObject : Widget, ratio : float2)
	local maxRect = widget.Attribs.Rect
	local rect = dragObject.Attribs.Size
	assert(maxRect, "Invalid maxrect")
	assert(rect, "Invalied rect")
	local width = maxRect[3] - maxRect[1] - rect[1]
	local height = maxRect[4] - maxRect[2] - rect[2]

	dragObject.Attribs.ScrollOffset = { - math.max(width * ratio[1], 0), math.max(height * ratio[2], 0) }
end

function Drag.New(object, filter : (Widget?) -> (Widget?), fix : string)
	local function CheckAndSet(widget : Widget, origin : Widget, down : boolean, x : number, y : number)
		if not down then
			return
		end

		local drag : Drag? = widget.Attribs.CurrentDrag
		if drag then
			-- get offset
			local nx, ny, width, height = FixOffset(widget, drag, fix, x, y)
			drag.object.Attribs.ScrollOffset = { - nx, - ny }
			-- print("SET " .. nx .. ", " .. ny)
			-- print("DRAG STATE" .. (self.Events.OnDrag and "YES" or "NO"))
			widget:PostEvent("OnDrag", widget, down, drag.x, drag.y, x, y, width == 0 and 0 or nx / width, height == 0 and 0 or -ny / height)
		end
	end

	object.Events.Filters.OnClick = function (widget, origin, down, x, y)
		if (down) then
			origin = filter(widget, origin)
			if origin then
				local offset = origin.Attribs.ScrollOffset
				Luna.PushLockFocus(origin)
				object.Attribs.CurrentDrag = { object = origin, x = x, y = y, ox = offset[1], oy = offset[2] }
			end	
		else
			Luna.PopLockFocus()
			CheckAndSet(widget, origin, down, x, y)	
			object.Attribs.CurrentDrag = nil
		end
	end

	object.Events.Filters.OnMove = CheckAndSet

	return object
end

return Drag