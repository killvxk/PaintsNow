local Dialog = { __index = _ENV }
local Core = require("Engine/Core")
local Luna = require("Engine/UserInterface/Luna")
local Widget = require("Engine/UserInterface/Widget")
local Editor = require("Engine/UserInterface/Staff/Editor")
local Button = require("Engine/UserInterface/Widget/Button")

-- Import some constants
local config = Widget.UIConfig
local titleFont = config.Title.Font
local plainFont = config.Plain.Font
local margin = config.Border.Margin

local function ForwardCommand(object : Widget, widget : Widget, down : boolean)
	-- close dialog
	if not down then
		local tag = widget.Attribs.Tag
		-- print("Forward Command")
		if tag then
			-- return object:Display(false, -1)
		end
	end
end


Dialog.Styles = {
	OKCancel = {
		OK = Core.Merge(Button.New("OK"), { Order = 1 }),
		Cancel = Core.Merge(Button.New("Cancel"), { Order = 2 }),
	},
	YesNo = {
		Yes = Core.Merge(Button.New("Yes"), { Order = 1 }),
		No = Core.Merge(Button.New("No"), { Order = 2 }),
	},
	OKOnly = {
		OK = Core.Merge(Button.New("OK"), { Order = 1 })
	},
	LoginRegister = {
		Yes = Core.Merge(Button.New("Login"), { Order = 1 }),
		No = Core.Merge(Button.New("Register"), { Order = 2 }),
	}
}

function Dialog.New(title : string, text : string, buttonList, inputList) : { any : any }
	return {
		Attribs = {
			Weight = 1,
			Layout = "vertical",
			BkColor = { 1.0, 1.0, 1.0, 1.0 },
			Image = config.Frame.Image.Background,
			BkImage = config.Frame.Image.Frame,
		},
		Nodes = {
			Title = {
				Order = 1,
				Attribs = { Weight = 1, Text = title, Font = titleFont, Padding = { 20, 5, 20, 5 } },
			},
			Text = Editor.New({
				Order = 2,
				Attribs = { Weight = 1, Text = text, Font = plainFont, Padding = { 20, 5, 20, 5 } },
			}),
			Inputs = inputList and Core.Merge(inputList, { Order = 3, Attribs = { Weight = 5, Padding = { 20, 5, 20, 5} } }) or nil,
			Buttons = {
				Order = 4,
				Attribs = { Weight = 1, Font = plainFont, Layout = "horizontal", Padding = { 20, 5, 20, 5 } },
				Nodes = buttonList or Core.Copy(Button.Styles.OKCancel),
			}
		},
		Events = {
			Filters = { OnClick = ForwardCommand }
		}
	}
end

return Dialog