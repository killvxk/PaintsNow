// Module.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-15
//

#ifndef __MODULE_H__
#define __MODULE_H__

#include "../../Core/Interface/Interfaces.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Entity;
		class Engine;
		class Module : public IScript::Library {
		public:
			Module(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual Unique GetTinyUnique() const = 0;
			virtual void TickFrame();

		protected:
			Engine& engine;
		};

		template <class T>
		class ModuleImpl : public Module {
		public:
			ModuleImpl(Engine& engine) : Module(engine) {}
			virtual Unique GetTinyUnique() const {
				return UniqueType<T>::Get();
			}
		};
	}
}

#endif // __MODULE_H__