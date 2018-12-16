local ComboBox = {}
local Widget = require("Engine/UserInterface/Widget")
local Dropdown = require("Engine/UserInterface/Staff/Dropdown")

function ComboBox.New(inputList : Widget, count : integer)
	inputList.Attribs.Weight = count
	inputList = Dropdown.New(inputList)

	local k, v = next(inputList)
	return {
		Attribs = {
			Layout = "vertical"
		},
		Nodes = {
			Selected = {
				Attribs = {
					Weight = 1,
					Text = v and v.Content.Attribs.Text or "<Empty>"
				},
				Events = {
					Filter = {
						OnClick = function (widget : Widget, origin : Widget, down : boolean, x : number, y : number)
							local parent = widget.Parent
							if parent then
								local list = parent.Nodes.List
								if list then
									return list:PostEvent("OnClick", list, down, x, y)
								end
							end
						end
					}
				}
			},
			List = inputList
		}
	}
end

return ComboBox