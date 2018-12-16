local Slider = {}
local Core = require("Engine/Core")
local Widget = require("Engine/UserInterface/Widget")
local Drag = require("Engine/UserInterface/Staff/Drag")
local config = Widget.UIConfig

function Slider.SetSliderPositionByRatio(widget, vertical, ratio)
	widget.Attribs.Value = ratio
	local control = widget.Nodes.Control
	if control then
		local x : number = vertical and 0 or (ratio or widget.Attribs.InitValue)
		local y : number = vertical and (ratio or widget.Attribs.InitValue) or 0 
		Drag.SetPositionByRatio(widget, control, { x, y })
	end
end

function Slider.New(vertical, initValue)
	assert(config.Slider.Image)
	local slider = vertical and config.SliderVertical or config.Slider
	local size : { integer : number } = Core.Copy(slider.Image.Control.Size)
	local sizeLine : { integer : number } = Core.Copy(slider.Image.Line.Size)
	local offset = vertical and 1 or 0

	size[1 + offset] = 0
	size[3 + offset] = 0
	sizeLine[1 + offset] = 0
	sizeLine[3 + offset] = 0

	local function filter(widget, origin)
		return widget.Nodes.Control
	end

	local lhs : number = size[2 + offset] or 0
	local rhs : number = sizeLine[2 + offset] or 0
	local pad =  (lhs - rhs) / 2 + 1

	return Drag.New({
		Attribs = {
			Layout = "horizontal",	
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			Size = size,
			Value = 0.0,
			InitValue = initValue or 0.0,
		},
		Nodes = {
			Line = {
				Order = 1,
				Attribs = {
					Padding = { pad, 0, 0, 0 },
					Size = size,	
					Float = true,
				},
				Nodes = {
					Inner = {
						Order = 1,
						Attribs = {
							Size = sizeLine,	
							BkColor = { 1.0, 1.0, 1.0, 1.0 },
							Image = slider.Image.Line,
						}
					}
				}
			},
			Control = {
				Order = 2,
				Attribs = {
					BkColor = { 1.0, 1.0, 1.0, 1.0 },
					Size = slider.Image.Control.Size,
					Image = slider.Image.Control,
					Draggable = true
				}
			}
		},
		Events = {
			Filters = {	
				OnDrag = function (widget : Widget, origin : Widget, down : boolean, tx : number, ty : number, x : number, y : number, rx : number, ry : number)
					-- Update Content
					widget:PostEvent("OnSlider", widget, vertical, vertical and -ry or rx)
				end,
				OnOpen = function (widget : Widget)
					Slider.SetSliderPositionByRatio(widget, vertical, initValue or 0.0)
				end
			}
		}
	}, function (t) return t.Nodes.Control end, vertical and "vertical" or "horizontal")
end

return Slider