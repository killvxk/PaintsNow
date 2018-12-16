// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LeavesExporterPCH.h"
#include "ILeavesExporter.h"
#include "LeavesExporter.h"
#include "CoreUObject.h"
#include "SWindow.h"
#include "LeavesWidget.h"
#include "Service.h"

// Resources
#include "../../../../../Source/Utility/MythForest/Entity.h"
#include "../../../../../Source/Utility/MythForest/Component/Form/FormComponent.h"
#include "../../../../../Source/Utility/SnowyStream/Resource/MeshResource.h"
#include "../../../../../Source/Utility/SnowyStream/Resource/MaterialResource.h"
#include "../../../../../Source/Driver/Filter/Pod/ZFilterPod.h"

#define LOCTEXT_NAMESPACE "FLeavesExporter"

IMPLEMENT_MODULE(FLeavesExporter, LeavesExporter)

// Just follow the instructions here: https://answers.unrealengine.com/questions/25609/customizing-the-editors-toolbar-buttons-menu-via-c.html

// static PaintsNow::NsGalaxyWeaver::Service service;
static const FName LeavesTabName("LeavesTab");
using namespace PaintsNow::NsGalaxyWeaver;

void FLeavesExporter::StartupModule() {
	UE_LOG(LogStats, Log, TEXT("Start LeavesExporter "));
	TSharedRef<class FGlobalTabmanager> tm = FGlobalTabmanager::Get();

	FLeavesCommands::Register();

	leavesCommands = MakeShareable(new FUICommandList);

	leavesCommands->MapAction(
		FLeavesCommands::Get().button,
		FExecuteAction::CreateRaw(this, &FLeavesExporter::OnButtonClicked),
		FCanExecuteAction());

	toolbarExtender = MakeShareable(new FExtender);
	toolbarExtension = toolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, leavesCommands, FToolBarExtensionDelegate::CreateRaw(this, &FLeavesExporter::AddToolbarExtension));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(toolbarExtender);

	extensionManager = LevelEditorModule.GetToolBarExtensibilityManager();

	// TSharedRef<FWorkspaceItem> cat = tm->AddLocalWorkspaceMenuCategory(FText::FromString(TEXT("LeavesExporter")));
	// auto WorkspaceMenuCategoryRef = cat.ToSharedRef();

	/*
	tm->RegisterTabSpawner(LeavesTabName, FOnSpawnTab::CreateRaw(this, &FLeavesExporter::SpawnTab))
		.SetDisplayName(FText::FromString(TEXT("Leaves Exporter")));*/

	tm->RegisterNomadTabSpawner(LeavesTabName, FOnSpawnTab::CreateRaw(this, &FLeavesExporter::SpawnTab))
		.SetDisplayName(FText::FromString(TEXT("Leaves Exporter")))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions.Small"));

	service = MakeUnique<Service>();
	service->Initialize(this);
}

void FLeavesExporter::WriteLog(LOG_LEVEL logLevel, const std::string& content) {
	FString uniContent = UTF8_TO_TCHAR(content.c_str());
	switch (logLevel) {
		case LOG_TEXT:
			UE_LOG(LogStats, Display, TEXT("%s"), *uniContent);
			break;
		case LOG_WARNING:
			UE_LOG(LogStats, Warning, TEXT("%s"), *uniContent);
			break;
		case LOG_ERROR:
		default:
			UE_LOG(LogStats, Error, TEXT("%s"), *uniContent);
			break;
	}
}

void FLeavesExporter::OnButtonClicked() {
	FGlobalTabmanager::Get()->InvokeTab(LeavesTabName);
	// Trigger scene collection
	// CollectSceneEntities();
}


TSharedRef<SDockTab> FLeavesExporter::SpawnTab(const FSpawnTabArgs& TabSpawnArgs) {
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.ViewOptions.Small"))
		.TabRole(ETabRole::NomadTab)
		.Label(NSLOCTEXT("LeavesExporter", "Expoter", "Control Panel"))
		[
			SNew(SLeavesWidget)
		];
}

void FLeavesExporter::AddToolbarExtension(FToolBarBuilder &builder) {
	UE_LOG(LogStats, Log, TEXT("Starting Extension logic"));

	FSlateIcon IconBrush = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions", "LevelEditor.ViewOptions.Small");

	builder.AddToolBarButton(FLeavesCommands::Get().button, NAME_None, LOCTEXT("LeavesCommands", "Leaves Exporter"), LOCTEXT("LeavesCommandsOverride", "Click to toggle LeavesExporter"), IconBrush, NAME_None);
}

