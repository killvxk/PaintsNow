#if defined(_WIN32)

#include "ComDef.h"
#include "ComBridge.h"
#include "ComDispatch.h"
#include "../LibLoader.h"

using namespace PaintsNow;
using namespace PaintsNow::NsRayForce;


ComBridge::ComBridge(IThread& thread) : Bridge(thread) {
	InitComReflectMap(typeMap, reflectMap);
}

TObject<IReflect>& ComBridge::operator () (IReflect& reflect) {
	ReflectClass(ComBridge);

	return *this;
}

static ComDispatch* CreateFromCLSID(IScript::Request& request, ComBridge* host, const CLSID& clsid) {
	ComDispatch* result = nullptr;
	IClassFactory* factory;
	if (SUCCEEDED(CoGetClassObject(clsid, CLSCTX_INPROC_SERVER, nullptr, IID_IClassFactory, (LPVOID*)&factory))) {
		IDispatch* dispatch;
		if (SUCCEEDED(factory->CreateInstance(nullptr, IID_IDispatch, (LPVOID*)&dispatch))) {
			result = new ComDispatch(request, host, dispatch);
		}
		factory->Release();
	}

	return result;
}

IReflectObject* ComBridge::Create(IScript::Request& request, IArchive& archive, const String& path, const String& data) {
	// check ext name
	size_t pos = path.find("!");
	if (pos == (size_t)-1 || pos < 4) {
		return nullptr;
	}

	String className = path.substr(pos + 1);
	String ext = path.substr(pos - 4, 4);
	ComDispatch* result = nullptr;

	// Is CLSID?
	if (path[0] == '{') {
		CComBSTR bstr = FromCoreString(path);
		CLSID clsid;
		if (SUCCEEDED(::CLSIDFromString(bstr, &clsid))) {
			result = CreateFromCLSID(request, this, clsid);
		}
	} else if (_stricmp(ext.c_str(), ".exe") == 0 || _stricmp(ext.c_str(), ".dll") == 0 || _stricmp(ext.c_str(), ".tlb") == 0) {
		String fullPath = path.substr(0, pos);
		String root = archive.GetMetaData("root");
		if (!root.empty()) {
			fullPath = root + fullPath;
		}

		ITypeLib* type;
		HRESULT hr;
		CComBSTR lib = FromCoreString(fullPath);
		if (SUCCEEDED(hr = ::LoadTypeLibEx(lib, REGKIND_REGISTER, &type))) {
			size_t count = type->GetTypeInfoCount();
			// try to add all possible classes
			for (size_t i = 0; i < count && result == nullptr; i++) {
				ITypeInfo* info;
				if (SUCCEEDED(type->GetTypeInfo((UINT)i, &info))) {
					CComBSTR bstrName;
					info->GetDocumentation(MEMBERID_NIL, &bstrName, nullptr, nullptr, nullptr);
					if (className == ToCoreString(bstrName)) {
						TYPEATTR* attr;
						if (SUCCEEDED(info->GetTypeAttr(&attr))) {
							if ((result = CreateFromCLSID(request, this, attr->guid)) == nullptr) {
								// do not save return value
								request.Push();
								HMODULE module = (HMODULE)LibLoader::DynamicLoadLibrary(SystemToUtf8(String((const char*)(BSTR)lib, ::SysStringByteLen((BSTR)lib))), "ComBridge", this);
								request.Pop();

								if (module != nullptr && module != (void*)-1) {
									typedef HRESULT(_stdcall*PFNDLLGETCLASSOBJECT)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
									PFNDLLGETCLASSOBJECT DllGetClassObject = (PFNDLLGETCLASSOBJECT)::GetProcAddress(module, "DllGetClassObject");
									if (DllGetClassObject != nullptr) {
										IClassFactory* factory;
										if (SUCCEEDED(hr = DllGetClassObject(attr->guid, IID_IClassFactory, (void**)&factory))) {
											IDispatch* dispatch = nullptr;
											if (SUCCEEDED(hr = factory->CreateInstance(nullptr, IID_IDispatch, (void**)&dispatch))) {
												result = new ComDispatch(request, this, dispatch);
											}

											factory->Release();
										}
									}
								}
							}
							info->ReleaseTypeAttr(attr);
						}
					}
					info->Release();
				}
			}
			type->Release();
		}
	}

	return result;
}

void ComBridge::Call(IReflectObject* object, const TProxy<>* p, IScript::Request& request) {
	ComDispatch* dispatch = static_cast<ComDispatch*>(object);
	dispatch->Call(p, request);
}


std::map<Unique, ZScriptReflect::Type>& ComBridge::GetReflectMap() {
	return reflectMap;
}

std::map<size_t, Unique>& ComBridge::GetTypeMap() {
	return typeMap;
}

#endif // _WIN32