local Button = {}
local Widget = require("Engine/UserInterface/Widget")
local config = Widget.UIConfig

Button.Styles = { OKCancel = 1 }

local function OnFly(widget : Widget, origin : Widget, state : boolean)
	widget.Attribs.Image = state and config.Button.Image.Hover or config.Button.Image.Normal
end

local function OnClick(widget : Widget, origin : Widget, down : boolean)
	widget.Attribs.PushedDown = down
	widget.Attribs.Image = down and config.Button.Image.Pressed or config.Button.Image.Normal
end

function Button.New(tag : string)
	local text = config.Language[tag]
	assert(text, "Missing language support for " .. tag)

	if config.Button.TextAlignCenter then
		text = "<align=center>" .. text	
	end

	assert(config.Button.Image)
	local height = config.Button.Font[2] + 10

	return {
		Attribs = {
			Tag = tag,
			Text = text,
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			Padding = { 20, 0, 20, 0 },
			Size = { 80, height, 150, height },
			Font = config.Button.Font,
			Image = config.Button.Image.Normal
		},
		Events = {
			Filters = {
				OnFly = OnFly,
				OnClick = OnClick
			}
		}
	 }
end


return Button