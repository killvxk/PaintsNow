local Popup = {}
local Widget = require("Engine/UserInterface/Widget")
local Luna = require("Engine/UserInterface/Luna")

local interface Setting
	index : integer
	start : number
end

local mapDirection : { string : Setting } = {
	["LeftToRight"] = { index = 1, start = 1.5 },
	["RightToLeft"] = { index = 1, start = -1.5 },
	["TopToBottom"] = { index = 2, start = 1.5 },
	["BottomToTop"] = { index = 2, start = -1.5 },
}

local function CreatePopupHandler(open, e)
	e = e or 0.8
	return function (widget : Widget, origin : Widget, down : boolean, x : number, y : number)
		local config = widget.Attribs.PopupConfig
		local range = widget.Attribs.ScrollSize
		local option = mapDirection[config.Direction]
		assert(option)
		local pos = open and option.start or 0
		local index = option.index

		HeartVioliner.Push(Luna.Queue, 25, function ()
			local c = option
			if c then
				local start = c.start
				pos[index] = open and pos[index] * e or pos[index] + (start - pos[index]) * e
				local ret = true
				if pos[index] < 0.001 then
					pos[index] = 0
					ret = false
				end

				widget.Attribs.ScrollOffset = pos
				return ret
			end
		end)
	end
end

-- static 
local OnOpen = CreatePopupHandler(true)
local OnClose = CreatePopupHandler(false)

function Popup.New(object, config)
	object.Attribs.PopupConfig = config
	object.Events = object.Events or {}
	object.Events.Filters = object.Events.Filters or {}

	object.Events.Filters.OnOpen = OnOpen
	object.Events.Filters.OnClose = OnClose
end

return Popup