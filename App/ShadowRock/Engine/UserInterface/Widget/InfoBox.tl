local InfoBox = {}

function InfoBox.New(text : string, icon)
	return {
		Attribs = {
			Layout = "horizontal",
			Float = true,
		},
		Icon = icon and {
			Attribs = {
				Image = icon,
				Weight = 1,
			}
		} or nil,
		Info = {
			Attribs = {
				Weight = 4,
				Text = text,
			}
		}	
	}
end

function CreateFlyHandler(infoBox, size)
	-- fly
	return function (widget, origin, state)
		if state then
			local rect = origin.Attribs.Rect
			infoBox.Rect = { rect[1], rect[4], rect[1] + size[1], rect[4] + size[2] }
			origin.Nodes.InfoBox = infoBox
		else
			origin.Nodes.InfoBox = nil	
		end
	end
end


return InfoBox