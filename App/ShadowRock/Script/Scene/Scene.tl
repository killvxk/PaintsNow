local Scene = {}
Scene.__index = Scene
local rootEntity : NsMythForest__Entity = {}
Scene.RootEntity = rootEntity

function Scene.New()
	local rootEntity : NsMythForest__Entity = MythForest.CreateEntity()
	local scene = {}
	scene.RootEntity = rootEntity

	return setmetatable({}, Scene)
end

function Scene:Prefetch()

end

return Scene