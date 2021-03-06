#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "ZScriptLua.h"
#include <cassert>
#include <cstring>


extern "C" {
	#include "Core/lua.h"
	#include "Core/lualib.h"
	#include "Core/lauxlib.h"
	#include "Core/lobject.h"
	#include "Core/lstate.h"
	extern int luaopen_lpeg(lua_State *L);
}


#include "TypedLua/loader.inc"
#include "TypedLua/tlast.inc"
#include "TypedLua/tlchecker.inc"
#include "TypedLua/tldparser.inc"
#include "TypedLua/tlfilter.inc"
#include "TypedLua/tllexer.inc"
#include "TypedLua/tlst.inc"
#include "TypedLua/tltype.inc"
#include "TypedLua/tlcode.inc"
#include "TypedLua/tlparser.inc"
#include "TypedLua/lsl53/tl_base.inc"
#include "TypedLua/lsl53/tl_coroutine.inc"
#include "TypedLua/lsl53/tl_debug.inc"
#include "TypedLua/lsl53/tl_io.inc"
#include "TypedLua/lsl53/tl_math.inc"
#include "TypedLua/lsl53/tl_os.inc"
#include "TypedLua/lsl53/tl_package.inc"
#include "TypedLua/lsl53/tl_string.inc"
#include "TypedLua/lsl53/tl_table.inc"
#include "TypedLua/lsl53/tl_utf8.inc"

#ifdef _MSC_VER
#pragma warning(disable:4800)
#endif

using namespace PaintsNow;

// static int g_global = 0;
static int g_scriptKey = 1;
static int g_bindKey = 2;
static int g_dummyKey = 3;
static int g_refKey = 4;
static int g_objKey = 5;
static int g_wrapKey = 6;
static int g_localKey = 7;

std::map<String, std::pair<const char*, size_t> > ZScriptLua::builtinModules;

