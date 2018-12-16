#include "MaterialResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

MaterialResource::MaterialResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

uint64_t MaterialResource::GetMemoryUsage() const {
	return 0;
}

void MaterialResource::Attach(IRender& render) {
}

void MaterialResource::Detach(IRender& render) {

}

void MaterialResource::Upload(IRender& render) {

}

void MaterialResource::Download(IRender& render) {

}

size_t MaterialResource::GetMaterialCount() const {
	return collection.materials.size();
}

const IAsset::Material& MaterialResource::GetMaterialParam(size_t i) const {
	assert(i < GetMaterialCount());
	return collection.materials[i];
}

TShared<ShaderResource> MaterialResource::GetMaterialShader(size_t i) const {
	assert(i < GetMaterialCount());
	return shaderResources[i];
}

String MaterialResource::MatchStaticSwitches(std::map<String, SWITCH_TYPE>& switches, const IAsset::Material& material) {
	static Unique boolUnique = UniqueType<bool>::Get();
	for (size_t i = 0; i < material.variables.size(); i++) {
		const IAsset::Material::Variable& var = material.variables[i];
		if (var.type == boolUnique) {
			if (switches.count(var.key)) {
				switches[var.key] = var.value == "true";
			}
		}
	}

	// Makeup string
	String hashStr = ShaderResource::GetShaderPathPrefix() + material.shaderName;
	char ch = 0, count = 0;
	char buf[6];
	static_assert(sizeof(SWITCH_TYPE) <= 1, "Please modify the following code if you change the size of SWITCH_TYPE");
	for (std::map<String, SWITCH_TYPE>::const_iterator it = switches.begin(); it != switches.end(); ++it) {
		ch = (ch << 1) + (char)(it->second);
		if (++count == 8) {
			sprintf(buf, "%02X", ch);
			count = ch = 0;
			hashStr += buf;
		}
	}

	return hashStr;
}

TShared<ShaderResource> MaterialResource::MakeShaderResource(IRender& render, const IAsset::Material& material) {
	// Load Basic Shader Resource Template ...
	ShaderResource* resource = static_cast<ShaderResource*>(resourceManager.LoadExist(ShaderResource::GetShaderPathPrefix() + material.shaderName));
	if (resource != nullptr) {
		std::map<String, SWITCH_TYPE> switches;
		resource->GetStaticSwitches(switches);

		// match switches
		String shaderHash = MatchStaticSwitches(switches, material);
		if (!shaderHash.empty()) { // has different switches with default one?
			// find hashed version ...
			ShaderResource* cached = static_cast<ShaderResource*>(resourceManager.LoadExist(shaderHash));
			if (cached == nullptr) {
				resource = static_cast<ShaderResource*>(resource->Duplicate());

				// apply switches
				resource->UpdateSwitches(switches);
				resource->SetLocation(shaderHash);
				resourceManager.Insert(shaderHash, resource);
			} else {
				resource = cached;
			}
		}
	}

	return TShared<ShaderResource>::Move(resource);
}

void MaterialResource::FlushMaterials(IRender& render) {
	std::vector<TShared<ShaderResource> > shaderResources(collection.materials.size());

	// prepare new materials
	for (size_t j = 0; j < collection.materials.size(); j++) {
		shaderResources[j] = std::move(MakeShaderResource(render, collection.materials[j]));
	}

	std::swap(this->shaderResources, shaderResources);
}

TObject<IReflect>& MaterialResource::operator () (IReflect& reflect) {
	ReflectClass(MaterialResource)[Interface(GraphicResourceBase)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(collection);
	}

	return *this;
}