void FLeavesExporter::ShutdownModule() {
	if (extensionManager.IsValid()) {
		FLeavesCommands::Unregister();

		TSharedRef<class FGlobalTabmanager> tm = FGlobalTabmanager::Get();
		tm->UnregisterNomadTabSpawner(LeavesTabName);

		toolbarExtender->RemoveExtension(toolbarExtension.ToSharedRef());
		extensionManager->RemoveExtender(toolbarExtender);
	} else {
		extensionManager.Reset();
	}

	service->Deinitialize();
}

void FLeavesExporter::CollectSceneEntities() {
	// Enumerate actors
	for (TObjectIterator<AActor> Itr; Itr; ++Itr) {
		AActor& actor = **Itr;
		CollectSceneEntity(actor);
	}
}

void FLeavesExporter::CollectSceneEntity(AActor& actor) {
	using namespace PaintsNow;
	using namespace PaintsNow::NsMythForest;
	Entity entity;
	// Add necessary component(s)
	String actorName = TCHAR_TO_UTF8(*actor.GetName());
	/*
	FormComponent formComponent(actorName);
	entity.AddComponent(&formComponent);*/

	// Enumerate all components and export them
	ExportActorComponentsByClass(entity, actor, &FLeavesExporter::OnExportMeshComponent);
	ExportActorComponentsByClass(entity, actor, &FLeavesExporter::OnExportLandscapeComponent);

	// Export entity attributes ...

	// Resolve grouping information ...

	// Resolve prefab information ...
}

static void OnResourceComplete(PaintsNow::IScript::Request& request) {

}

static void PostResource(Service& service, PaintsNow::NsSnowyStream::ResourceBase& resource, PaintsNow::String& location) {
	// PostResource
	using namespace PaintsNow;
	using namespace PaintsNow::NsSnowyStream;
	ZMemoryStream stream(4096, true);
	static ZFilterPod filter;
	IStreamBase* f = filter.CreateFilter(stream);
	*f << resource;
	f->ReleaseObject();

	auto& request = service.GetMainRequest();
	String extension = SnowyStream::GetReflectedExtension(resource.GetUnique());
	String data(reinterpret_cast<const char*>(stream.GetBuffer()), stream.GetTotalLength());
	service->PostResource(request, request.Adapt(Wrap(OnResourceComplete)), true, location, extension, data);
}

void FLeavesExporter::OnExportExpressionInput(UMaterialExpression* expression) {
	if (expression->IsA(UMaterialExpressionTextureBase::StaticClass())) {
		auto textureExpression = static_cast<UMaterialExpressionTextureBase*>(expression);
		auto samplerType = textureExpression->SamplerType;
		auto texture = textureExpression->Texture;
		OnExportTextureResource(texture, samplerType);
	}
}

void FLeavesExporter::OnExportColorMaterialInput(FColorMaterialInput& input) {
	// fetch albedo texture
	if (!input.UseConstant) {
		OnExportExpressionInput(input.Expression);
	}
}

void FLeavesExporter::OnExportTextureResource(UTexture* texture, EMaterialSamplerType samplerType) {
	if (!collectedObjects.Contains(texture)) {
		collectedObjects.Add(texture);
	}
}

