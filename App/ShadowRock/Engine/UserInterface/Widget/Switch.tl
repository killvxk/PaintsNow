local Switch = {}
local Core = require("Engine/Core")
local Widget = require("Engine/UserInterface/Widget")

local config = Widget.UIConfig
local plainFont = config.Tag.Font
Switch.Styles = {
	OnOff = {
		Empty = config.Switch.Image.On,
		Checked = config.Switch.Image.Off,
	},
	Radio = {
		Empty = config.Switch.Image.RadioUnchecked,
		Checked = config.Switch.Image.RadioChecked,
	},	
	Check = {
		Empty = config.Switch.Image.Unchecked,
		Checked = config.Switch.Image.Checked,
	},
}

function Switch.New(label, style : { "Empty" : any, "Checked": any }, on : boolean)
	style = style or Switch.Styles.Check
	local size = math.max(label.Attribs.Size and label.Attribs.Size[2] or 0, style.Empty.Size[2]) + 5

	local function UpdateStatus(widget)
		widget.Attribs.Image = widget.Attribs.On and style.Checked or style.Empty
	end

	local function OnClick(widget : Widget, origin : Widget, down : boolean)
		if down then
			-- radio button?
			if widget.Attribs.Radio then
				local p = widget.Parent
				if p then
					p = p.Parent
					if p then
						for k, v in pairs(p.Nodes) do
							local state = v.Nodes.State
							if state and state.Attribs.Radio then
								state.Attribs.On = false
								UpdateStatus(state)
							end
						end
					end
				end
				widget.Attribs.On = true
			else
				widget.Attribs.On = not widget.Attribs.On
			end

			UpdateStatus(widget)
		end
	end

	local switch = {
		Attribs = { Weight = 1, Layout = "horizontal", Size = { 0, size, 0, size } },
		Nodes = {
			State = {
				Order = 1,
				Attribs = {
					Weight = 1,
					BkColor = { 1.0, 1.0, 1.0, 1.0 },
					On = on,
					Radio = style == Switch.Styles.Radio,
					Size = style.Empty.Size,
					Padding = { 3, 1, 0, 0 },
				},
				Events = {
					OnClick = OnClick
				}
			},
			Label = Core.Merge(label, { Order = 2, Attribs = { Weight = 1, Font = plainFont, Padding = { 3, 0, 0, 0 } } }),
		}
	}

	UpdateStatus(switch.Nodes.State)
	return switch
end


return Switch