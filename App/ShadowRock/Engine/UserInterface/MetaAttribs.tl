local MetaAttribs = {}
local initHost : any
MetaAttribs.__host = initHost

-- Aux Helpers
local function PackConfig(func)
	return function (handle, config)
		return func(handle, table.unpack(config))
	end
end

local function SetImage(widget, image, coord, percent) -- percent not used by now
	return LunaWidget.SetWidgetImage(widget, image, coord, { 0, 0, 1, 1})
end

MetaAttribs.Set = {
	Image = PackConfig(SetImage),
	Layout = LunaWidget.SetWidgetLayout,
	Font = PackConfig(LunaWidget.SetWidgetFont),
	Text = LunaWidget.SetWidgetText,
	Password = LunaWidget.SetWidgetPasswordChar,
	BkColor = LunaWidget.SetWidgetBkColor,
	FitContent = LunaWidget.SetWidgetFitContent,
	Float = LunaWidget.SetWidgetFloat,
	Show = LunaWidget.ShowWidget,
	Rect = LunaWidget.SetWidgetRect,
	Size = LunaWidget.SetWidgetSize,
	Padding = LunaWidget.SetWidgetPadding,
	Margin = LunaWidget.SetWidgetMargin,
	Weight = LunaWidget.SetWidgetWeight,
	SubIndexRange = PackConfig(LunaWidget.SetWidgetIndexRange),
	ScrollOffset = LunaWidget.SetWidgetScrollOffset,
}

MetaAttribs.Get = {
	Rect = LunaWidget.GetWidgetRect,
	ClippedRect = LunaWidget.GetWidgetClippedRect,
	Text = LunaWidget.GetWidgetText,
	Size = LunaWidget.GetWidgetSize,
	Float = LunaWidget.GetWidgetFloat,
	Padding = LunaWidget.GetWidgetPadding,
	Weight = LunaWidget.GetWidgetWeight,
	FitContent = LunaWidget.GetWidgetFitContent,
	Show = LunaWidget.IsWidgetShow,
	ScrollSize = LunaWidget.GetWidgetScrollSize,
	ScrollOffset = LunaWidget.GetWidgetScrollOffset
}

function MetaAttribs.New(host, attribs) : { string : any }
	local newAttribs = attribs or rawget(host, "Attribs") or {}
	newAttribs.__host = host or newAttribs.__host
	assert(newAttribs.__host)
	setmetatable(newAttribs, MetaAttribs)

	for k, v in pairs(newAttribs) do
		if k ~= "__host" then
			newAttribs[k] = nil
			newAttribs[k] = v -- refresh
		end
	end

	return newAttribs
end

function MetaAttribs.__index(object, key)
	if (key ~= "__index") then
		assert(key ~= "__host")
		local attrib = MetaAttribs.Get[key]
		if (attrib) then
			return attrib(object.__host.Handle)
		else
			local host = object.__host
			local events = host.Events
			if (events) then
				local filter = host.Events.Filters
				if (filter) then
					local routine = filter.OnAttrib
					if (routine) then
						local p = routine[key]
						if (p) then
							local c = p(false, host, key)
							if (c) then
								return c
							end
						end
					end
				end

				local attribEvents = events.OnAttrib
				if (attribEvents) then
					local handler = attribEvents[key]
					if (handler) then
						return handler(false, host, key)
					end
				end
			end
		end
	end

	return rawget(object, key)
end


function MetaAttribs.__newindex(object, key, value)
	if (key ~= "__newindex") then
		assert(key ~= "__host")
		local attrib = MetaAttribs.Set[key]
			
		if (attrib) then
			return attrib(object.__host.Handle, value)
		else
			local host = object.__host	
		
			local events = host.Events
			if (events) then
				local filter = events.Filters
				if (filter) then
					local routine = filter.OnAttrib
					if (routine) then
						local p = routine[key]
						
						if (p) then
							value = p(true, host, key, value)
						end
					end
				end

				local attribEvents = events.OnAttrib
				if (attribEvents) then
					local handler = attribEvents[key]
					if (handler) then
						value = handler(true, host, key, value)
					end
				end
			end
		end
	end

	return rawset(object, key, value)
end

return MetaAttribs