inline ZScriptLua* GetScript(lua_State* L) {
	lua_pushlightuserdata(L, (void*)&g_scriptKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	ZScriptLua* pRet = reinterpret_cast<ZScriptLua*>(lua_touserdata(L, -1));
	lua_pop(L, 1);

	return pRet;
}

static void DebugHandler(lua_State* L, lua_Debug* ar) {
	ZScriptLua* pRet = GetScript(L);
	ZScriptLua::Request s(pRet, L);
	TWrapper<void, IScript::Request&, int, int> handler = pRet->GetDebugHandler();
	if (handler) {
		int m = 0;
		switch (ar->event) {
			case LUA_HOOKCALL:
				m = IScript::DEBUG_CALL;
				break;
			case LUA_HOOKTAILCALL:
				m = IScript::DEBUG_TAILCALL;
				break;
			case LUA_HOOKRET:
				m = IScript::DEBUG_RETURN;
				break;
			case LUA_HOOKLINE:
				m = IScript::DEBUG_LINE;
				break;
		}

		assert(pRet->GetLockCount() == 1);
		pRet->UnLock();

		if (handler) {
			handler(s, m, ar->currentline);
		}

		pRet->DoLock();
	}
}

static void HandleError(ZScriptLua* script, lua_State* L) {
	if (script->IsResetting()) {
		return;
	}
	// error
	ZScriptLua::Request s(script, L);
	const char* errMsg = lua_tostring(L, -1);
	size_t locksize = script->GetLockCount();
	for (size_t i = 0; i < locksize; i++) {
		script->UnLock();
	}

	s.Error(errMsg);
	if (strcmp(errMsg, "attempt to yield across a C-call boundary") == 0) {
		s.Error("Please use asynchronized calls in your script handler while calling lua routines that may yield themselves.");
	}

	for (size_t j = 0; j < locksize; j++) {
		script->DoLock();
	}

	lua_pop(L, 1);
}

static int ContinueProxy(lua_State* L, int status, lua_KContext ctx);

static int FunctionProxyEx(ZScriptLua* pRet, const IScript::Request::AutoWrapperBase* wrapper, lua_State* L) {
	if (pRet->IsResetting()) {
		return 0;
	}

	int valsize = 0;
	ZScriptLua::Request s(pRet, L);
	int ptr = lua_gettop(L);
	// popup all locks
	size_t locksize = pRet->GetLockCount();
	assert(locksize == 1);
	pRet->UnLock();

	(*wrapper).Execute(s);

	pRet->DoLock();
	valsize = lua_gettop(L) - ptr;
	return valsize;
}

static int ContinueProxy(lua_State* L, int status, lua_KContext ctx) {
	IScript::Request::AutoWrapperBase* wrapper = reinterpret_cast<IScript::Request::AutoWrapperBase*>(ctx);
	ZScriptLua* pRet = GetScript(L);
	if (status != LUA_OK && status != LUA_YIELD) {
		HandleError(pRet, L);
		return 0;
	} else {
		if (wrapper != nullptr) {
			FunctionProxyEx(pRet, wrapper, L);
			delete wrapper;
		}

		int count = pRet->GetInitDeferCount();
		lua_State* state = pRet->GetDeferState();
		int savedValsize = (int)lua_tointeger(state, -1);
		lua_pop(state, 1);

		if (count != 0) {
			int paramsize = lua_gettop(state) - count;

			lua_xmove(state, L, paramsize);
			wrapper = reinterpret_cast<IScript::Request::AutoWrapperBase*>(lua_touserdata(L, -1));
			lua_pop(L, 1);

			pRet->SetInitDeferCount((int)lua_tointeger(state, -1));
			lua_pop(state, 1);
			lua_pushinteger(state, savedValsize); // repush

			return ContinueProxy(L, lua_pcallk(L, paramsize - 2, LUA_MULTRET, 0, (lua_KContext)wrapper, ContinueProxy), (lua_KContext)wrapper);
		} else {
			return savedValsize;
		}
	}
}

static int FunctionProxy(lua_State* L) {
	// const TProxy<void, IScript::Request&>* proxy = reinterpret_cast<TProxy<void, IScript::Request&>* const>(lua_touserdata(L, lua_upvalueindex(1)));
	// IHost* handler = reinterpret_cast<IHost*>(lua_touserdata(L, lua_upvalueindex(2)));
	const IScript::Request::AutoWrapperBase* wrapper = *reinterpret_cast<const IScript::Request::AutoWrapperBase**>(lua_touserdata(L, lua_upvalueindex(1)));

	ZScriptLua* pRet = GetScript(L);

	int valsize = FunctionProxyEx(pRet, wrapper, L);
	lua_State* defer = pRet->GetDeferState();
	lua_pushinteger(defer, valsize);
	return ContinueProxy(L, LUA_OK, (lua_KContext)nullptr);
}


static int FreeMem(lua_State* L) {
	void* mem = lua_touserdata(L, -1);
	if (mem == nullptr) // we only free user data
		return 0;

	IScript::Object* obj = *(IScript::Object**)mem;
	ZScriptLua::Request s(GetScript(L), L);

	IScript* script = s.GetScript();
	size_t locksize = script->GetLockCount();
	for (size_t i = 0; i < locksize; i++) {
		script->UnLock();
	}

	// printf("<<<< DELETED OBJ: %p Type: %s\n", obj, obj->GetUnique()->typeName.c_str());
	if (obj != nullptr) {
		obj->ScriptUninitialize(s);
	}

	for (size_t j = 0; j < locksize; j++) {
		script->DoLock();
	}

	return 0;
}

static int FreeWrapper(lua_State* L) {
	IScript::Request::AutoWrapperBase** base = reinterpret_cast<IScript::Request::AutoWrapperBase**>(lua_touserdata(L, -1));
	delete *base;
	// printf("Freed!");
	return 0;
}

static int SetMetaTableEx(lua_State* L) {
	int t = lua_type(L, 2);
	// do not limited to tables
	// luaL_checktype(L, 1, LUA_TTABLE);
	luaL_argcheck(L, t == LUA_TNIL || t == LUA_TTABLE, 2,
		"nil or table expected");

	if (luaL_getmetafield(L, 1, "__metatable") != LUA_TNIL)
		return luaL_error(L, "cannot change a protected metatable");
	
	if (lua_type(L, 1) == LUA_TUSERDATA) {
		// set gc table
		lua_pushcfunction(L, FreeMem);
		lua_setfield(L, 2, "__gc");
	}

	lua_settop(L, 2);
	lua_setmetatable(L, 1);

	return 1;
}

class InitBuiltinModules {
public:
	InitBuiltinModules(std::map<String, std::pair<const char*, size_t> >& builtinModules) {
		// register builtin modules
		builtinModules["typedlua.lua"] = std::make_pair((const char*)loader_lua, (size_t)loader_lua_len);
		builtinModules["typedlua.tlast.lua"] = std::make_pair((const char*)tlast_lua, (size_t)tlast_lua_len);
		builtinModules["typedlua.tlchecker.lua"] = std::make_pair((const char*)tlchecker_lua, (size_t)tlchecker_lua_len);
		builtinModules["typedlua.tldparser.lua"] = std::make_pair((const char*)tldparser_lua, (size_t)tldparser_lua_len);
		builtinModules["typedlua.tlfilter.lua"] = std::make_pair((const char*)tlfilter_lua, (size_t)tlfilter_lua_len);
		builtinModules["typedlua.tllexer.lua"] = std::make_pair((const char*)tllexer_lua, (size_t)tllexer_lua_len);
		builtinModules["typedlua.tlst.lua"] = std::make_pair((const char*)tlst_lua, (size_t)tlst_lua_len);
		builtinModules["typedlua.tltype.lua"] = std::make_pair((const char*)tltype_lua, (size_t)tltype_lua_len);
		builtinModules["typedlua.tlcode.lua"] = std::make_pair((const char*)tlcode_lua, (size_t)tlcode_lua_len);
		builtinModules["typedlua.tlparser.lua"] = std::make_pair((const char*)tlparser_lua, (size_t)tlparser_lua_len);
		builtinModules["typedlua/lsl53/base.tld"] = std::make_pair((const char*)__base_tld, (size_t)__base_tld_len);
		builtinModules["typedlua/lsl53/coroutine.tld"] = std::make_pair((const char*)__coroutine_tld, (size_t)__coroutine_tld_len);
		builtinModules["typedlua/lsl53/package.tld"] = std::make_pair((const char*)__package_tld, (size_t)__package_tld_len);
		builtinModules["typedlua/lsl53/string.tld"] = std::make_pair((const char*)__string_tld, (size_t)__string_tld_len);
		builtinModules["typedlua/lsl53/utf8.tld"] = std::make_pair((const char*)__utf8_tld, (size_t)__utf8_tld_len);
		builtinModules["typedlua/lsl53/table.tld"] = std::make_pair((const char*)__table_tld, (size_t)__table_tld_len);
		builtinModules["typedlua/lsl53/math.tld"] = std::make_pair((const char*)__math_tld, (size_t)__math_tld_len);
		builtinModules["typedlua/lsl53/io.tld"] = std::make_pair((const char*)__io_tld, (size_t)__io_tld_len);
		builtinModules["typedlua/lsl53/os.tld"] = std::make_pair((const char*)__os_tld, (size_t)__os_tld_len);
		builtinModules["typedlua/lsl53/debug.tld"] = std::make_pair((const char*)__debug_tld, (size_t)__debug_tld_len);
	}
};

ZScriptLua::ZScriptLua(IThread& threadApi) : IScript(threadApi), callCounter(0), resetting(false) {
	static InitBuiltinModules initBuiltinModules(builtinModules);
	runningEvent = threadApi.NewEvent();
	Init();
}

bool ZScriptLua::IsResetting() const {
	return resetting;
}

void ZScriptLua::Clear() {
	DoLock();
	dispatcher.Clear();

	lua_close(state);
	delete defaultRequest;
	UnLock();
}

void ZScriptLua::Init() {
	DoLock();
	state = luaL_newstate();
	defaultRequest = new ZScriptLua::Request(this, state);
	luaL_openlibs(state);
	callCounter = 0;

	lua_pushlightuserdata(state, &g_scriptKey);
	lua_pushlightuserdata(state, this);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make __gc metatable
	lua_pushlightuserdata(state, &g_bindKey);
	lua_newtable(state);
	lua_pushliteral(state, "__gc");
	lua_pushcfunction(state, FreeMem);
	lua_settable(state, -3);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make a dummy node for empty table accesses
	lua_pushlightuserdata(state, &g_dummyKey);
	lua_newtable(state);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make a reference table for references
	lua_pushlightuserdata(state, &g_refKey);
	lua_newtable(state);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make a table for delegates
	lua_pushlightuserdata(state, &g_objKey);
	lua_newtable(state);
	lua_pushliteral(state, "v");
	lua_setfield(state, -2, "__mode");
	lua_pushvalue(state, -1);
	lua_setmetatable(state, -2);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make a table for local storage
	lua_pushlightuserdata(state, &g_localKey);
	lua_newtable(state);
	lua_pushliteral(state, "k");
	lua_setfield(state, -2, "__mode");
	lua_pushvalue(state, -1);
	lua_setmetatable(state, -2);
	lua_settable(state, LUA_REGISTRYINDEX);

	// make a table for wrappers
	lua_pushlightuserdata(state, &g_wrapKey);
	lua_newtable(state);
	lua_pushliteral(state, "__gc");
	lua_pushcfunction(state, FreeWrapper);
	lua_settable(state, -3);
	lua_settable(state, LUA_REGISTRYINDEX);

	// load lpeg lib
	luaL_requiref(state, "lpeg", luaopen_lpeg, 0);
	// const char* type = lua_typename(state, lua_type(state, -1));

	/*
	luaL_loadbuffer(state, (const char*)Core_lua, Core_lua_len, "Core");
	lua_call(state, 0, 1); // get returned core lib
	lua_setglobal(state, "Core");*/

	// enable setmetatable for userdata
	lua_pushcfunction(state, SetMetaTableEx);
	lua_setglobal(state, "setmetatable");

	totalReference = 0;
	initCountDefer = 0;

	deferState = lua_newthread(state); // don't pop it from stack unless the state was closed.
	resetting = false;

	if (debugHandler) {
		// set hook
		SetDebugHandler(debugHandler, debugMask);
	}

	UnLock();
}


bool ZScriptLua::BeforeCall() {
	if (resetting) {
		return false;
	}

	assert(GetLockCount() == 1);
	callCounter++;

	return true;
}

void ZScriptLua::AfterCall() {
	assert(GetLockCount() == 1);
	if (--callCounter == 0) {
		if (resetting)
			threadApi.Signal(runningEvent, false);
	}
}

void ZScriptLua::Reset() {
	// Wait for all active routines finished.
	DoLock();
	assert(!resetting);
	resetting = true;

	if (callCounter != 0) {
		threadApi.Wait(runningEvent, lock);
	}

	UnLock();

	IScript::Reset();
	Clear();

#ifdef _DEBUG
	if (totalReference != 0) {
		for (std::map<size_t, String>::iterator it = debugReferences.begin(); it != debugReferences.end(); ++it) {
			printf("Leak lua object where: %s\n", (*it).second.c_str());
		}
	}
#endif

	assert(totalReference == 0);
	Init();
}


ZScriptLua::~ZScriptLua() {
	Clear();

	threadApi.DeleteEvent(runningEvent);
}


void ZScriptLua::Request::DebugPrint() {
	int tp = lua_gettop(L);
	printf("IDX = %d\n", idx);
	for (int i = 1; i <= tp; i++) {
		printf("[%d] - %s\n", i, lua_typename(L, lua_type(L, i)));
	}
}


IScript::Request::TYPE ZScriptLua::Request::GetReferenceType(const IScript::Request::Ref& g) {
	*this << g;
	int t = lua_type(L, -1);
	lua_pop(L, 1);

	return ZScriptLua::Request::ConvertType(t);
}


bool ZScriptLua::Request::Call(const AutoWrapperBase& defer, const IScript::Request::Ref& g) {
	assert(GetScript()->GetLockCount() == 1);
	assert(tableLevel == 0);
	if (!state->BeforeCall())
		return false;

	int pos = lua_gettop(L);
	int in = pos - initCount;
	// lua_State* s = state->GetState();
	if (defer.IsSync()) {
		*this << g;
		// printf("Type : %s\n", lua_typename(L, lua_type(L, -1)));
		assert(lua_isfunction(L, -1));
		lua_insert(L, initCount + 1);
		assert(lua_isfunction(L, -in - 1));
		// lua_KContext context = (lua_KContext)defer.Copy();
		// dispatch deferred calls after the next sync call.
		// int status = lua_pcallk(L, in, LUA_MULTRET, 0, nullptr, ContinueProxy);
		int status = lua_pcall(L, in, LUA_MULTRET, 0);
		if (status != LUA_OK && status != LUA_YIELD) {
			HandleError(static_cast<ZScriptLua*>(GetScript()), L);
			state->AfterCall();
			return false;
		} else {
			// important!
			SetIndex(initCount + 1);
			state->AfterCall();
			return true;
		}
	} else {
		// Invoke deferred call
		lua_State* host = state->GetDeferState();
		lua_pushinteger(host, state->GetInitDeferCount());
		state->SetInitDeferCount(lua_gettop(host));
		*this << g;
		lua_xmove(L, host, 1);
		lua_xmove(L, host, in);
		lua_pushlightuserdata(host, (void*)defer.Copy()); // as lightuserdata

		state->AfterCall();
		return true;
	}
}

const char* ZScriptLua::GetFileExt() const {
	static const String ext = "lua";
	return ext.c_str();
}

void ZScriptLua::SetDebugHandler(const TWrapper<void, IScript::Request&, int, int>& handler, int mask) {
	assert(GetLockCount() != 0);
	if (handler) {
		int m = 0;
		if ((mask & IScript::DEBUG_CALL) == IScript::DEBUG_CALL) m |= LUA_MASKCALL;
		if ((mask & IScript::DEBUG_RETURN) == IScript::DEBUG_RETURN) m |= LUA_MASKRET;
		if ((mask & IScript::DEBUG_LINE) == IScript::DEBUG_LINE) m |= LUA_MASKLINE;

		lua_sethook(state, DebugHandler, m, 1);
	} else {
		lua_sethook(state, nullptr, 0, 0);
	}

	IScript::SetDebugHandler(handler, mask);
}

const char* ZScriptLua::QueryUniformResource(const String& path, size_t& length) {
	std::map<String, std::pair<const char*, size_t> >::const_iterator it = builtinModules.find(path);
	if (it != builtinModules.end()) {
		length = it->second.second;
		return it->second.first;
	} else {
		return nullptr;
	}
}

IScript::Request* ZScriptLua::NewRequest(const String& entry) {
	assert(GetLockCount() == 1);
	return new ZScriptLua::Request(this, nullptr);
}

IScript::Request& ZScriptLua::Request::MoveVariables(IScript::Request& target, size_t count) {
	assert(GetScript()->GetLockCount() == 1);
	lua_xmove(L, (static_cast<ZScriptLua::Request&>(target)).L, (int)count);
	return *this;
}

IScript* ZScriptLua::Request::GetScript() {
	return state;
}

int ZScriptLua::Request::GetCount() {
	assert(GetScript()->GetLockCount() == 1);
	// return lua_gettop(L);
	return lua_gettop(L) - initCount;
}

IScript::Request& PaintsNow::ZScriptLua::Request::CleanupIndex() {
	idx = initCount + 1;
	return *this;
}

void ZScriptLua::Request::SetIndex(int i) {
	assert(GetScript()->GetLockCount() == 1);
	idx = i;
}

lua_State* ZScriptLua::Request::GetRequestState() {
	return L;
}

static int IncreaseTableIndex(lua_State* L, int count = 1) {
	void* t = (lua_touserdata(L, -1));
	int index = *(int*)&t;
	if (count == 0) {
		index = 0;
	} else {
		index += count;
	}
	lua_pop(L, 1);
	lua_pushlightuserdata(L, (void*)(size_t)index);
	return index;
}

inline void refget(lua_State* L, const IScript::Request::Ref& ref) {
	lua_pushlightuserdata(L, &g_refKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref.value);
	lua_replace(L, -2);
}


template <class F, class C>
inline void Write(lua_State* L, int& tableLevel, int& idx, String& key, IScript::Request::LocalInfo& localInfo, F f, const C& value) {
	if (localInfo.index != 0) {
		if (localInfo.localType == IScript::Request::LOCAL) {
			lua_Debug ar;
			lua_getstack(L, (int)localInfo.level, &ar);
			lua_getinfo(L, "nSltu", &ar);
			f(L, value);
			lua_setlocal(L, &ar, (int)localInfo.index);
		} else if (localInfo.localType == IScript::Request::UPVALUE) {
			assert(localInfo.level != 0);
			refget(L, IScript::Request::Ref(localInfo.level));
			f(L, value);
			lua_setupvalue(L, -2, (int)localInfo.index);
			lua_pop(L, 1);
		}

		localInfo.index = 0;
	} else if (tableLevel != 0) {
		if (key.empty()) {
			int index = IncreaseTableIndex(L);
			f(L, value);
			lua_rawseti(L, -3, index);
		} else {
			lua_pushvalue(L, lua_upvalueindex(0));
			bool isGlobal = lua_compare(L, -1, -3, LUA_OPEQ);
			lua_pop(L, 1);
			if (isGlobal) {
				f(L, value);
				lua_setglobal(L, key.c_str());
			} else {
				lua_pushstring(L, key.c_str());
				f(L, value);
				lua_rawset(L, -4);
			}
			key = "";
		}
	} else {
		f(L, value);
	}
}


template <class F, class C>
inline void Read(lua_State* L, int& tableLevel, int& idx, String& key, IScript::Request::LocalInfo& localInfo, F f, C& value) {
	if (localInfo.index != 0) {
		if (localInfo.localType == IScript::Request::LOCAL) {
			lua_Debug ar;
			lua_getstack(L, (int)localInfo.level, &ar);
			lua_getlocal(L, &ar, (int)localInfo.index);
		} else if (localInfo.localType == IScript::Request::UPVALUE) {
			assert(localInfo.level != 0);
			refget(L, IScript::Request::Ref(localInfo.level));
			lua_getupvalue(L, -2, (int)localInfo.index);
			lua_replace(L, -2);
		}
		value = f(L, -1);
		lua_pop(L, 1);
		localInfo.index = 0;
	} else if (tableLevel != 0) {
		assert(lua_isuserdata(L, -1));
		assert(lua_istable(L, -2));
		if (key.empty()) {
			int index = IncreaseTableIndex(L);
			lua_rawgeti(L, -2, index);
		} else {
			lua_pushstring(L, key.c_str());
			key = "";
			assert(lua_istable(L, -3));
			lua_rawget(L, -3);
		}
		if (!lua_isnil(L, -1)) {
			value = f(L, -1);
		}
		lua_pop(L, 1);
	} else {
		value = f(L, idx++);
	}
}

IScript::Request& ZScriptLua::Request::operator >> (const Skip& skip) {
	assert(GetScript()->GetLockCount() == 1);
	if (tableLevel != 0) {
		assert(lua_isuserdata(L, -1));
		assert(lua_istable(L, -2));
		if (key.empty()) {
			IncreaseTableIndex(L, skip.count);
		}
	} else {
		idx += skip.count;
		assert(idx <= lua_gettop(L));
	}

	return *this;
}


IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::Local&) {
	lua_pushlightuserdata(L, &g_localKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushthread(L);
	assert(lua_istable(L, -2));
	if (lua_rawget(L, -2) == LUA_TNIL) {
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);
		assert(lua_istable(L, -1));
		lua_newtable(L);
		lua_pushthread(L);
		lua_pushvalue(L, -2); // make a copy
		lua_settable(L, -4);
		lua_replace(L, -2);
	}

	lua_remove(L, -2);
	lua_pushlightuserdata(L, (void*)0);
	tableLevel++;

	return *this;
}


IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::Global&) {
	assert(GetScript()->GetLockCount() == 1);
	//	lua_pushvalue(L, lua_upvalueindex(0));
	lua_pushvalue(L, lua_upvalueindex(0));
	// now the top of stack is object table
	// push index
	lua_pushlightuserdata(L, (void*)0);
	tableLevel++;
	return *this;
}


void ZScriptLua::Request::Dereference(IScript::Request::Ref& ref) {
	assert(GetScript()->GetLockCount() == 1);
	if (ref.value != 0) {
		lua_pushlightuserdata(L, &g_refKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		// refget(L, ref);
		// printf("UNREF: %s\n", lua_typename(L, lua_type(L, -1)));
		// lua_pop(L, 1);

		luaL_unref(L, -1, (int)ref.value);
		lua_pop(L, 1);
		DelReference(ref);
		ref.value = 0;
	}
}

inline IScript::Request::Ref refadd(lua_State* L, int index) {
	if (lua_isnil(L, index) || index > lua_gettop(L)) {
		return IScript::Request::Ref(0);
	}

	lua_pushlightuserdata(L, &g_refKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, index > 0 ? index : index - 1);
	// printf("Val type: %s\n", lua_typename(L, lua_type(L, -1)));
	int ref = luaL_ref(L, -2);
	lua_pop(L, 1);
	return IScript::Request::Ref(ref == -1 ? 0 : ref);
}

inline void wrapget(lua_State* L, const IScript::Request::AutoWrapperBase& wrapper) {
	// make a reference in wrap table
	IScript::Request::AutoWrapperBase** ptr = reinterpret_cast<IScript::Request::AutoWrapperBase**>(lua_newuserdata(L, sizeof(IScript::Request::AutoWrapperBase*)));
	*ptr = wrapper.Copy();

	lua_pushlightuserdata(L, &g_wrapKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);

	lua_pushcclosure(L, FunctionProxy, 1);
}

IScript::Request& ZScriptLua::Request::Push() {
	assert(L != state->state);
	// save state
	assert(key.empty());
	lua_pushinteger(L, idx);
	lua_pushinteger(L, tableLevel);
	lua_pushinteger(L, initCount);
	initCount = lua_gettop(L);
	idx = initCount + 1;
	tableLevel = 0;
	return *this;
}

IScript::Request& ZScriptLua::Request::Pop() {
	assert(L != state->state);
	// save state
	assert(key.empty());
	assert(tableLevel == 0);
	lua_settop(L, initCount);
	initCount = (int)lua_tointeger(L, -1);
	tableLevel = (int)lua_tointeger(L, -2);
	idx = (int)lua_tointeger(L, -3);
	lua_pop(L, 3);
	return *this;
}

void ZScriptLua::Request::AddReference(IScript::Request::Ref ref, const String& file, int line) {
	if (ref.value == 0)
		return;

	state->totalReference++;
#ifdef _DEBUG
	char no[64];
	sprintf(no, "%d", line);
	state->debugReferences[ref.value] = file + no;
#endif
}

void ZScriptLua::Request::DelReference(IScript::Request::Ref ref) {
	if (ref.value == 0)
		return;

	state->totalReference--;
#ifdef _DEBUG
	state->debugReferences.erase(ref.value);
#endif
}

IScript::Request& ZScriptLua::Request::operator << (const AutoWrapperBase& wrapper) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, wrapget, wrapper);
	return *this;
}


