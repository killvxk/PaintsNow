local ListBox = {}
local Core = require("Engine/Core")
local Luna = require("Engine/UserInterface/Luna")
local Widget = require("Engine/UserInterface/Widget")
local Scroll = require("Engine/UserInterface/Staff/Scroll")
local Slider = require("Engine/UserInterface/Widget/Slider")
local EditBox = require("Engine/UserInterface/Widget/EditBox")

-- Import some constants
local config = Widget.UIConfig
local plainFont = config.Plain.Font

local function ResizeParents(widget : Widget, diff : number)
	local subWidget = widget.Parent
	if subWidget then
		local size = subWidget.Attribs.CollapseSize
		if size and size[2] ~= 0 then
			size[2] = size[2] + diff
			size[4] = size[4] + diff
			subWidget.Attribs.CollapseSize = size
			return ResizeParents(subWidget, diff)
		end

		size = subWidget.Attribs.Size
		if size[2] ~= 0 then
			size[2] = size[2] + diff
			size[4] = size[4] + diff
			subWidget.Attribs.Size = size
			return ResizeParents(subWidget, diff)
		end
	end
end

local function Collapse(widget : Widget, origin : Widget, down : boolean, x : number, y : number)
	-- hide all sub elements?
	if down then
		return
	end

	local source = widget
	local subWidget = widget.Parent
	if subWidget then
		local show
		local diff
		if not subWidget.Attribs.CollapseSize then
			show = false
			subWidget.Attribs.CollapseSize = subWidget.Attribs.Size
			subWidget.Attribs.Size = source.Attribs.Size
			diff = subWidget.Attribs.Size[2] - subWidget.Attribs.CollapseSize[2]
		else
			show = true	
			diff = -subWidget.Attribs.Size[2] + subWidget.Attribs.CollapseSize[2]
			subWidget.Attribs.Size = subWidget.Attribs.CollapseSize 
			subWidget.Attribs.CollapseSize = nil
		end

		for key, value in pairs(subWidget.Nodes) do
			if key ~= "__host" and key ~= "Content" then
				value.Attribs.Show = show
			end
		end

		-- resize parents
		ResizeParents(subWidget, diff)
		-- return true
	end
end

local function Preprocess(inputList)
	inputList.Attribs = inputList.Attribs or {}
	inputList.Attribs.Layout = "vertical"
	inputList.Nodes = inputList.Nodes or {}

	local totalSize = 0
	local cs = 0
	for k, v in pairs(inputList.Nodes) do
		local editor = EditBox.New("", EditBox.Styles.Field, false)
		local w, s = Preprocess(v)
		if w and s then
			if totalSize == 0 then
				totalSize = totalSize + editor.Nodes.Content.Attribs.Size[2] + 16
			end
			s = s + editor.Nodes.Content.Attribs.Size[2]

			w.Order = w.Order + 1
			totalSize = totalSize + s

			-- editor.Attribs.Padding = { 5, 10, 5, 10 }
			editor.Attribs.Size = { 0, s, 0, s }
			editor.Attribs.Font = plainFont
			editor.Nodes.Content.Events = editor.Nodes.Content.Events or {}
			editor.Nodes.Content.Events.Filters = editor.Nodes.Content.Events.Filters or {}
			editor.Nodes.Content.Events.Filters.OnClick = Collapse
			inputList.Nodes[k] = Core.Merge(w, editor)
			cs = s
		end
	end

	-- print("TOTAL SIZE " .. totalSize)

	inputList.Events = inputList.Events or {}
	inputList.Events.Filters = inputList.Events.Filters or {}

	return inputList, totalSize
end

local function SetPosition(widget : Widget, origin : Widget, vertical : boolean, ratio : number)
	local content = widget.Nodes.Content
	if content then
		local list = content.Nodes.List
		if list then
			Scroll.SetScrollOffsetByRatio(list, { vertical and 0 or ratio, vertical and ratio or 0 })
		end
	end

	Slider.SetSliderPositionByRatio(widget.Nodes.ScrollBarVertical, true, ratio)
end

local function SetPositionFromScroll(widget : Widget, origin : Widget, vertical : boolean, ratio : number)
	Slider.SetSliderPositionByRatio(widget.Nodes.ScrollBarVertical, true, ratio)
end

function ListBox.New(inputList)
	-- assign size
	return {
		Attribs = {
			Weight = 1,
			Layout = "horizontal",
		},
		Nodes = {
			Content = {
				Order = 1,	
				Attribs = {
					Weight = 1,
					BkColor = { 1.0, 1.0, 1.0, 1.0 },
					Font = plainFont,
					Image = config.Frame.Image.Background,
					BkImage = config.Frame.Image.Frame,
					Padding = { 10, 10, 10, 10 },
				},
				Nodes = {	
					List = Core.Merge(Scroll.New((Preprocess(inputList)), true, 25, 2), { Order = 1 })
				}
			},
			ScrollBarVertical = Core.Merge(Slider.New(true), { Order = 2 })
		},
		Events = {
			Filters = {
				OnSlider = SetPosition,
				OnScroll = SetPositionFromScroll
			}
		}
	}
end

return ListBox