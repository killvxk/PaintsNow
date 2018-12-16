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

local Scene = require("Script/Scene/Scene")
local StandardCamera = require("Script/Prefabs/Components/Camera/StandardCamera")
local StandardRenderFlow = require("Script/Prefabs/Components/RenderFlow/StandardRenderFlow")
local Explorer = {}

function Explorer:Main()
	-- Load Scene
	local scene = Scene.New()
	local cameraEntity = MythForest.CreateEntity()
	local renderFlowComponent = StandardRenderFlow.New()
	local cameraComponent = StandardCamera.New(renderFlowComponent)
	MythForest.AddEntityComponent(cameraEntity, cameraComponent)
end

return Explorer