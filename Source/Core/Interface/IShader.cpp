#include "IShader.h"
using namespace PaintsNow;

namespace PaintsNow {
	MetaShader ShaderType(IRender::VERTEX_SHADER);
}

MetaShader::MetaShader(IRender::SHADER_TYPE v) : shaderType(v) {}
MetaShader MetaShader::operator = (IRender::SHADER_TYPE value) {
	return MetaShader(value);
}

TObject<IReflect>& MetaShader::operator () (IReflect& reflect) {
	ReflectClass(MetaShader);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(shaderType);
	}

	return *this;
}

IShader::IShader() {}
IShader::~IShader() {}

const String& IShader::GetTextHead() const {
	return shaderTextHead;
}

const String& IShader::GetTextBody() const {
	return shaderTextBody;
}

template <bool attach>
class ReflectVariable : public IReflect {
public:
	ReflectVariable(IShader& s, IRender& r, IRender::Program* p) : IReflect(true, false), shader(s), render(r), program(p), vertexBufferElementCount((size_t)-1) {}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		if (!s.IsBaseObject()) {
			if (s.IsIterator()) {
				IIterator& iter = static_cast<IIterator&>(s);
				while (iter.Next()) {
					(*reinterpret_cast<IReflectObject*>(iter.Get()))(*this);
				}
			} else {
				assert(s.QueryInterface(UniqueType<IShaderVariableBase>()) != nullptr);
				IShaderVariableBase& var = static_cast<IShaderVariableBase&>(s);
				if (attach) {
					var.Initialize(render, program, name);
				} else {
					if (var.GetType() == IShaderVariableBase::INPUT_ATTRIB_BUFFER) {
						// get element size
						size_t size = render.GetBufferSize(var.GetBuffer());
						size_t unit = var.GetDataLength();
						size /= unit;
						if (vertexBufferElementCount != (size_t)-1) {
							assert(size == vertexBufferElementCount); // all var buffer must have the same element count
						} else {
							vertexBufferElementCount = size;
						}
					}
					var.Bind(render, program);
				}
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

private:
	IShader& shader;
	IRender& render;
	IRender::Program* program;
	size_t vertexBufferElementCount;
};

void IShader::OnAttach(IRender& render, IRender::Program* program) {
	ReflectVariable<true> reflect(*this, render, program);
	(*this)(reflect);
}

void IShader::OnUpdate(IRender& render, IRender::Program* program) {
	ReflectVariable<false> reflect(*this, render, program);
	(*this)(reflect);
}

void IShader::OnDetach(IRender& render, IRender::Program* program) {}