void FLeavesExporter::OnExportMeshComponent(PaintsNow::NsMythForest::Entity& entity, UMeshComponent* meshComponent) {
	using namespace PaintsNow;
	using namespace PaintsNow::NsSnowyStream;

	if (meshComponent->IsA(UStaticMeshComponent::StaticClass())) {
		auto staticMeshComponent = static_cast<UStaticMeshComponent*>(meshComponent);
		auto materials = staticMeshComponent->GetMaterials();
		for (int32 i = 0; i < materials.Num(); i++) {
			auto materialInterface = materials[i];
			if (materialInterface->IsA(UMaterial::StaticClass())) {
				auto material = static_cast<UMaterial*>(materialInterface);
				if (!collectedObjects.Contains(material)) {
					collectedObjects.Add(material);

					// params
					auto& metallic = material->Metallic;
					auto& roughness = material->Roughness;
					auto& albedo = material->BaseColor;
					auto& emission = material->EmissiveColor;
					auto& specular = material->Specular;
					auto& normal = material->Normal;

					// TODO: get source texture if exists
					auto& resourceManager = service->GetResourceManager();
					String materialName = TCHAR_TO_UTF8(*material->GetName());
					MaterialResource materialResource(resourceManager, materialName);
					auto materialCollection = materialResource.Inspect(UniqueType<IAsset::MaterialCollection>());
					
					OnExportColorMaterialInput(albedo);
				}
			}
		}

		auto staticMesh = staticMeshComponent->GetStaticMesh();
		if (!collectedObjects.Contains(staticMesh)) {
			collectedObjects.Add(staticMesh);

			UE_LOG(LogStats, Display, TEXT("Collecting Mesh: %s\n"), *staticMesh->GetName());
			auto& lodResources = staticMesh->RenderData->LODResources;
			for (int32 i = 0; i < lodResources.Num(); i++) {
				auto& mesh = lodResources[i];
				auto& resourceManager = service->GetResourceManager();
				String meshName = TCHAR_TO_UTF8(*staticMesh->GetName());
				meshName += "_LOD_" + std::to_string(i);
				MeshResource meshResource(resourceManager, meshName);
				auto meshCollection = meshResource.Inspect(UniqueType<IAsset::MeshCollection>());

				if (meshCollection != nullptr) {
					// Copy position buffer
					auto& vertexBuffers = mesh.VertexBuffers;
					auto& positionBuffer = vertexBuffers.PositionVertexBuffer;
					auto& targetPositionBuffer = meshCollection->vertices;
					targetPositionBuffer.resize(positionBuffer.GetNumVertices());
					for (uint32 j = 0; j < positionBuffer.GetNumVertices(); j++) {
						auto position = positionBuffer.VertexPosition(j);
						targetPositionBuffer[j] = ZFloat3(position.X, position.Y, position.Z);
					}

					// Copy UVs
					auto& staticMeshBuffer = vertexBuffers.StaticMeshVertexBuffer;
					auto& targetUVBuffer = meshCollection->texCoords;
					targetUVBuffer.resize((staticMeshBuffer.GetNumTexCoords() + 1) >> 1);
					for (uint32 j = 0; j < positionBuffer.GetNumVertices(); j++) {
						for (uint32 k = 0; k < staticMeshBuffer.GetNumTexCoords(); k++) {
							auto uv = staticMeshBuffer.GetVertexUV(j, k);
							auto& target = targetUVBuffer[k >> 1][j];
							if (k & 1) {
								target.x() = uv.X;
								target.y() = uv.Y;
							} else {
								target.z() = uv.X;
								target.w() = uv.Y;
							}
						}
					}

					// Copy indices
					auto indexBuffer = mesh.IndexBuffer.GetArrayView();
					auto& targetIndexBuffer = meshCollection->indices;
					targetIndexBuffer.resize(indexBuffer.Num() / 3);
					for (int32 j = 0; j < indexBuffer.Num() / 3; j++) {
						for (int32 m = 0; m < 3; m++) {
							auto index = indexBuffer[j * 3 + m];
							targetIndexBuffer[j][m] = index;
						}
					}

					// Copy vertex colors
					auto& colorBuffer = vertexBuffers.ColorVertexBuffer;
					auto& targetColorBuffer = meshCollection->colors;
					for (uint32 j = 0; j < colorBuffer.GetNumVertices(); j++) {
						auto color = colorBuffer.VertexColor(j);
						targetColorBuffer[j] = ZFloat4((float)color.R / 255, (float)color.G / 255, (float)color.B / 255, (float)color.A / 255);
					}

					PostResource(*service.Get(), meshResource, meshName);

					UE_LOG(LogStats, Display, TEXT("LOD %d Vertices: %d, Primitives: %d, VertexColor: %s, UV Channels: %d\n"), i, positionBuffer.GetNumVertices(), indexBuffer.Num() / 3, colorBuffer.GetNumVertices() == 0 ? TEXT("False") : TEXT("True"), staticMeshBuffer.GetNumTexCoords());
				}
			}
		}
	}
}

void FLeavesExporter::OnExportLandscapeComponent(PaintsNow::NsMythForest::Entity& entity, ULandscapeComponent* landspaceComponent) {}