local Progress = {}
local Core = require("Engine/Core")
local Widget = require("Engine/UserInterface/Widget")
local Luna = require("Engine/UserInterface/Luna")

local config = Widget.UIConfig
local plainFont = config.Tag.Font

function Progress.New(hasIndicator : boolean)
	local height = (hasIndicator and config.Progress.Image.Hint.Size[2] or 0) + config.Progress.Image.Frame.Size[2]
	local padding = config.Progress.Image.Hint.Size[1] / 2 - 10
	return {
		Attribs = {
			Layout = "vertical",
			Size = { 0, height, 0, height },
			InnerPercent = 0,
		},
		Nodes = {
			Indicator = hasIndicator and {
				Order = 1,
				Nodes = {
					Control = {
						Order = 1,
						Attribs = {
							Weight = 1,
							BkColor = { 1.0, 1.0, 1.0, 1.0 },
							Image = config.Progress.Image.Hint,
							Size = config.Progress.Image.Hint.Size,
							Font = plainFont,
							Text = "<align=center>0 %"
						}
					}
				}
			} or nil,
			Progress = {
				Order = 2,
				Attribs = { Padding = { hasIndicator and padding or 0, hasIndicator and 20 or 0, hasIndicator and padding or 0, 0 } },
				Nodes = {
					Bar = {
						Order = 1,
						Attribs = {
							BkColor = { 1.0, 1.0, 1.0, 1.0 },
							-- BkImage = config.Frame.Image.Frame,
							BkImage = config.Progress.Image.Frame,
							-- Size = config.Progress.Image.Frame.Size,
						}
					}
				}
			}			
		},
		Events = {
			Filters = {
				OnAttrib = {
					Value = function (write, object, key, value)
						if write then
							-- print("WRITE!")
							local target = object.Attribs.TargetPercent
							object.Attribs.TargetPercent = value
							if not target then
								HeartVioliner.Push(Luna.Queue, 25, function ()
									if object.Attribs.TargetPercent == nil then
										return false
									end

									local diff = object.Attribs.TargetPercent - object.Attribs.InnerPercent
									if diff < 0.005 then
										object.Attribs.InnerPercent = object.Attribs.TargetPercent
										object.Attribs.TargetPercent = nil
									else
										object.Attribs.InnerPercent = object.Attribs.InnerPercent + (object.Attribs.TargetPercent - object.Attribs.InnerPercent) * 0.2
									end

									local v = object.Attribs.InnerPercent
									local indicator = object.Indicator
									if indicator then
										indicator.Nodes.Control.Attribs.Text = string.format("<align=center>%.1f%%", (v * 100))
										local rect = object.Progress.Attribs.Rect
										local width = rect[3] - rect[1]
										if width == 0 then
											width = config.Progress.Image.Frame.Size[1] - padding
										end
										-- print("RECT : " .. rect[1] .. ", " .. rect[2] .. ", " .. rect[3] .. ", " .. rect[4])
										indicator.Attribs.Padding = { (width - padding) * v, 0, 0, 0 }
									end

									object.Nodes.Progress.Nodes.Bar.Attribs.Image = Core.Merge(Core.Copy(config.Progress.Image.Fill), { [7] = { 0, 0, 0.05 + v * 0.9, 1 } })

									return object.Attribs.TargetPercent ~= nil
								end)
							end
						else
							return object.Attribs.InnerPercent or 0
						end
					end
				}
			}
		}
	}
end


return Progress
