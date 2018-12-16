// FlameWork.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-3
// 

#ifndef __FLAMEWORK_H__
#define __FLAMEWORK_H__

#include "../../Core/Interface/IScript.h"
#include "Native.h"

namespace PaintsNow {
	namespace NsFlameWork {
		class FlameWork : public IScript::Library {
		public:
			FlameWork(IThread& threadApi, IScript& nativeScript);

			// function convension
			// int main(int argc, char* argv[]);
			// <Output> argv[0] -> returns a pointer to 

			void RequestCompileNativeCode(IScript::Request& request, String& code);
			void RequestExecuteNative(IScript::Request& request, IScript::Delegate<Native> native, String& entry, std::vector<String>& params);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

		private:
			IScript& nativeScript;
		};
	}
}

#endif // __FLAMEWORK_H__