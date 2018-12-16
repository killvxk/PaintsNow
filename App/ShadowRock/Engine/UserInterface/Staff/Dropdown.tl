local Dropdown = {}
local Widget = require("Engine/UserInterface/Widget")

function Dropdown.New(object : Widget)
	object.Attribs.dropDownCollapsed = false
	object.Events.Filters.OnClick = function (widget : Widget, origin : Widget, down : boolean, x : number, y : number)
		local status = origin.Attribs.dropDownCollapsed
		origin.Attribs.dropDownCollapsed = not status
		for key, node in pairs(origin.Nodes) do
			node:Display(status)
		end
	end

	return object
end

return Dropdown