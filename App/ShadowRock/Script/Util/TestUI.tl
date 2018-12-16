local Core = require("Engine/Core")
local Widget = require("Engine/UserInterface/Widget")
Widget:LoadTheme(require("Script/Theme/PurpleLove"))
local Switch = require("Engine/UserInterface/Widget/Switch")
local EditBox = require("Engine/UserInterface/Widget/EditBox")
local ListBox = require("Engine/UserInterface/Widget/ListBox")
local Loading = require("Engine/UserInterface/Widget/Loading")
local Button = require("Engine/UserInterface/Widget/Button")
local Progress = require("Engine/UserInterface/Widget/Progress")
local TreeBox = require("Engine/UserInterface/Widget/TreeBox")
local Tag = require("Engine/UserInterface/Widget/Tag")
local Dialog = require("Engine/UserInterface/Widget/Dialog")

local Luna = require("Engine/UserInterface/Luna")


local function TestClass()
	local SpaceComponent = require("Runtime/Class/SpaceComponent")
	local ComputeComponent = require("Runtime/Class/ComputeComponent")

	local rootEntity = MythForest.CreateEntity()
	MythForest.QueueRoutine(rootEntity, function ()
		local spaceComponent = SpaceComponent.Create()
		MythForest.AddEntityComponent(rootEntity, spaceComponent)

		local entity = MythForest.CreateEntity()
		spaceComponent:InsertEntity(entity)
		local computeComponent = ComputeComponent.Create()
		local computeRoutine = computeComponent:Load([[
			local args = { ... }
			print('Hello, ComputeComponent!' .. tostring(args[1]))
			SysCall(args[1][1], { 'Callback' })
		]])

		computeComponent:Call(computeRoutine, { function (param : { string }) print("Message: " .. (param[1] or "MISSING")) end })
	end)
end

local function MainProc()
	TestClass()
	print("Loading UI ... ")
	local inputs = {
		Attribs = { Weight = 3, Layout = "horizontal" },	
		Nodes = {
			left = {
				Order = 1,
				Attribs = {
					Weight = 1,
					Layout = "vertical",
				},
				Nodes = {
					first = Core.Merge(Switch.New(Tag.New("<align=center>Sample switch on"), Switch.Styles.OnOff, true), { Order = 1 }),
					second = Core.Merge(Switch.New(Tag.New("<align=center>Sample switch off"), Switch.Styles.OnOff, false), { Order = 2 }),
					third = Core.Merge(EditBox.New("Input text HERE ... ", EditBox.Styles.Search, true), { Order = 3 }),
					radiobox = {
						Order = 4,
						Attribs = { Layout = "horizontal" },
						Nodes = {
							loading = Core.Merge(Loading.New(), { Order = 1 }),
							radone = Core.Merge(Switch.New({ Attribs = { Text = "Sample Radio 1" } }, Switch.Styles.Radio, false), { Order = 2 }),
							radtwo = Core.Merge(Switch.New({ Attribs = { Text = "Sample Radio 2" } }, Switch.Styles.Radio, false), { Order = 3 }),
						}
					},
					tester = {
						Order = 5,
						Attribs = { Layout = "vertical", Font = Widget.UIConfig.Plain.Font, FitContent = true },
						Nodes = {
							top = {
								Order = 1,
								Attribs = { Layout = "horizontal", FitContent = true },
								Nodes = {
									left = Core.Merge(Button.New("ABCDEFGH"), { Order = 1 }),
									right = Core.Merge(Button.New("OPQRSTUVWXYZ12345"), { Order = 2 }),
								}
							},
							bottom = {
								Order = 2,
								Attribs = { Layout = "horizontal", FitContent = true },
								Nodes = {
									left = Core.Merge(Button.New("12345677"), { Order = 1 }),
									right = Core.Merge(Button.New("12934788"), { Order = 2 }),
								}
							}
						}
					},
					progress = Core.Merge(Progress.New(true), { Order = 6 })
				}
			},
			right = {
				Order = 2,
				Attribs = {
					Weight = 1,
					Layout = "vertical"
				},
				Nodes = {
					listbox = Core.Merge(TreeBox.New(ListBox.New({ Nodes = {
						{ Order = 1, Attribs = { Text = "First Blood!" } },
						{ Order = 2, Attribs = { Text = "Double Kill!!" } },
						{ Order = 3, Attribs = { Text = "Killing Spree!Triple Kill!!!" } },
						--[[
						{ Order = 4, Attribs = { Text = "Dominating!UltraKill!!!" } },
						{ Order = 5, Attribs = { Text = "Mega Kill!Rampage!!!" } },
						{ Order = 6, Attribs = { Text = "Unstoppable!Rampage!!!" } },
						{ Order = 7, Attribs = { Text = "Wicked Sick!Rampage!!!" } },
						]]
						{ Order = 8, Attribs = { Text = "Mmmmmmonster Kill!Rampage!!!" },
							Nodes = {
								{ Order = 1, Attribs = { Text = "Monster 1" } },
								{ Order = 2, Attribs = { Text = "Monster 2" },
									Nodes = {
										{ Order = 1, Attribs = { Text = "Sub Monster n" } },
										{ Order = 2, Attribs = { Text = "Sub Monster n" } },
										{ Order = 3, Attribs = { Text = "Sub Monster n" } },	
										{ Order = 4, Attribs = { Text = "Sub Monster n" } }	
									}
								},
								{ Order = 3, Attribs = { Text = "Monster 3" } },		
								{ Order = 4, Attribs = { Text = "Monster 4" } },		
							}
						},
						{ Order = 9, Attribs = { Text = "God Like!Rampage!!!" } },
					} })), { Order = 1 }),
				}
			}
		}
	}

	System.SetScreenSize({ 1400, 800 })
	inputs = Dialog.New("Sample dialog","Start console chat?",Dialog.Styles.YesNo,inputs)
	local widget = Widget.New(inputs)

	if widget then
		widget:Display(true, -1)
		Luna.Pin(Luna.Present(widget), { 800, 780 }, "righttop")
		local proxy : any = widget
		proxy.Nodes.Inputs.Nodes.left.Nodes.progress.Attribs.Value = 0.618
		proxy.Nodes.Inputs.Nodes.left.Nodes.radiobox.Nodes.loading.Attribs.Run = true
		LunaWidget.SetRootWidget(widget.Handle)
	end

	--[[
	local document = Dialog.New("Sample dialog","Start console chat?",Dialog.Styles.YesNo,inputs)
	local dialog = Luna.Pin(Widget.New(input),{800, 780},"righttop")
	local result = Luna.WaitForModalInput(dialog)
	print("You pushed the " .. result .. " button")
	]]
	Core.StepListen()

	-- local dialog = Widget:MoveToCenterAlways(Widget:Display(Dialog.New("Sample dialog", "Start console chat?", Dialog.Styles.YesNo, inputs), true), { 800, 780 })
	
	--[[	
	dialog.Inputs.left.progress.Attribs.Value = 0.618
	--dialog.Inputs.left.progress.Attribs.Value = 0.2
	local result = Widget.WaitForModalInput(dialog)
	
	dialog.Inputs.right.listbox =  TreeBox.New(ListBox.New({ Nodes = {
						{ Order = 1, Attribs = { Text = "First Blood!" } },
						{ Order = 2, Attribs = { Text = "Double Kill!!" } },
					} })), { Order = 1 })

	print("You pushed the " .. result .. " button")
	return result == "Yes"
	]]
end

return {
	Main = function ()
		return Core.Invoke(MainProc)
	end
}