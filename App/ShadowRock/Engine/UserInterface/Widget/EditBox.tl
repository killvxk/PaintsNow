local EditBox = {}
local Widget = require("Engine/UserInterface/Widget")
local Editor = require("Engine/UserInterface/Staff/Editor")

local config = Widget.UIConfig

EditBox.Styles = {
	Search = {
		Hover = config.TextBox.Image.SearchHover,
		Normal = config.TextBox.Image.Search
	},
	Field = {
		Hover = config.TextBox.Image.FieldHover,
		Normal = config.TextBox.Image.Field
	},
	RoundField = {
		Hover = config.TextBox.Image.RoundFieldHover,
		Normal = config.TextBox.Image.RoundField
	}
}

function EditBox.New(text : string, temp : { "Hover" : any, "Normal" : any }, editable : boolean)
	local function OnFly(widget : Widget, origin : Widget, state : boolean)
		widget.Attribs.Image = state and temp.Hover or temp.Normal
	end

	local size = { 0, temp.Normal.Size[2], 0, temp.Normal.Size[2] }
	assert(size[2] ~= 0)
	local content = {
		Order = 1,
		Attribs = {
			Size = size,
			Text = text,
			Font = config.Tag.Font,	
		}
	}

	if editable then
		content = Editor.New(content)
	end

	return {
		Attribs = {
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			Image = temp.Normal,
			Size = size,
			-- Size = { 120, 40, 200, 40 },
			Padding = { 10, math.max(5, size[2] - temp.Normal.Size[2]), 30, math.max(5, (size[2] - temp.Normal.Size[2]) / 2) }
		},
		Nodes = {
			Content = content
		},	
		Events = {
			Filters = { OnFly = OnFly }
		}
	}
end


return EditBox