#include "ModelResource.h"
#include "MeshResource.h"
#include "MaterialResource.h"
#include "SkeletonResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ModelResource::ModelResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

TObject<IReflect>& ModelResource::operator () (IReflect& reflect) {
	ReflectClass(ModelResource)[Interface(GraphicResourceBase)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(meshResource)[Runtime];
		ReflectProperty(materialResource)[Runtime];
	}

	return *this;
}

void ModelResource::Attach(IRender& render) {
	/*
	meshResource->Load(resourceManager);
	materialResource.Load(resourceManager);*/
}

void ModelResource::Detach(IRender& render) {
	/*
	meshResource.UnLoad();
	materialResource.UnLoad();*/
}

void ModelResource::Upload(IRender& render) {
	// check material
	if (materialResource && meshResource) {
		std::vector<String> renderData;
		std::vector<TShared<TextureResource> > textureResources;

		for (size_t i = 0; i < materialResource->GetMaterialCount(); i++) {
			TShared<ShaderResource> shader = materialResource->GetMaterialShader(i);
			const IAsset::Material& material = materialResource->GetMaterialParam(i);

			// fill corresponding params
			ZPipeline& pipeline = shader->GetPipeline();
			std::vector<ZPipeline::ArgumentsHandler>& argsHandler = pipeline.argsHandler;
			std::map<String, size_t>& argsMap = pipeline.argsMap;

			if (!argsHandler.empty()) {
				const ZPipeline::ArgumentsHandler& lastHandler = argsHandler[argsHandler.size() - 1];
				String drawCallData;
				drawCallData.resize(lastHandler.offset + lastHandler.dataLength);

				const std::vector<IAsset::Material::Variable>& vars = material.variables;
				for (size_t k = 0; k < vars.size(); k++) {
					const String& key = vars[k].key;
					const String& value = vars[k].value;

					std::map<String, size_t>::const_iterator it = argsMap.find(key);
					if (it != argsMap.end()) {
						ZPipeline::ArgumentsHandler& handler = argsHandler[it->second];
						if (handler.var->GetType() == IShaderVariableBase::INPUT_UNIFORM_TEXTURE) {
							// get correspond texture
							ResourceBase* resource = resourceManager.LoadExist(value);
							TextureResource* textureResource = resource->QueryInterface(UniqueType<TextureResource>());
							if (textureResource != nullptr) {
								textureResources.push_back(textureResource);
								memcpy(const_cast<char*>(drawCallData.data()) + handler.offset, &textureResource, handler.dataLength);
							}

							resource->ReleaseObject();
						} else {
							// must compatible
							if (handler.dataLength == value.size()) {
								memcpy(const_cast<char*>(drawCallData.data()) + handler.offset, value.data(), handler.dataLength);
							}
						}
					}
				}
			}
		}

		std::swap(this->renderData, renderData);
		std::swap(this->textureResources, textureResources);
	}
}