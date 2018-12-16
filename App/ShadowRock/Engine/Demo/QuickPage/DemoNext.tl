-- DemoNext.lua
-- Next generation demo presentation platform
-- By PaintDream (paintdream@paintdream.com)
-- 2016-1-23
--

local DemoNext = { __index = _ENV }
local Core = require("Engine/Core")

--[[
function Template()

end

function Main()
	local Map = {}

	Map.Center = function ()
		print("Center")
		return Core.Listen(function (defer, direction)
			local dispatch = {
				left = Map.West,
				right = Map.East,
				top = Map.North,
				bottom = Map.South
			}

			return dispatch[direction] and dispatch[direction]() or Core.Listen(defer)()
		end)
	end

	Map.East = function ()
		print("East")
		return Core.Listen(function (defer, direction)
			local dispatch = {
				left = Map.Center,
				right = Map.West,
			}

			return dispatch[direction] and dispatch[direction]() or Core.Listen(defer)()
		end)
	end

	Map.West = function ()
		print("West")
		return Core.Listen(function (defer, direction)
			local dispatch = {
				left = Map.Center,
				right = Map.East,
			}

			return dispatch[direction] and dispatch[direction]() or Core.Listen(defer)()
		end)
	end

	Map.North = function ()
		print("North")
		return Core.Listen(function (defer, direction)
			local dispatch = {
				top = Map.South,
				bottom = Map.Center,
			}

			return dispatch[direction] and dispatch[direction]() or Core.Listen(defer)()
		end)

	end

	Map.South = function ()
		print("South")
		return Core.Listen(function (defer, direction)
			local dispatch = {
				top = Map.Center,
				bottom = Map.North,
			}

			return dispatch[direction] and dispatch[direction]() or Core.Listen(defer)()
		end)

	end

	local instance = Invoke(Map.Center)
	local map = { w = 'top', s = 'bottom', d = 'right', a = 'left' }
	while (true) do
		local input = io.read()
		Post(instance, map[input])
	end
end

Main()
]]

return DemoNext

