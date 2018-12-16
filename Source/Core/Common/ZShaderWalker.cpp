#include "ZShaderWalker.h"
#include "../Interface/IShader.h"
#include <sstream>

using namespace PaintsNow;

ZShaderWalker::ZShaderWalker(IShader& s, IRender::SHADER_TYPE type, std::vector<ShaderText>& text) : IReflect(true, false), shader(s), mainShaderType(type), shaderText(text) {}

void ZShaderWalker::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	static Unique boolType = UniqueType<bool>::Get();
	static Unique intType = UniqueType<int>::Get();
	if (typeID == boolType) {
		BindSwitch(name, *reinterpret_cast<bool*>(ptr));
	} else if (typeID == intType) {
		BindConstant(name, *reinterpret_cast<int*>(ptr));
	} else if (!s.IsBaseObject()) {
		// Must be TShaderVariable!!
		if (s.IsIterator()) {
			IIterator& it = static_cast<IIterator&>(s);
			if (!it.GetPrototype().IsBaseObject()) {
				while (it.Next()) {
					(*reinterpret_cast<IReflectObject*>(it.Get()))(*this);
				}
			}
		} else {
			IShaderVariableBase& base = static_cast<IShaderVariableBase&>(s);
			Bind(base.GetSubUnique(), name, base.GetType(), false, false, &base, 0);
		}
	}
}

void ZShaderWalker::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}


void ZShaderWalker::Bind(Unique typeID, const String& rep, VAR_TYPE varType, bool array, bool flat, void* value, size_t count) {
}

void ZShaderWalker::BindSwitch(const String& rep, bool enable) {
	switches[rep] = enable;
}

void ZShaderWalker::BindIdentifier(const String& rep, const String& id) {
	replacements.push_back(std::make_pair(String("$") + rep, id));
}

void ZShaderWalker::BindConstant(const String& rep, int value) {
	std::ostringstream s;
	s << value;
	BindIdentifier(rep, s.str());
}

void ZShaderWalker::GenerateCode() {
	// cleanup last values ...
	shader(*this);
	ShaderText& s = shaderText[(int)mainShaderType];
	s.head += ProcessCodeEx(shader.GetTextHead());
	s.body += ProcessCodeEx(shader.GetTextBody());
}

String ZShaderWalker::ProcessCodeEx(const String& input) const {
	String result = input;
	for (size_t i = 1; i < result.length(); i++) {
		char ch = result[i - 1];
		char next = result[i];
		if ((ch == ';' || ch == '{' || ch == '}') && next == ' ') {
			result[i] = '\n';
		}
	}

	for (std::list<std::pair<String, String> >::const_iterator p = replacements.begin(); p != replacements.end(); ++p) {
		size_t pos = 0;
		size_t srclen = p->first.size();
		while ((pos = result.find(p->first, pos)) != String::npos) {
			if (isalnum(result[pos + srclen]) || result[pos + srclen] == '_') {
				pos++;
				continue;
			}

			result.replace(pos, srclen, p->second);
		}
	}

	String prepare;
	result.swap(prepare);

	// FIND IF
	size_t pos = 0;
	size_t oldpos = 0;
	const String& prefix = "IF ";
	const String& midfix = "ELSE";
	const String& postfix = "ENDIF";
	while ((pos = prepare.find(prefix, pos)) != String::npos) {
		result.append(prepare.substr(oldpos, pos - oldpos));
		// parse the switch name
		size_t from = pos + prefix.size();
		while (!(isalnum(prepare[from]) || prepare[from] == '_')) from++;
		size_t to = from;
		while (isalnum(prepare[to]) || prepare[from] == '_') to++;
		pos = to;

		String id = prepare.substr(from, to - from);
		std::map<String, bool>::const_iterator p = switches.find(id);

		from = to;
		size_t t = prepare.find(midfix, to);
		to = prepare.find(postfix, to);

		bool hasElsePart = t != String::npos && t < to;
		if (p != switches.end() && p->second) {
			// reserve
			String part = prepare.substr(from + 2, (hasElsePart ? t : to) - from - 2);
			result.append(part);
		} else if (hasElsePart) { // has else part
			String elsepart = prepare.substr(t + midfix.size() + 2, to - t - midfix.size() - 2);
			result.append(elsepart);
		}

		oldpos = to + postfix.size();
	}

	if (pos != (size_t)-1)
		result.append(prepare.substr(oldpos, pos - oldpos));
	else
		result.append(prepare.substr(oldpos));

	return result;
}
