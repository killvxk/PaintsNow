local CheckBox = { }
local Widget = require("Engine/UserInterface/Widget")
local config = Widget.UIConfig

local function OnClick(widget : Widget, down, x, y)
	if down then
		return
	end

	widget.Attribs.IsChecked = not widget.Attribs.IsChecked
	local mark = widget.Nodes.Mark
	if mark then
		mark.Attribs.Text = widget.Attribs.IsChecked and "<align=center>*" or ""
	end

	return true
end

function CheckBox.New(object : Widget)
	object.Nodes.Mark = Widget.New({
		Order = 1,
		Attribs = { Weight = 1, Size = { 0, 0, 35, 0 }, Padding = { 5, 0, 0, 0 } },
	})
	
	object.Nodes.Content = Widget.New({
		Order = 2,
		Attribs = { Weight = 1, Text = object.Attribs.Text }
	})

	object.Attribs.Layout = "horizontal" 
	object.Attribs.Text = "" -- Clear Text

	local orgclick = object.Events.OnClick
	object.Events.OnClick = function (...)
		if orgclick then
			if orgclick(...) then
				return OnClick(...)
			end
		else
			return OnClick(...)
		end
	end

	return object
end

return CheckBox
