#include "ShaderResource.h"
#include "../../../Core/Interface/IShader.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ShaderResource::ShaderResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

const String& ShaderResource::GetShaderName() const {
	return GetUnique()->typeName;
}

uint64_t ShaderResource::GetMemoryUsage() const {
	return 0;
}

bool ShaderResource::operator << (IStreamBase& stream) {
	return false;
}

bool ShaderResource::operator >> (IStreamBase& stream) {
	return false;
}

void ShaderResource::Attach(IRender& render) {
}

void ShaderResource::Detach(IRender& render) {

}

Tiny* ShaderResource::Duplicate() const {
	return new ShaderResource(resourceManager, uniqueLocation);
}

ZPipeline& ShaderResource::GetPipeline() {
	static ZPipeline dummy;
	assert(false);
	return dummy;
}

void ShaderResource::Upload(IRender& render) {

}

void ShaderResource::Download(IRender& render) {

}

class StaticSwitchProbe : public IReflect {
public:
	StaticSwitchProbe() : IReflect(true, false) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static_assert(sizeof(bool) == sizeof(SWITCH_TYPE), "Please modify the following code if you change type of SWITCH_TYPE");
		static Unique boolUnique = UniqueType<bool>::Get();
		static Unique switchTypeUnique = UniqueType<SWITCH_TYPE>::Get();

		if (typeID == boolUnique || typeID == switchTypeUnique) {
			// add to static switch ...
			assert(s.IsBaseObject());
			SWITCH_TYPE value = *reinterpret_cast<SWITCH_TYPE*>(ptr);
			switches[name] = value;
		} else if (s.QueryInterface(UniqueType<IShaderVariableBase>()) != nullptr)  {
			IShaderVariableBase& var = static_cast<IShaderVariableBase&>(s);
			varTypes[name] = var.GetType();
		} else if (!s.IsBaseObject()) {
			s(*this); // continue searching for sub pipelines ...
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

	std::map<String, SWITCH_TYPE> switches;
	std::map<String, IShaderVariableBase::VAR_TYPE> varTypes;
};

void ShaderResource::GetStaticSwitches(std::map<String, SWITCH_TYPE>& switches) {
	ZPipeline& pipeline = GetPipeline();
	StaticSwitchProbe probe;
	pipeline(probe);
	std::swap(probe.switches, switches);
}

ShaderResource* ShaderResource::CreateShadowInstance() {
	ZPipeline& pipeline = GetPipeline();
	StaticSwitchProbe probe;
	pipeline(probe);

	String id;
	id.resize(probe.switches.size() + probe.varTypes.size());

	size_t i = 0;
	for (std::map<String, SWITCH_TYPE>::const_iterator it = probe.switches.begin(); it != probe.switches.end(); ++it) {
		id[i++] = '0' + (char)it->second;
	}

	for (std::map<String, IShaderVariableBase::VAR_TYPE>::const_iterator ip = probe.varTypes.begin(); ip != probe.varTypes.end(); ++ip) {
		id[i++] = '0' + (char)ip->second;
	}

	String newLocation = uniqueLocation;
	size_t sharp = newLocation.find('#');
	if (sharp != String::npos) {
		newLocation = newLocation.substr(0, sharp);
	}

	newLocation = newLocation + "#" + id;
	ShaderResource* exist = static_cast<ShaderResource*>(resourceManager.LoadExist(newLocation));
	if (exist != nullptr) {
		return exist;
	} else {
		uniqueLocation = newLocation;
		return static_cast<ShaderResource*>(resourceManager.Insert(newLocation, this));
	}
}

class UpdateSwitch : public IReflect {
public:
	UpdateSwitch(const std::map<String, SWITCH_TYPE>& s) : IReflect(true, false), switches(s) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique boolUnique = UniqueType<bool>::Get();
		if (typeID == boolUnique) {
			// add to static switch ...
			assert(s.IsBaseObject());
			const std::map<String, SWITCH_TYPE>::const_iterator it = switches.find(name);
			static_assert(sizeof(bool) == sizeof(SWITCH_TYPE), "Please modify the following code if you change type of SWITCH_TYPE");
			if (it != switches.end()) {
				*reinterpret_cast<SWITCH_TYPE*>(ptr) = it->second;
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

	const std::map<String, SWITCH_TYPE>& switches;
};


void ShaderResource::UpdateSwitches(const std::map<String, SWITCH_TYPE>& switches) {
	ZPipeline& pipeline = GetPipeline();
	UpdateSwitch updater(switches);
	pipeline(updater);
}

const String& ShaderResource::GetShaderPathPrefix() {
	static const String shaderPrefix = "[Runtime]/ShaderResource/";
	return shaderPrefix;
}