IScript::Request& ZScriptLua::Request::operator >> (IScript::Request::Ref& ref) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);

	Read(L, tableLevel, idx, key, local, refadd, ref);
	AddReference(ref, "operator >> ", 0);
	return *this;
}

IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::Ref& ref) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, refget, ref);
	return *this;
}


IScript::Request& ZScriptLua::GetDefaultRequest() {
	return *defaultRequest;
}

IScript::Request& ZScriptLua::Request::operator >> (IScript::Request::TableStart& start) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	if (tableLevel == 0) {
		lua_pushvalue(L, idx++); // table
	} else {
		if (key.empty()) {
			int index = IncreaseTableIndex(L);
			lua_rawgeti(L, -2, index);
		} else {
			lua_pushvalue(L, lua_upvalueindex(0));
			bool isGlobal = lua_compare(L, -1, -3, LUA_OPEQ);
			lua_pop(L, 1);

			if (isGlobal) {
				lua_getglobal(L, key.c_str());
			} else {
				lua_pushstring(L, key.c_str());
				lua_rawget(L, -3);
			}

			key = "";
		}
	}

	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		lua_pushlightuserdata(L, &g_dummyKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	start.count = (size_t)lua_rawlen(L, -1);

	lua_pushlightuserdata(L, (void*)0); // index
	tableLevel++;

	assert(lua_istable(L, -2));
	assert(lua_isuserdata(L, -1));

	return *this;
}

IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::TableStart&) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	// add table
	if (tableLevel != 0) {
		if (key.empty()) {
			int index = IncreaseTableIndex(L);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_rawseti(L, -4, index);
		} else {
			lua_pushvalue(L, lua_upvalueindex(0));
			bool isGlobal = lua_compare(L, -1, -3, LUA_OPEQ);
			lua_pop(L, 1);

			lua_newtable(L);
			if (isGlobal) {
				lua_pushvalue(L, -1);
				lua_setglobal(L, key.c_str());
			} else {
				lua_pushstring(L, key.c_str());
				lua_pushvalue(L, -2);
				lua_rawset(L, -5);
			}
			key = "";
		}
	} else {
		lua_newtable(L);
		lua_pushvalue(L, -1);
	}

	// load table
	assert(lua_istable(L, -1));
	lua_pushlightuserdata(L, (void*)0);
	tableLevel++;
	return *this;
}


IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::TableEnd& d) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	assert(tableLevel != 0);
	tableLevel--;
	lua_pop(L, 2);
	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (const IScript::Request::TableEnd&) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	assert(tableLevel != 0);
	lua_pop(L, 2);
	tableLevel--;

	return *this;
}


IScript::Request& ZScriptLua::Request::operator << (double value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, lua_pushnumber, value);
	return *this;
}

