local StandardCamera = {}

local Create = CameraComponentModule.Create

function StandardCamera.New(renderFlowComponent : NsMythForest__RenderFlowComponent)
	local camera = Create(renderFlowComponent)

	return camera
end

return StandardCamera