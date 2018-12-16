#ifndef __ZSCRIPTLUA_H__
#define __ZSCRIPTLUA_H__

#include "../../../Core/Interface/IScript.h"

struct lua_State;

namespace PaintsNow {
	// A general interface of lua
	class ZScriptLua final : public IScript {
	public:
		ZScriptLua(IThread& threadApi);
		virtual ~ZScriptLua();

		bool BeforeCall();
		void AfterCall();

		class Request final : public IScript::Request {
		public:
			Request(ZScriptLua* s, lua_State* L);
			virtual ~Request();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual IScript* GetScript();
			virtual bool Call(const AutoWrapperBase& defer, const Request::Ref& g);
			virtual std::vector<Key> Enumerate();
			virtual IScript::Request::TYPE GetCurrentType();
			static IScript::Request::TYPE ConvertType(int type);
			virtual void Debug(std::vector<StackInfo>& debugInfo, std::vector<LocalInfo>& localInfos, const Ref* func);
			virtual Request::Ref Load(const String& script, const String& pathname);
			virtual IScript::Request& Push();
			virtual IScript::Request& Pop();
			virtual IScript::Request& operator >> (IScript::Request::Ref&);
			virtual IScript::Request& operator << (const IScript::Request::Ref&);
			virtual IScript::Request& operator << (const IScript::Request::Nil&);
			virtual IScript::Request& operator << (const IScript::BaseDelegate&);
			virtual IScript::Request& operator >> (IScript::BaseDelegate&);
			virtual IScript::Request& operator << (const IScript::Request::Global&);
			virtual IScript::Request& operator << (const IScript::Request::Local&);
			virtual IScript::Request& operator << (const IScript::Request::TableStart&);
			virtual IScript::Request& operator >> (IScript::Request::TableStart&);
			virtual IScript::Request& operator << (const IScript::Request::TableEnd&);
			virtual IScript::Request& operator >> (const IScript::Request::TableEnd&);
			virtual IScript::Request& operator << (const IScript::Request::Key&);
			virtual IScript::Request& operator >> (const IScript::Request::Key&);
			virtual IScript::Request& operator << (const LocalInfo&);
			virtual IScript::Request& operator >> (const LocalInfo&);
			virtual IScript::Request& operator << (double value);
			virtual IScript::Request& operator >> (double& value);
			virtual IScript::Request& operator << (const String& str);
			virtual IScript::Request& operator >> (String& str);
			virtual IScript::Request& operator << (const char* str);
			virtual IScript::Request& operator >> (const char*& str);
			virtual IScript::Request& operator << (int64_t value);
			virtual IScript::Request& operator >> (int64_t& value);
			virtual IScript::Request& operator << (bool b);
			virtual IScript::Request& operator >> (bool& b);
			virtual IScript::Request& operator << (const AutoWrapperBase& wrapper);
			virtual IScript::Request& operator >> (const Skip& skip);
			virtual IScript::Request& MoveVariables(IScript::Request& target, size_t count);
			virtual bool IsValid(const BaseDelegate& d);
			virtual void Dereference(IScript::Request::Ref& ref);
			virtual IScript::Request::Ref ReferenceWithDebugInfo(const IScript::BaseDelegate& d, const String& tag, int line);
			virtual IScript::Request::Ref ReferenceWithDebugInfo(const IScript::Request::Ref& d, const String& tag, int line);
			virtual IScript::Request::TYPE GetReferenceType(const IScript::Request::Ref& d);
			virtual void DebugPrint();
			virtual IScript::Request& CleanupIndex();
			virtual int GetCount();
			void SetIndex(int i);
			lua_State* GetRequestState();
			void AddReference(IScript::Request::Ref ref, const String& file, int line);
			void DelReference(IScript::Request::Ref ref);

		private:
			ZScriptLua* state;
			IScript::Request::Ref ref;
			String key;
			LocalInfo local;
			lua_State* L;
			int idx;
			int initCount;
			int tableLevel;
		};

		friend class Request;

		lua_State* GetState() const;
		lua_State* GetDeferState() const;
		virtual void DoLock();
		virtual void UnLock();
		virtual bool TryLock();
		virtual void Reset();
		virtual IScript::Request& GetDefaultRequest();
		virtual IScript* NewScript() const;
		virtual IScript::Request* NewRequest(const String& entry);
		virtual const char* GetFileExt() const;
		virtual void SetDebugHandler(const TWrapper<void, IScript::Request&, int, int>& handler, int mask);
		virtual const char* QueryUniformResource(const String& path, size_t& length);

		int GetInitDeferCount() const;
		void SetInitDeferCount(int count);
		bool IsResetting() const;

	private:
		void Init();
		void Clear();

		lua_State* state;
		lua_State* deferState;
		ZScriptLua::Request* defaultRequest;
		size_t totalReference;
		int initCountDefer;
		size_t callCounter;
		IThread::Event* runningEvent;
		static std::map<String, std::pair<const char*, size_t> > builtinModules;
		bool resetting;
	private:

#ifdef _DEBUG
		std::map<size_t, String> debugReferences;
#endif
	};
}

#endif // __ZSCRIPTLUA_H__