inline double tonumber(lua_State* L, int index) {
	return lua_tonumber(L, index);
}

IScript::Request& ZScriptLua::Request::operator >> (double& value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Read(L, tableLevel, idx, key, local, tonumber, value);

	return *this;
}


inline void strwrite(lua_State* L, const String& v) {
	lua_pushlstring(L, v.c_str(), v.size());
}

String strget(lua_State* L, int index) {
	size_t length;
	const char* ptr = lua_tolstring(L, index, &length);
	return String(ptr, length);
}

IScript::Request& ZScriptLua::Request::operator << (const String& v) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, strwrite, v);
	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (String& v) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Read(L, tableLevel, idx, key, local, strget, v);

	return *this;
}

// lua 5.3 now supports 64bit integer
IScript::Request& ZScriptLua::Request::operator << (int64_t value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, lua_pushinteger, value);
	return *this;
}


inline int64_t tointeger(lua_State* L, int index) {
	int isnumber;
	int64_t v = lua_tointegerx(L, index, &isnumber);
	if (isnumber) {
		return v;
	} else {
		return (int64_t)lua_tonumber(L, index);
	}
}

IScript::Request& ZScriptLua::Request::operator >> (int64_t& value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Read(L, tableLevel, idx, key, local, tointeger, value);

	return *this;
}

IScript::Request& ZScriptLua::Request::operator << (bool value) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	Write(L, tableLevel, idx, key, local, lua_pushboolean, value);
	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (bool& value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Read(L, tableLevel, idx, key, local, lua_toboolean, value);

	return *this;
}

IScript::Request& ZScriptLua::Request::operator << (const char* value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Write(L, tableLevel, idx, key, local, lua_pushstring, value);

	return *this;
}

inline const char* tostrptr(lua_State* L, int i) {
	return lua_tostring(L, i);
}

IScript::Request& ZScriptLua::Request::operator >> (const char*& value) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	Read(L, tableLevel, idx, key, local, tostrptr, value);

	return *this;
}

IScript::Request& ZScriptLua::Request::operator << (const IScript::Request::Key& k) {
	assert(GetScript()->GetLockCount() == 1);
	assert(L != state->state);
	key = k.GetKey();
	return *this;
}


IScript::Request::TYPE ZScriptLua::Request::ConvertType(int type) {
	IScript::Request::TYPE target = NIL;
	switch (type) {
	case LUA_TNIL:
		target = NIL;
		break;
	case LUA_TNUMBER:
		target = NUMBER;
		break;
	case LUA_TNUMINT:
		target = INTEGER;
		break;
	case LUA_TSTRING:
		target = STRING;
		break;
	case LUA_TTABLE:
		target = TABLE;
		break;
	case LUA_TFUNCTION:
		target = FUNCTION;
		break;
	case LUA_TUSERDATA:
		target = OBJECT;
		break;
	default:
		target = NIL;
		break;
	}

	return target;
}



