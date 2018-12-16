local Loading = {}
local Core = require("Engine/Core")
local Luna = require("Engine/UserInterface/Luna")
local Widget = require("Engine/UserInterface/Widget")
local config = Widget.UIConfig
local plainFont = config.Tag.Font

function Loading.New()
	local image = Core.Copy(config.Loading.Image.Fill)

	return {
		Attribs = {
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			BkImage = config.Loading.Image.Frame,
			Image = image,
			Size = config.Loading.Image.Frame.Size,
		},
		Events = {
			Filters = {
				OnAttrib = {
					Run = function (write, object, key, value)
						if write then
							if value and not object.Attribs.InnerRun then
								HeartVioliner.Push(Luna.Queue, 25, function (v)
									image[4] = ((image[4] or 0) + 10) % 360
									object.Attribs.Image = image
									return object.Attribs.InnerRun
								end)
							end
							object.Attribs.InnerRun = value
						end
					end
				},
				OnClose = function (object)
					object.Attribs.InnerRun = false	
				end
			}
		}
	}
end


return Loading