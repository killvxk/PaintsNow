local MetaEvents = {}
local initHost : any
MetaEvents.__host = initHost
MetaEvents.Filters = {}

function MetaEvents:__newindex(key : string, value : any)
	return rawset(self, key, value)
end

function MetaEvents.New(host, events) : { string : any }
	events = events or rawget(host, "Events") or {}
	events.__host = host or events.__host
	events.Filters = events.Filters or {}
	return events
end

return MetaEvents