IScript::Request& ZScriptLua::Request::operator << (const LocalInfo& localInfo) {
	local = localInfo;
	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (const LocalInfo& localInfo) {
	local = localInfo;
	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (const IScript::Request::Key& y) {
	assert(L != state->state);
	IScript::Request::Key& k = const_cast<IScript::Request::Key&>(y);
	assert(GetScript()->GetLockCount() == 1);
	int type;
	if (tableLevel != 0) {
		// try to read the value
		lua_pushvalue(L, lua_upvalueindex(0));
		bool isGlobal = lua_compare(L, -1, -3, LUA_OPEQ);
		lua_pop(L, 1);


		if (isGlobal) {
			lua_getglobal(L, k.GetKey());
		} else {
			lua_pushstring(L, k.GetKey());
			lua_rawget(L, -3);
		}

		type = lua_type(L, -1);
		lua_pop(L, 1);
		key = k.GetKey();
	} else {
		type = lua_type(L, idx); // read the current one
	}

	k.SetType(ConvertType(type));

	return *this;
}

bool ZScriptLua::Request::IsValid(const BaseDelegate& d) {
	assert(L != state->state);
	// check if already exists
	lua_pushlightuserdata(L, &g_objKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, d.ptr);
	lua_gettable(L, -2);
	lua_replace(L, -2);

	bool valid = !lua_isnil(L, -1);
	lua_pop(L, 1);

	return valid;
}

inline void PushUserdata(lua_State* L, const IScript::BaseDelegate& b) {
	IScript::BaseDelegate& d = const_cast<IScript::BaseDelegate&>(b);
	void* ptr = d.ptr;
	int t = lua_gettop(L);
	// check if already exists
	lua_pushlightuserdata(L, &g_objKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, ptr);
	lua_gettable(L, -2);
	lua_replace(L, -2);

	if (lua_isnil(L, -1)) {
		d.isNewRegistered = true;
		lua_pop(L, 1);
		void* p = lua_newuserdata(L, sizeof(void*));
		memcpy(p, &ptr, sizeof(void*));
		lua_pushlightuserdata(L, &g_bindKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_setmetatable(L, -2);
		lua_pushlightuserdata(L, &g_objKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L, ptr);
		lua_pushvalue(L, -3);
		lua_settable(L, -3);
		lua_pop(L, 1);
	}

	assert(lua_type(L, -1) == LUA_TUSERDATA);
	int m = lua_gettop(L);
	assert(m == t + 1);
}

inline IScript::Object* ToUserdata(lua_State* L, int idx) {
	IScript::Object** ptr = reinterpret_cast<IScript::Object**>(lua_touserdata(L, idx));
	if (ptr == nullptr) {
		return nullptr;
	} else {
		return *ptr;
	}
}

IScript::Request::Ref ZScriptLua::Request::ReferenceWithDebugInfo(const IScript::BaseDelegate& d, const String& tag, int line) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	int idx = GetCount();
	*this << d;
	IScript::Request::Ref ref = refadd(L, -1);
	lua_pop(L, 1);
	AddReference(ref, tag, line);
	assert(idx == GetCount());

	return ref;
}


IScript::Request::Ref ZScriptLua::Request::ReferenceWithDebugInfo(const IScript::Request::Ref& d, const String& tag, int line) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	int idx = GetCount();
	*this << d;
	IScript::Request::Ref ref = refadd(L, -1);
	lua_pop(L, 1);

	assert(idx == GetCount());
	AddReference(ref, tag, line);

	return ref;
}

IScript::Request& ZScriptLua::Request::operator << (const IScript::BaseDelegate& d) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	assert(d.ptr != nullptr);
	// write
	Write(L, tableLevel, idx, key, local, PushUserdata, d);
	if (d.isNewRegistered) {
		d.ptr->ScriptInitialize(*this);
	}

	return *this;
}

IScript::Request& ZScriptLua::Request::operator >> (IScript::BaseDelegate& d) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	Read(L, tableLevel, idx, key, local, ToUserdata, d.ptr);

	return *this;
}

inline void fake_pushnil(lua_State* L, const IScript::Request::Nil&) {
	lua_pushnil(L);
}

IScript::Request& ZScriptLua::Request::operator << (const Nil& n) {
	assert(L != state->state);
	assert(GetScript()->GetLockCount() == 1);
	Write(L, tableLevel, idx, key, local, fake_pushnil, n);
	return *this;
}

IScript::Request::TYPE ZScriptLua::Request::GetCurrentType() {
	int type = LUA_TNIL;
	if (tableLevel != 0) {
		assert(lua_isuserdata(L, -1));
		assert(lua_istable(L, -2));
		if (key.empty()) {
			int index = IncreaseTableIndex(L, 0) + 1;
			lua_rawgeti(L, -2, index);
		} else {
			lua_pushstring(L, key.c_str());
			assert(lua_istable(L, -3));
			lua_rawget(L, -3);
		}
		if (!lua_isnil(L, -1)) {
			type = lua_type(L, -1);
		}
		lua_pop(L, 1);
	} else {
		type = lua_type(L, idx);
	}

	return ConvertType(type);
}

std::vector<IScript::Request::Key> ZScriptLua::Request::Enumerate() {
	assert(L != state->state);
	std::vector<Key> keys;
	assert(GetScript()->GetLockCount() == 1);
	// from lua document
	lua_pushnil(L);  // first key
	while (lua_next(L, -3) != 0) { // it's -3, [table, index, frontKey]
		int type = lua_type(L, -2);
		if (type == LUA_TSTRING) {
			keys.push_back(std::move(Key(lua_tostring(L, -2), ConvertType(lua_type(L, -1)))));
		}
		lua_pop(L, 1);
	}

	return keys;
}

static int lastlevel(lua_State *L) {
	lua_Debug ar;
	int li = 1, le = 1;
	/* find an upper bound */
	while (lua_getstack(L, le, &ar)) { li = le; le *= 2; }
	/* do a binary search */
	while (li < le) {
		int m = (li + le) / 2;
		if (lua_getstack(L, m, &ar)) li = m + 1;
		else le = m;
	}
	return le - 1;
}

void Assign(String& target, const char* str) {
	if (target != str) {
		target = str;
	}
}

void SetLocalInfo(lua_State* L, std::vector<IScript::Request::LocalInfo>& debugInfos, const char* name, size_t index, size_t itemIndex, bool pop, IScript::Request::LOCALTYPE localType) {
	assert(index <= debugInfos.size());
	if (index == debugInfos.size()) {
		debugInfos.push_back(IScript::Request::LocalInfo());
	}

	IScript::Request::LocalInfo& debugInfo = debugInfos.back();
	assert(index < debugInfos.size());
	if (pop) {
		debugInfo.type = ZScriptLua::Request::ConvertType(lua_type(L, -1));
		lua_pop(L, 1);
	} else {
		debugInfo.type = IScript::Request::NIL;
	}

	debugInfo.index = itemIndex + 1;
	debugInfo.name = name;
	debugInfo.level = 0;
	debugInfo.localType = localType;
}

