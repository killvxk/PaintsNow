// ZShaderTextPreprocssor.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-18
//

#ifndef __ZSHADERWALKER_H__
#define __ZSHADERWALKER_H__

#include "../PaintsNow.h"
#include "../Interface/IReflect.h"
#include "../Template/TShaderVariable.h"
#include <string>
#include <map>
#include <list>

namespace PaintsNow {
	class ZShaderWalker : public IReflect {
	public:
		typedef IShaderVariableBase::VAR_TYPE VAR_TYPE;
		struct ShaderText {
			String head;
			String body;
			std::map<String, String> slots;
		};

		ZShaderWalker(IShader& shader, IRender::SHADER_TYPE mainShaderType, std::vector<ShaderText>& shaderText);
		void GenerateCode();

	protected:
		virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
		virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);

	protected:
		void BindConstant(const String& rep, int value);
		void BindSwitch(const String& rep, bool enable);
		void BindIdentifier(const String& rep, const String& id);
		String ProcessCodeEx(const String& input) const;
		virtual void Bind(Unique typeID, const String& name, VAR_TYPE varType, bool array, bool flat, void* value, size_t count);

	protected:
		IShader& shader;
		IRender::SHADER_TYPE mainShaderType;
		std::vector<ShaderText>& shaderText;
		std::map<String, bool> switches;
		std::list<std::pair<String, String> > replacements;
	};
}

#endif // __ZSHADERWALKER_H__