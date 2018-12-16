// IShader.h -- shader interface
// By PaintDream (paintdream@paintdream.com)
// 2014-12-3
//

#ifndef __ISHADER_H__
#define __ISHADER_H__


#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "IRender.h"
#include "IReflect.h"
#include "../Template/TShaderVariable.h"
#include <string>
#include <map>

namespace PaintsNow {
	typedef uint8_t SWITCH_TYPE;
	class MetaShader : public MetaNodeBase {
	public:
		MetaShader(IRender::SHADER_TYPE shaderType);
		MetaShader operator = (IRender::SHADER_TYPE shaderType);

		template <class T, class D>
		inline const MetaShader& operator () (T* t, D* d) const {
			return *this; // do nothing
		}

		template <class T, class D>
		struct RealType {
			typedef MetaShader Type;
		};

		typedef MetaShader VC6Type;

		virtual TObject<IReflect>& operator () (IReflect& reflect);
		IRender::SHADER_TYPE shaderType;
	};

	extern MetaShader ShaderType;

	class IShader : public IReflectObjectComplex {
	public:
		IShader();
		virtual ~IShader();

		virtual void OnAttach(IRender& render, IRender::Program* program);
		virtual void OnUpdate(IRender& render, IRender::Program* program);
		virtual void OnDetach(IRender& render, IRender::Program* program);
		
		const String& GetTextHead() const;
		const String& GetTextBody() const;

	protected:
		String shaderTextHead;
		String shaderTextBody;
	};
}


#endif // __ZSHADER_H__
