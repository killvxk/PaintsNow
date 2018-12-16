#pragma once

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		class Service;
	}
	namespace NsMythForest {
		class Entity;
	}
}

class FLeavesExporter : public ILeavesExporter, public ISceneExplorer
{
public:
	TSharedPtr<SViewport> Viewport;
	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs);
	void OnButtonClicked();

protected:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void WriteLog(LOG_LEVEL logLevel, const std::string& content);

	void AddToolbarExtension(FToolBarBuilder &);
	void CollectSceneEntities();
	void CollectSceneEntity(AActor& actor);

	template <class T>
	void ExportActorComponentsByClass(PaintsNow::NsMythForest::Entity& entity, AActor& actor, void (FLeavesExporter::*exportProc)(PaintsNow::NsMythForest::Entity&, T* component)) {
		TArray<UActorComponent*> components = actor.GetComponentsByClass(typename T::StaticClass());
		for (int32 i = 0; i < components.Num(); i++) {
			T* component = static_cast<T*>(components[i]);
			(this->*exportProc)(entity, component);
		}
	}

	void OnExportMeshComponent(PaintsNow::NsMythForest::Entity& entity, UMeshComponent* meshComponent);
	void OnExportLandscapeComponent(PaintsNow::NsMythForest::Entity& entity, ULandscapeComponent* landspaceComponent);
	void OnExportColorMaterialInput(FColorMaterialInput& input);
	void OnExportExpressionInput(UMaterialExpression* expression);
	void OnExportTextureResource(UTexture* texture, EMaterialSamplerType samplerType);

	TSharedPtr<FUICommandList> leavesCommands;
	TSharedPtr<FExtensibilityManager> extensionManager;
	TSharedPtr<const FExtensionBase> toolbarExtension;
	TSharedPtr<FExtender> toolbarExtender;
	TSet<UObject*> collectedObjects;
	TUniquePtr<PaintsNow::NsGalaxyWeaver::Service> service;

};
