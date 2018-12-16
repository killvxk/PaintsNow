-- StandardRenderFlow.tl
-- 

local StandardRenderFlow = {}

local Create = RenderFlowComponentModule.Create
local CreateRenderStage = RenderFlowComponentModule.CreateRenderStage
local ConnectRenderStagePort = RenderFlowComponentModule.ConnectRenderStagePort
local ExportRenderStagePort = RenderFlowComponentModule.ExportRenderStagePort

function StandardRenderFlow.New()
	local renderFlowComponent : NsMythForest__RenderFlowComponent = Create()

	-- Step 1: Setup PBR deferred flows
	local stageGeometryBuffer : NsMythForest__RenderStage = CreateRenderStage(renderFlowComponent, "GeometryBufferRenderStage", "")
	local stageDeferredLighting : NsMythForest__RenderStage = CreateRenderStage(renderFlowComponent, "DeferredLightingRenderStage", "")
	ConnectRenderStagePort(stageGeometryBuffer, "AlbedoShininess", stageDeferredLighting, "AlbedoShininess")
	ConnectRenderStagePort(stageGeometryBuffer, "NormalDepth", stageDeferredLighting, "NormalDepth")
	ConnectRenderStagePort(stageGeometryBuffer, "Material", stageDeferredLighting, "Material")

	-- Step 2: Apply HDR effects
	local stageHighDynamicRange : NsMythForest__RenderStage = CreateRenderStage(renderFlowComponent, "HighDynamicRangeRenderStage", "")
	ConnectRenderStagePort(stageDeferredLighting, "Color", stageHighDynamicRange, "HDRColor")
	ExportRenderStagePort(renderFlowComponent, "Primitives", stageGeometryBuffer, "Primitives")
	ExportRenderStagePort(renderFlowComponent, "MainColor", stageHighDynamicRange, "LDRColor")

	return renderFlowComponent
end

return StandardRenderFlow