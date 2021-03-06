// HoneyData.h
// By PaintDream (paintdream@paintdream.com)
// 2015-12-31
//

#ifndef __HONEY_H__
#define __HONEY_H__

#include "../../Core/Interface/IDatabase.h"
#include "../../Core/Interface/IScript.h"
#include "../../Core/Common/ZDynamicObject.h"
#include "../../Core/Object/Tiny.h"

namespace PaintsNow {
	namespace NsRemembery {
		class SchemaResolver : public IReflect {
		public:
			SchemaResolver();
			virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
			static void SetValueString(IScript::Request& request, char* base);
			static void SetValueText(IScript::Request& request, char* base);
			static void SetValueFloat(IScript::Request& request, char* base);
			static void SetValueInt(IScript::Request& request, char* base);
			virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);

			typedef void(*Set)(IScript::Request& request, char* base);
			std::vector<std::pair<Set, size_t> > setters;
		};

		class Honey : public SharedTiny {
		public:
			Honey(IDatabase::MetaData* metaData);
			bool Step();
			void WriteLine(IScript::Request& request);

		protected:
			SchemaResolver resolver;
			IDatabase::MetaData* metaData;
		};

		class HoneyData : public IDatabase::MetaData {
		public:
			HoneyData();
			virtual ~HoneyData();

			virtual IIterator* New() const;
			virtual void Attach(void* base);
			virtual void Initialize(size_t count);
			virtual size_t GetTotalCount() const;
			virtual void* Get();
			virtual const IReflectObject& GetPrototype() const;
			virtual bool Next();
			virtual bool IsLayoutLinear() const;
			virtual bool IsLayoutPinned() const;
			virtual void* GetHost() const;
			virtual Unique GetPrototypeUnique() const;

			void SetFloat(size_t i);
			void SetString(size_t i);
			void SetInteger(size_t i);

		private:
			ZDynamicUniqueAllocator uniqueAllocator;
			ZDynamicObject* dynamicObject;
			IScript::Request* request;
			IScript::Request::Ref tableRef;
			size_t index;
			size_t count;
			typedef void (HoneyData::*Set)(size_t i);
			std::vector<Set> sets;
		};

		IScript::Request& operator >> (IScript::Request& request, HoneyData& honey);
	}
}


#endif // __HONEY_H__
