local Tag = {}
local Widget = require("Engine/UserInterface/Widget")

local config = Widget.UIConfig

function Tag.New(tag : string)
	return {
		Attribs = {
			Text = tag,
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			Size = config.Tag.Image.Arrow.Size,
			Image = config.Tag.Image.Arrow,
			Padding = { 25, 5, 5, 0 }
		},
		Events = {}
	}
end

return Tag