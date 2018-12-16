#include "ZPipeline.h"
#include <string>

using namespace PaintsNow;

typedef ZPipeline::ArgumentsHandler ArgumentsHandler;

template <bool skipOutputBindings>
class PipelineInspector : public IReflect {
public:
	PipelineInspector() : IReflect(true, false), offset(0) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) override {
		if (!s.IsBaseObject()) {
			IShaderVariableBase* var = s.QueryInterface(UniqueType<IShaderVariableBase>());
			if (var != nullptr) {
				if (var->GetType() == IShaderVariableBase::OUTPUT_TARGET && skipOutputBindings)
					return;
				
				ArgumentsHandler handler;
				handler.var = var;
				handler.binder = var->GetBinder();
				handler.dataLength = (uint32_t)var->GetDataLength();
				handler.exporter = var->GetExporter();
				handler.importer = var->GetImporter();
				handler.offset = offset;
				argsHandler.push_back(std::move(handler));
				argsMap[name] = argsMap.size();

				offset += handler.dataLength;
			} else {
				(s)(*this);
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) override {}

	std::vector<ArgumentsHandler> argsHandler;
	std::map<String, size_t> argsMap;
	uint32_t offset;
};

ZPipeline::ZPipeline() : program(nullptr), indexBuffer(0), fromIndex(0), toIndex(0), instanceCount(0) {}

TObject<IReflect>& ZPipeline::operator () (IReflect& reflect) {
	ReflectClass(ZPipeline);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(program)[Runtime];
		ReflectProperty(indexBuffer)[Runtime];
		ReflectProperty(fromIndex)[Runtime];
		ReflectProperty(toIndex)[Runtime];
	}

	return *this;
}

ZPipeline::~ZPipeline() {
	assert(program == nullptr);
}


class ReflectCollectShader : public IReflect {
public:
	ReflectCollectShader() : IReflect(true, false) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique shaderTypeUnique = UniqueType<MetaShader>::Get();
		while (meta != nullptr) {
			const MetaNodeBase* metaNode = meta->GetNode();
			if (metaNode->GetUnique() == shaderTypeUnique) {
				const MetaShader* metaShader = static_cast<const MetaShader*>(metaNode);
				assert(!s.IsBaseObject() && s.QueryInterface(UniqueType<IShader>()) != nullptr);
				shaders[metaShader->shaderType].push_back(static_cast<IShader*>(&s));
				return;
			}

			meta = meta->GetNext();
		}

		assert(s.IsBaseObject() || s.QueryInterface(UniqueType<IShader>()) == nullptr);
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}
	virtual void Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta) {}

	std::map<IRender::SHADER_TYPE, std::vector<IShader*> > shaders;
};

bool ZPipeline::IsCompiled() const {
	return program != nullptr;
}

void ZPipeline::Cleanup(IRender& render) {
	// exit shaders
	if (program != nullptr) {
		render.DeleteProgram(program);
		program = nullptr;
	}

	ReflectCollectShader allShaders;
	(*this)(allShaders);
	for (std::map<IRender::SHADER_TYPE, std::vector<IShader*> >::const_iterator ip = allShaders.shaders.begin(); ip != allShaders.shaders.end(); ++ip) {
		for (size_t i = 0; i < ip->second.size(); i++) {
			(ip->second)[i]->OnDetach(render, program);
		}
	}

	argsHandler.clear();
}


IRender::Program* ZPipeline::GetProgram() const {
	return program;
}

IRender::Buffer* ZPipeline::GetIndexBuffer() const {
	return indexBuffer;
}

void ZPipeline::GetIndexRange(uint32_t& from, uint32_t& to) const {
	from = fromIndex;
	to = toIndex;
}

void ZPipeline::BindIndexBuffer(IRender& render, IRender::Buffer* buffer, uint32_t from, uint32_t to, uint32_t inst) {
	render.BindIndex(program, buffer, from, to);
	indexBuffer = buffer;
	fromIndex = from;
	toIndex = to;
	instanceCount = instanceCount;
}

void ZPipeline::FireRender(IRender& render) {
	eventFireRender(render, *this);
	// deprecated
	// UpdateParams(render);

	for (size_t k = 0; k < argsHandler.size(); k++) {
		ArgumentsHandler& handler = argsHandler[k];
		handler.var->Bind(render, program);
	}

	render.FireRender(program);
}

void ZPipeline::FireRenderCompacted(IRender& render, const String& dataBuffer) {
	IRender::Program* program = GetProgram();
	assert(!argsHandler.empty()); // must call UpdateArgsHandlers before calling me.
	assert(dataBuffer.size() == argsHandler.back().dataLength + argsHandler.back().offset);
	const char* p = dataBuffer.data();

	for (size_t k = 0; k < argsHandler.size(); k++) {
		ArgumentsHandler& handler = argsHandler[k];
		handler.binder(render, program, handler.var, p + handler.offset);
	}

	assert(indexBuffer != nullptr);
	render.FireRender(program);
}


bool ZPipeline::Compile(IRender& render) {
	Cleanup(render);

	program = render.CreateProgram();

	if (program == nullptr) {
		Cleanup(render);
		return false;
	}

	ReflectCollectShader allShaders;
	(*this)(allShaders);

	for (std::map<IRender::SHADER_TYPE, std::vector<IShader*> >::iterator it = allShaders.shaders.begin(); it != allShaders.shaders.end(); ++it) {
		std::vector<IShader*>& s = it->second;
		render.SetShaderSource(program, it->first, &s[0], (uint32_t)s.size(), "main");
	}

	if (!render.CompileProgram(program)) {
		Cleanup(render);
	}

	for (std::map<IRender::SHADER_TYPE, std::vector<IShader*> >::const_iterator ip = allShaders.shaders.begin(); ip != allShaders.shaders.end(); ++ip) {
		for (size_t i = 0; i < ip->second.size(); i++) {
			(ip->second)[i]->OnAttach(render, program);
		}
	}

	return UpdateArgsHandlers(false);
}

bool ZPipeline::UpdateArgsHandlers(bool skipOutputBindings) {
	PipelineInspector<false> insp;
	PipelineInspector<true> inspSkip;
	(*this)(skipOutputBindings ? static_cast<IReflect&>(inspSkip) : static_cast<IReflect&>(insp));

	std::swap(insp.argsMap, argsMap);
	std::swap(insp.argsHandler, argsHandler);
	return !argsHandler.empty();
}

void ZPipeline::UpdateParams(IRender& render) {
	ReflectCollectShader allShaders;
	(*this)(allShaders);

	for (std::map<IRender::SHADER_TYPE, std::vector<IShader*> >::const_iterator it = allShaders.shaders.begin(); it != allShaders.shaders.end(); ++it) {
		for (size_t i = 0; i < it->second.size(); i++) {
			(it->second)[i]->OnUpdate(render, program);
		}
	}
}

void ZPipeline::Attach(IRender& render, IRender::Target* renderTarget) {
	assert(program != nullptr);
	render.BeginBind(program, renderTarget);
}

void ZPipeline::Detach(IRender& render, IRender::Target* renderTarget) {
	render.EndBind(program, renderTarget);
}