void SetStackInfo(lua_State* L, std::vector<IScript::Request::StackInfo>& debugInfos, lua_Debug& ar, size_t index) {
	assert(index <= debugInfos.size());
	if (index == debugInfos.size()) {
		debugInfos.push_back(IScript::Request::StackInfo());
	}
	IScript::Request::StackInfo& debugInfo = debugInfos.back();
	assert(index < debugInfos.size());
	Assign(debugInfo.name, ar.name == nullptr ? "" : ar.name);
	Assign(debugInfo.namewhat, ar.namewhat == nullptr ? "" : ar.namewhat);
	Assign(debugInfo.source, ar.source == nullptr ? "" : ar.source);
	debugInfo.range.first = ar.linedefined;
	debugInfo.range.second = ar.lastlinedefined;
	debugInfo.currentLine = ar.currentline;
}


void ZScriptLua::Request::Debug(std::vector<StackInfo>& debugInfos, std::vector<LocalInfo>& localInfos, const Ref* func) {
	assert(GetScript()->GetLockCount() == 1);

	assert(L != state->state);
	/*
	char buf[256];
	sprintf(buf, "Info size: %d\n", localInfos.size());
	OutputDebugStringA(buf);

	if (localInfos.size() == 1) {
		int a = 0;
	}*/

	lua_Debug ar;
	size_t index = 0;
	size_t localIndex = 0;
	size_t upvalueIndex = 0;
	const char* name = nullptr;
	if (func == nullptr) {
		int level = lastlevel(L);
		while (level >= 0) {
			lua_getstack(L, level--, &ar);
			lua_getinfo(L, "nSltu", &ar);

			// removes all C calls
			if (ar.source != nullptr && strcmp(ar.source, "=[C]") != 0) {
				SetStackInfo(L, debugInfos, ar, index++);
			}
		}

		// get locals
		while ((name = lua_getlocal(L, &ar, (int)localIndex + 1)) != nullptr) {
			SetLocalInfo(L, localInfos, name, localIndex, localIndex, true, IScript::Request::LOCAL);
			localIndex++;
		}


	} else {
		refget(L, *func);

		// get locals
		while ((name = lua_getlocal(L, nullptr, (int)localIndex + 1)) != nullptr) {
			SetLocalInfo(L, localInfos, name, localIndex, localIndex, false, IScript::Request::PARAM);
			localIndex++;
		}

		int top = lua_gettop(L);
		// get upvalues
		while ((name = lua_getupvalue(L, -1, (int)upvalueIndex + 1))) {
			SetLocalInfo(L, localInfos, name, localIndex, upvalueIndex, true, IScript::Request::UPVALUE);
			localIndex++;
			upvalueIndex++;
		}

		assert(lua_gettop(L) == top);
		lua_getinfo(L, ">nSltu", &ar);
		SetStackInfo(L, debugInfos, ar, 0);
		index = 1;
	}


	debugInfos.resize(index);
	localInfos.resize(localIndex);
}


ZScriptLua::Request::Request(ZScriptLua* s, lua_State* l) : state(s), key(""), L(l), idx(1), tableLevel(0) {
	assert(GetScript()->GetLockCount() == 1);
	if (L == nullptr) {
		L = lua_newthread(s->GetState());
		// make reference so gc won't collect this request
		ref = refadd(s->GetState(), -1);
		lua_pop(s->GetState(), 1);

		AddReference(ref, "Request", 0);
	} else {
		ref.value = 0;
	}

	// make reference so gc won't collect this request's defer stack
	initCount = lua_gettop(L);
	local.index = 0;
}


IScript::Request::Ref ZScriptLua::Request::Load(const String& script, const String& path) {
	assert(GetScript()->GetLockCount() == 1);
	// lua_pushlightuserdata(L, &g_refKey);
	// lua_rawget(L, LUA_REGISTRYINDEX);
	// assert(lua_istable(L, -1));
	IScript::Request::Ref ref;
	int error = luaL_loadbuffer(L, script.c_str(), script.length(), path.c_str());

	if (error != 0) {
		Error(lua_tostring(L, -1));
		lua_pop(L, 1);
		return ref;
	}

	assert(lua_isfunction(L, -1));
	ref = refadd(L, -1);
	lua_pop(L, 1);

	AddReference(ref, "Load", 0);
	return ref;
}


ZScriptLua::Request::~Request() {
	assert(GetScript()->GetLockCount() == 1);
	if (ref.value != 0) {
		Dereference(ref);
	}
}

lua_State* ZScriptLua::GetState() const {
	return state;
}

TObject<IReflect>& ZScriptLua::Request::operator () (IReflect& reflect) {
	ReflectClass(Request);

	return *this;
}

int ZScriptLua::GetInitDeferCount() const {
	return initCountDefer;
}

void ZScriptLua::SetInitDeferCount(int count) {
	initCountDefer = count;
}


lua_State* ZScriptLua::GetDeferState() const {
	return deferState;
}

bool ZScriptLua::TryLock() {
	assert(GetLockCount() == 1);
	return ISyncObject::TryLock();
}

void ZScriptLua::DoLock() {
	ISyncObject::DoLock();
	assert(GetLockCount() == 1);
}

void ZScriptLua::UnLock() {
	assert(GetLockCount() == 1);
	ISyncObject::UnLock();
}

IScript* ZScriptLua::NewScript() const {
	return new ZScriptLua(threadApi);
}