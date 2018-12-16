// IScript.h
// By PaintDream (paintdream@paintdream.com)
// 2013-7-24
//

#ifndef __ISCRIPT_H__
#define __ISCRIPT_H__

#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "../Common/ZMemoryStream.h"
#include "../Template/TVector.h"
#include "../Template/TProxy.h"
#include "../Template/TAlgorithm.h"
#include "IThread.h"
#include <vector>
#include <list>
#include <string>
#include <cassert>
#include <utility>


namespace PaintsNow {
	class IScript : public ISyncObject {
	public:
		class Request;
		class Object : public IReflectObjectComplex {
		public:
			Object();
			virtual ~Object();
			virtual void ScriptInitialize(Request& request);
			virtual void ScriptUninitialize(Request& request);
		};

		class Library : public Object, public ISyncObject {
		public:
			Library(IThread& threadApi);
			virtual ~Library();
			virtual bool Initialize();
			virtual void Uninitialize();
			virtual void Require(Request& request);
			virtual void ScriptUninitialize(Request& request);
			virtual void RequestInspect(Request& request);
			static void InspectObject(Request& request, IReflectObject& object);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};

		struct BaseDelegate {
			union {
				Object* ptr;
				uint64_t alignment1;
			};
			union {
				struct {
					bool isNative;
					bool isNewRegistered;
				};

				uint32_t alignment2;
			};

			BaseDelegate(Object* p = nullptr, bool isNative = true);
			~BaseDelegate();
			BaseDelegate operator() (Object* p);
			bool IsNative() const;
		};

		template <class T>
		struct Delegate : public BaseDelegate {
		protected:
			inline T* GetNative() const {
				static Unique u = UniqueType<T>::Get();
				if (ptr != nullptr && (u == ptr->GetUnique() || ptr->QueryInterface(UniqueType<T>()) != nullptr)) {
					return static_cast<T*>(ptr);
				} else {
					return nullptr;
				}
			}

		public:
			inline const String& GetTypeName() const {
				static Unique u = UniqueType<T>::Get();
				return u.info->typeName;
			}

			inline T* Get() const {
				assert(IsNative());
				return IsNative() ? GetNative() : nullptr;
			}

			inline T* operator -> () const {
				return Get();
			}

			inline operator bool() const {
				return GetNative() != nullptr;
			}
		};

		// thread support for requests
		IScript(IThread& api);
		virtual ~IScript();

		class Request : public IReflectObjectComplex {
		public:
			virtual ~Request();
			virtual void DoLock();
			virtual void UnLock();

			struct Skip {
				int count;
				explicit Skip(int k);
				Skip operator () (int k);
			};

			struct PlaceHolder {};

			enum TYPE { NIL, NUMBER, INTEGER, STRING, TABLE, FUNCTION, OBJECT, ANY };
			struct Ref {
				Ref(size_t i = 0);
				operator bool () const {
					return value != 0;
				}

				size_t value;
			};

			struct StackInfo {
				String name;
				String namewhat;
				String source;
				std::pair<int, int> range;
				int currentLine;
			};

			enum LOCALTYPE { LOCAL, PARAM, UPVALUE };
			struct LocalInfo {
				String name;
				size_t index;
				size_t level;
				TYPE type;
				LOCALTYPE localType;
			};

			class AutoWrapperBase {
			public:
				virtual ~AutoWrapperBase() {}
				virtual bool IsSync() const;
				virtual void Execute(Request& request) const = 0;
				virtual AutoWrapperBase* Copy() const = 0;
			};

			class Sync : public AutoWrapperBase {
			public:
				virtual bool IsSync() const;
				virtual void Execute(Request& request) const;
				virtual AutoWrapperBase* Copy() const;
			};

			class Async : public AutoWrapperBase {
			public:
				virtual bool IsSync() const;
				virtual void Execute(Request& request) const;
				virtual AutoWrapperBase* Copy() const;
			};

			class PackBase {
			public:
				virtual ~PackBase() {}
				virtual bool Serialize(String& data) const = 0;
				virtual bool Deserialize(const String& data) = 0;
			};

			template <class T, size_t N = 4096>
			class Pack : public PackBase {
			public:
				virtual bool Serialize(String& data) const override {
					ZMemoryStream stream(N);
					bool result = stream << object;
					data.assign(reinterpret_cast<const char*>(stream.GetBuffer()), stream.GetTotalLength());
					return result;
				}

				virtual bool Deserialize(const String& data) override {
					size_t len = data.length();
					ZMemoryStream stream(len);
					stream.Write(data.data(), len);
					return stream >> object;
				}
			
				T object;
			};

#if (defined(_MSC_VER) && _MSC_VER < 1800) || defined(COMPATIBLE_PROXY)
			template <class A = Void, class B = Void, class C = Void, class D = Void, class E = Void, class F = Void, class G = Void, class H = Void, class I = Void, class J = Void, class K = Void, class L = Void, class M = Void, class N = Void, class O = Void>
			class AutoWrapper : public AutoWrapperBase, public TWrapper<void, Request&, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O> {
			public:
				AutoWrapper(const TWrapper<void, Request&, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O>& m) : TWrapper<void, Request&, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O>(m) {}

				virtual AutoWrapperBase* Copy() const {
					return new AutoWrapper(*this);
				}

				void InvokeRoutine(Request& request) {
					std::remove_reference<A>::type a; std::remove_reference<B>::type b; std::remove_reference<C>::type c; std::remove_reference<D>::type d; std::remove_reference<E>::type e;
					std::remove_reference<F>::type f; std::remove_reference<G>::type g; std::remove_reference<H>::type h; std::remove_reference<I>::type i; std::remove_reference<J>::type j;
					std::remove_reference<K>::type k; std::remove_reference<L>::type l; std::remove_reference<M>::type m; std::remove_reference<N>::type n; std::remove_reference<O>::type o;

				//	request.AssertUnlocked();
					switch (GetCount()) {
					case 0:
						assert(false);
						break;
					case 1:
						(*this)(request);
						break;
					case 2:
						request.DoLock();
						request >> a;
						request.UnLock();
						(*this)(request, a);
						break;
					case 3:
						request.DoLock();
						request >> a >> b;
						request.UnLock();
						(*this)(request, a, b);
						break;
					case 4:
						request.DoLock();
						request >> a >> b >> c;
						request.UnLock();
						(*this)(request, a, b, c);
						break;
					case 5:
						request.DoLock();
						request >> a >> b >> c >> d;
						request.UnLock();
						(*this)(request, a, b, c, d);
						break;
					case 6:
						request.DoLock();
						request >> a >> b >> c >> d >> e;
						request.UnLock();
						(*this)(request, a, b, c, d, e);
						break;
					case 7:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f);
						break;
					case 8:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g);
						break;
					case 9:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h);
						break;
					case 10:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i);
						break;
					case 11:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j);
						break;
					case 12:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j, k);
						break;
					case 13:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j, k, l);
						break;
					case 14:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j, k, l, m);
						break;
					case 15:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m >> n;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j, k, l, m, n);
						break;
					case 16:
						request.DoLock();
						request >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m >> n >> o;
						request.UnLock();
						(*this)(request, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);
						break;
					}

				//	request.AssertUnlocked();
				}

				virtual void Execute(Request& request) const {
					const TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>& >& dispatcher = request.GetScript()->GetDispatcher();
					if (dispatcher) {
						dispatcher(request, GetHost(), GetCount(), Wrap(const_cast<AutoWrapper*>(this), &AutoWrapper::InvokeRoutine));
					} else {
						(const_cast<AutoWrapper*>(this))->InvokeRoutine(request);
					}
				}
			};

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O>
			static AutoWrapper<A, B, C, D, E, F, G, H, I, J, K, L, M, N, O> Adapt(const TWrapper<void, Request&, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O>& wrapper) {
				return AutoWrapper<A, B, C, D, E, F, G, H, I, J, K, L, M, N, O>(wrapper);
			}

			template <class A>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a) {
				*this << a;
				return Call(defer, ref);	
			}

			template <class A, class B>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b) {
				*this << a << b;
				return Call(defer, ref);	
			}

			template <class A, class B, class C>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c) {
				*this << a << b << c;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d) {
				*this << a << b << c << d;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e) {
				*this << a << b << c << d << e;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f) {
				*this << a << b << c << d << e << f;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g) {
				*this << a << b << c << d << e << f << g;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h) {
				*this << a << b << c << d << e << f << g << h;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i) {
				*this << a << b << c << d << e << f << g << h << i;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j) {
				*this << a << b << c << d << e << f << g << h << i << j;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k) {
				*this << a << b << c << d << e << f << g << h << i << j << k;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k, const L& l) {
				*this << a << b << c << d << e << f << g << h << i << j << k << l;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k, const L& l, const M& m) {
				*this << a << b << c << d << e << f << g << h << i << j << k << l << m;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k, const L& l, const M& m, const N& n) {
				*this << a << b << c << d << e << f << g << h << i << j << k << l << m << n;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k, const L& l, const M& m, const N& n, const O& o) {
				*this << a << b << c << d << e << f << g << h << i << j << k << l << m << n << o;
				return Call(defer, ref);	
			}

			template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class AA>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j, const K& k, const L& l, const M& m, const N& n, const O& o, const AA& aa) {
				*this << a << b << c << d << e << f << g << h << i << j << k << l << m << n << o << aa;
				return Call(defer, ref);	
			}

#else
			template <typename... Args>
			class AutoWrapper : public AutoWrapperBase, public TWrapper<void, Request&, Args...> {
			public:
				AutoWrapper(const TWrapper<void, Request&, Args...>& m) : TWrapper<void, Request&, Args...>(m) {}
				virtual AutoWrapperBase* Copy() const {
					return new AutoWrapper(*this);
				}

				template <typename T, size_t... I>
				void Apply(Request& request, T& arg, seq<I...>) const {
					// request.AssertUnlocked();
					(*this)(request, std::get<I>(arg)...);
					// request.AssertUnlocked();
				}

				template <typename T, size_t index>
				struct Reader {
					void operator () (Request& request, T& arg) {
						request >> std::get<std::tuple_size<T>::value - index>(arg);
						Reader<T, index - 1>()(request, arg);
					}
				};

				template <typename T>
				struct Reader<T, 0> {
					void operator () (Request& request, T& arg) {}
				};

				void InvokeRoutine(Request& request) const {
					std::tuple<typename std::remove_reference<Args>::type ...> arg;
					request.DoLock();
					Reader<decltype(arg), sizeof...(Args)>()(request, arg);
					request.UnLock();

					Apply(request, arg, gen_seq<sizeof...(Args)>());
				}

				virtual void Execute(Request& request) const {
					const TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>& >& dispatcher = request.GetScript()->GetDispatcher();
					if (dispatcher) {
						dispatcher(request, TWrapper<void, Request&, Args...>::GetHost(), TWrapper<void, Request&, Args...>::GetCount(), Wrap(this, &AutoWrapper::InvokeRoutine));
					} else {
						InvokeRoutine(request);
					}
				}
			};

			template <typename... Args>
			static AutoWrapper<Args...> Adapt(const TWrapper<void, Request&, Args...>& wrapper) {
				return AutoWrapper<Args...>(wrapper);
			}

			template <typename First, typename... Args>
			bool Call(const AutoWrapperBase& defer, const IScript::Request::Ref& ref, const First& first, Args&&... args) {
				*this << first;
				return Call(defer, ref, std::forward<Args>(args)...);
			}
#endif

			struct Nil {};
			struct Global {};
			struct Local {};
			struct TableStart { size_t count; };
			struct TableEnd {};
			struct LocalStart {};
			struct LocalEnd {};

			class Key : public IReflectObjectComplex {
			public:
				TObject<IReflect>& operator () (IReflect& reflect);
				virtual TYPE GetType() const;
				virtual void SetType(TYPE type);
				virtual const char* GetKey() const;
				virtual void SetKey(const char* k);
				Key operator () (const String& k);
				Key(const String& k = "", TYPE t = NIL);

			private:
				TYPE type;
				String key;
			};

			// IScript* operator -> ();
			virtual int GetCount() = 0;
			virtual IScript* GetScript() = 0;
			virtual void QueryInterface(const TWrapper<void, Request&, IReflectObject&, const Ref&>& callback, IReflectObject& target, const Ref& g);
			virtual bool Call(const AutoWrapperBase& defer, const Request::Ref& g) = 0;

			virtual void Debug(std::vector<StackInfo>& debugInfo, std::vector<LocalInfo>& localInfos, const Ref* func = nullptr) = 0;
			virtual std::vector<Key> Enumerate() = 0;
			virtual TYPE GetCurrentType() = 0;
			virtual Request::Ref Load(const String& script, const String& pathname = String()) = 0;
			virtual Request& Push() = 0;
			virtual Request& Pop() = 0;
			virtual Request& CleanupIndex() = 0;
			inline Request& operator >> (PlaceHolder ph) { return *this; }
			virtual Request& operator >> (Ref&) = 0;
			virtual Request& operator << (const Ref&) = 0;
			virtual Request& operator << (const Nil&) = 0;
			virtual Request& operator << (const BaseDelegate&) = 0;
			virtual Request& operator >> (BaseDelegate&) = 0;
			virtual Request& operator << (const Global&) = 0;
			virtual Request& operator << (const Local&) = 0;
			virtual Request& operator << (const TableStart&) = 0;
			virtual Request& operator >> (TableStart&) = 0;
			virtual Request& operator << (const TableEnd&) = 0;
			virtual Request& operator >> (const TableEnd&) = 0;
			virtual Request& operator << (const Key&) = 0;
			virtual Request& operator >> (const Key&) = 0;
			virtual Request& operator << (const LocalInfo&) = 0;
			virtual Request& operator >> (const LocalInfo&) = 0;
			virtual Request& operator << (double value) = 0;
			virtual Request& operator >> (double& value) = 0;
			virtual Request& operator << (const String& str) = 0;
			virtual Request& operator >> (String& str) = 0;
			virtual Request& operator << (const char* str) = 0;
			virtual Request& operator >> (const char*& str) = 0;
			virtual Request& operator << (bool b) = 0;
			virtual Request& operator >> (bool& b) = 0;
			virtual Request& operator << (const AutoWrapperBase& wrapper) = 0;
			virtual Request& operator << (int64_t u) = 0;
			virtual Request& operator >> (int64_t& u) = 0;
			virtual Request& operator >> (const Skip& skip) = 0;

			virtual bool IsValid(const BaseDelegate& d) = 0;

			#define MakeReference(d) ReferenceWithDebugInfo(d, __FILE__, __LINE__)

			virtual Ref ReferenceWithDebugInfo(const BaseDelegate& d, const String& tag, int line) = 0;
			virtual Ref ReferenceWithDebugInfo(const Ref& d, const String& tag, int line) = 0;
			virtual TYPE GetReferenceType(const Ref& d) = 0;
			virtual void Dereference(Ref& ref) = 0;
			virtual void DebugPrint() = 0;
			virtual void Error(const String& msg);
			virtual Request& MoveVariables(Request& target, size_t count) = 0;

			virtual Request& operator >> (int8_t& t);
			virtual Request& operator << (int8_t t);
			virtual Request& operator >> (int16_t& t);
			virtual Request& operator << (int16_t t);
			virtual Request& operator >> (int32_t& t);
			virtual Request& operator << (int32_t t);
			virtual Request& operator >> (uint8_t& t);
			virtual Request& operator << (uint8_t t);
			virtual Request& operator >> (uint16_t& t);
			virtual Request& operator << (uint16_t t);
			virtual Request& operator >> (uint32_t& t);
			virtual Request& operator << (uint32_t t);

			virtual Request& operator >> (uint64_t& t);
			virtual Request& operator << (uint64_t t);
#ifdef _MSC_VER
			virtual Request& operator >> (long& t);
			virtual Request& operator << (long t);
			virtual Request& operator >> (unsigned long& t);
			virtual Request& operator << (unsigned long t);
#endif

			virtual Request& operator >> (Void*& t);
			virtual Request& operator << (Void* t);
			virtual Request& operator << (Library& module);
			virtual Request& operator >> (float& value);

			virtual Request& operator >> (IReflectObject& reflectObject);
			virtual Request& operator << (const IReflectObject& reflectObject);
			virtual Request& operator >> (PackBase& reflectObject);
			virtual Request& operator << (const PackBase& reflectObject);
			virtual Request& operator << (const Skip& skip);

			Request& operator >> (Void&);
			Request& operator << (const Void&);

			template <class T>
			Request& operator >> (std::vector<T>& vec) {
				Request& request = *this;
				TableStart ts;
				static TableEnd endtable;
				request >> ts;
				vec.resize(ts.count);
				for (size_t i = 0; i < ts.count; i++) {
					request >> vec[i];
				}
				request >> endtable;
				return request;
			}

			template <class T>
			Request& operator >> (std::list<T>& vec) {
				Request& request = *this;
				TableStart ts;
				static TableEnd endtable;
				request >> ts;
				for (size_t i = 0; i < ts.count; i++) {
					vec.push_back(T());
					request >> vec.back();
				}
				request >> endtable;
				return request;
			}

			template <class T, size_t n>
			Request& operator >> (TVector<T, n>& vec) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request >> begintable;
				for (size_t i = 0; i < n; i++) {
					request >> vec[i];
				}
				request >> endtable;
				return request;
			}

			template <class T>
			Request& operator >> (std::pair<T, T>& pair) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request >> begintable >> pair.first >> pair.second >> endtable;
				return request;
			}

			template <class T>
			Request& operator << (std::vector<T>& vec) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request << begintable;
				for (size_t i = 0; i < vec.size(); i++) {
					request << vec[i];
				}
				request << endtable;
				return request;
			}

			template <class T>
			Request& operator << (std::list<T>& vec) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request << begintable;
				for (typename std::list<T>::iterator it = vec.begin(); it != vec.end(); ++it) {
					request << *it;
				}
				request << endtable;
				return request;
			}

			template <class T, size_t n>
			Request& operator << (const TVector<T, n>& vec) {
				static TableStart begintable;
				static TableEnd endtable;
				Request& request = *this;
				request << begintable;
				for (size_t i = 0; i < n; i++) {
					request << vec[i];
				}
				request << endtable;
				return request;
			}

			template <class T>
			Request& operator << (const std::pair<T, T>& pair) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request << begintable << pair.first << pair.second << endtable;
				return request;
			}

			template <class T, size_t n, size_t m>
			Request& operator << (const TMatrix<T, n, m>& mat) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request << begintable;
				for (size_t i = 0; i < n; i++) {
					request << begintable;
					for (size_t j = 0; j < m; j++) {
						request << mat(i, j);
					}
					request << endtable;
				}
				request << endtable;

				return request;
			}

			template <class T, size_t n, size_t m>
			Request& operator >> (TMatrix<T, n, m>& mat) {
				Request& request = *this;
				static TableStart begintable;
				static TableEnd endtable;
				request >> begintable;
				for (size_t i = 0; i < n; i++) {
					request >> begintable;
					for (size_t j = 0; j < m; j++) {
						request >> mat(i, j);
					}
					request >> endtable;
				}
				request >> endtable;

				return request;
			}
		};

		class MetaLibrary : public MetaNodeBase {
		public:
			MetaLibrary(const String& name = "");
			MetaLibrary operator = (const String& name);

			template <class T, class D>
			inline const MetaLibrary& operator () (T* t, D* d) const {
				return *this; // do nothing
			}

			template <class T, class D>
			struct RealType {
				typedef MetaLibrary Type;
			};

			typedef MetaLibrary VC6Type;

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			const String& name;
		};

		class MetaMethod : public MetaNodeBase {
		public:
			MetaMethod(const String& key = "");
			virtual ~MetaMethod();
			MetaMethod operator = (const String& key);

			struct TypedBase : public MetaNodeBase {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				virtual Request::AutoWrapperBase* CreateWrapper() const = 0;
				virtual Request& Register(Request& request, const String& defName) const = 0;
				String name;
			};

			// For vc6 compat
			class VC6Type : public TypedBase {
			public:
				VC6Type(const VC6Type& rhs) {
					name.~String();
					memcpy(this, &rhs, sizeof(*this));
					new (&name) String(rhs.name);
				}

				virtual Request& Register(Request& request, const String& defName) const {
					assert(false);
					return request;
				}

				virtual Request::AutoWrapperBase* CreateWrapper() const {
					assert(false);
					return nullptr;
				}

				void* pointer;
				void* member;
			};

			template <class T, class D>
			class Typed : public TypedBase {
			public:
				Typed(const String& n, T* p, D* m) : pointer(p), member(m) {
					name = n;
				}

				virtual Request& Register(Request& request, const String& defName) const {
					request << Request::Key(name.empty() ? defName : name) << Request::Adapt(Wrap(pointer, *member));

					return request;
				}

				virtual Request::AutoWrapperBase* CreateWrapper() const {
					return Request::Adapt(Wrap(pointer, *member)).Copy();
				}

				T* pointer;
				D* member;
			};

#if defined(_MSC_VER) && _MSC_VER <= 1200
			template <class T, class D>
			VC6Type operator () (T* pointer, D* member) const {
				Typed<T, D> typed(key, pointer, member);
				return *(reinterpret_cast<VC6Type*>(&typed));
			}
#else
			template <class T, class D>
			Typed<T, D> operator () (T* pointer, D* member) const {
				return Typed<T, D>(key, pointer, member);
			}
#endif

			template <class T, class D>
			struct RealType {
				typedef Typed<T, D> Type;
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			String key;
		};

		class MetaVariable : public MetaNodeBase {
		public:
			MetaVariable(const String& key = "");
			virtual ~MetaVariable();
			MetaVariable operator = (const String& key);

			class TypedBase : public MetaNodeBase {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				virtual Request& Read(Request& request, bool hasKey, const String& defName, void* overrideObject) = 0;
				virtual Request& Write(Request& request, bool hasKey, const String& defName, void* overrideObject) = 0;
				String name;
			};

			// For vc6 compat
			class VC6Type : public TypedBase {
			public:
				VC6Type(const VC6Type& rhs) {
					name = rhs.name;
				}

				virtual MetaNodeBase* Copy() const {
					assert(false);
					return nullptr;
				}
				virtual Request& Read(Request& request, bool hasKey, const String& defName, void* overrideObject) {
					assert(false);
					return request;
				}

				virtual Request& Write(Request& request, bool hasKey, const String& defName, void* overrideObject) {
					assert(false);
					return request;
				}

				void* object;
			};

			template <class T>
			class Typed : public TypedBase {
			public:
				Typed(const String& s, T* o = nullptr) : object(o) { name = s; }
				virtual ~Typed() {}

				virtual MetaNodeBase* Copy() const {
					return new Typed(name, object);
				}
				virtual Request& Read(Request& request, bool hasKey, const String& defName, void* overrideObject) {
					if (hasKey) {
						request >> Request::Key(name.empty() ? defName : name);
					}

					request >> (overrideObject == nullptr ? *object : *reinterpret_cast<T*>(overrideObject));
					return request;
				}

				virtual Request& Write(Request& request, bool hasKey, const String& defName, void* overrideObject) {
					if (hasKey) {
						request << Request::Key(name.empty() ? defName : name);
					}
					
					request << (overrideObject == nullptr ? *object : *reinterpret_cast<T*>(overrideObject));
					return request;
				}

				T* object;
			};
#if defined(_MSC_VER) && _MSC_VER <= 1200
			template <class T, class D>
			VC6Type operator () (T* pointer, D* member) const {
				Typed<D> typed(name, member);
				return *(reinterpret_cast<VC6Type*>(&typed));
			}
#else
			template <class T, class D>
			inline Typed<D> operator () (T* pointer, D* member) const {
				return Typed<D>(name, member);
			}
#endif

			template <class T, class D>
			struct RealType {
				typedef Typed<D> Type;
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			String name;
		};

		class MetaVariableIterator : public MetaVariable {
		public:
			MetaVariableIterator(const String& defName = "") : MetaVariable(defName) {}
			MetaVariableIterator operator = (const String& key);

#if defined(_MSC_VER) && _MSC_VER <= 1200
			template <class T, class D>
			VC6Type operator () (T* pointer, D* member) const {
				Typed<D::value_type> typed(name, nullptr);
				return *(reinterpret_cast<VC6Type*>(&typed));
			}
#else
			template <class T, class D>
			inline Typed<typename D::value_type> operator () (T* pointer, D* member) const {
				return Typed<typename D::value_type>(name, nullptr);
			}
#endif

			template <class T, class D>
			struct RealType {
#if defined(_MSC_VER) && _MSC_VER <= 1200
				typedef Typed<D::value_type> Type;
#else
				typedef Typed<typename D::value_type> Type;
#endif
			};
		};

		class MetaRemoteEntry : public MetaNodeBase {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			template <class T>
			static TWrapper<void> Convert(const T& t) {
				return *reinterpret_cast<const TWrapper<void>*>(&t);
			}

			MetaRemoteEntry operator = (const String& key) { name = key;  }

#if defined(_MSC_VER) && _MSC_VER <= 1200
			template <class A = Void, class B = Void, class C = Void, class D = Void, class E = Void, class F = Void, class G = Void, class H = Void, class I = Void, class J = Void, class K = Void, class L = Void, class M = Void>
			struct Dispatcher : public TWrapper<std::pair<Request::Ref, long>, Request&, bool> {
				void Invoke0(const Request::AutoWrapperBase& wrapper, Request& request) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second);
					(*this)(request, false);
				}

				void Invoke1(const Request::AutoWrapperBase& wrapper, Request& request, A a) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a);
					(*this)(request, false);
				}

				void Invoke2(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b);
					(*this)(request, false);
				}

				void Invoke3(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c);
					(*this)(request, false);
				}

				void Invoke4(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d);
					(*this)(request, false);
				}

				void Invoke5(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e);
					(*this)(request, false);
				}

				void Invoke6(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f);
					(*this)(request, false);
				}

				void Invoke7(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g);
					(*this)(request, false);
				}

				void Invoke8(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h);
					(*this)(request, false);
				}

				void Invoke9(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i);
					(*this)(request, false);
				}

				void Invoke10(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i, j);
					(*this)(request, false);
				}

				void Invoke11(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i, j, k);
					(*this)(request, false);
				}

				void Invoke12(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i, j, k, l);
					(*this)(request, false);
				}

				void Invoke13(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i, j, k, l);
					(*this)(request, false);
				}

				void Invoke14(const Request::AutoWrapperBase& wrapper, Request& request, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m) {
					std::pair<Request::Ref, long> ref = (*this)(request, true);
					request.Call(wrapper, ref.first, ref.second, a, b, c, d, e, f, g, h, i, j, k, l, m);
					(*this)(request, false);
				}
			};

			template <typename RR, typename FF, class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M>
			static TWrapper<void> Make(const TWrapper<RR, FF, A, B, C, D, E, F, G, H, I, J, K, L, M>& wp) {
				Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>* ptr = nullptr;
				switch (wp.GetCount()) {
				case 0:
					assert(false);
					break;
				case 1:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke0));
					break;
				case 2:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke1));
					break;
				case 3:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke2));
					break;
				case 4:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke3));
					break;
				case 5:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke4));
					break;
				case 6:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke5));
					break;
				case 7:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke6));
					break;
				case 8:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke7));
					break;
				case 9:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke8));
					break;
				case 10:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke9));
					break;
				case 11:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke10));
					break;
				case 12:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke11));
					break;
				case 13:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke12));
					break;
				case 14:
					return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke13));
					break;
				}

				assert(false);
				return Convert(Wrap(ptr, &Dispatcher<A, B, C, D, E, F, G, H, I, J, K, L, M>::Invoke0));
			}
#else
			template <typename... Args>
			struct Dispatcher : public TWrapper<std::pair<Request::Ref, long>, Request&, bool> {
				void Invoke(Request& request, const Request::AutoWrapperBase& wrapper, Args&&... args) {
					std::pair<IScript::Request::Ref, long> r = (*this)(request, true);
					request.Call(wrapper, r.first, r.second, std::forward<Args>(args)...);
					(*this)(request, false);
				}
			};

			template <typename R, typename... Args>
			static TWrapper<void> Make(const TWrapper<R, Request&, const Request::AutoWrapperBase&, Args...>&) {
				return Convert(Wrap((Dispatcher<Args...>*)nullptr, &Dispatcher<Args...>::Invoke));
			}
#endif

			MetaRemoteEntry(const String& n = "") : name(n), callIndex(0) {}
			MetaRemoteEntry(const TWrapper<void>& w) : wrapper(w) {}

			virtual MetaNodeBase* Copy() const {
				return new MetaRemoteEntry(wrapper);
			}

			template <class T, class D>
			MetaRemoteEntry operator () (T* pointer, D* member) const {
				return MetaRemoteEntry(Make(*member));
			}

			template <class T, class D>
			struct RealType {
				typedef MetaRemoteEntry Type;
			};

			typedef MetaRemoteEntry VC6Type;

			TWrapper<void> wrapper;
			String name;
			int callIndex;
		};

#define METHOD_ENTRY(name)\
		static const String _methodName = #name; \
		(void)&name;

#define METHOD_NAME() _methodName
#define U ,
#define CHECK_DELEGATE(d) \
			(MUST_CHECK_REFERENCE_ONCE); \
			if (!d) { \
				request.Error(_methodName + ": Invalid <" + d.GetTypeName() + "> " + #d); \
				return; \
			}
#define CHECK_REFERENCES_NONE() \
			const int MUST_CHECK_REFERENCE_ONCE = 0;

#define CHECK_REFERENCES(d) \
			const int MUST_CHECK_REFERENCE_ONCE = 0; \
			{ \
				IScript::Request::Ref refs[] = { d }; \
				for (size_t i = 0; i < sizeof(refs) / sizeof(refs[0]); i++) { \
					if (!refs[i]) { \
						char digit[32]; \
						sprintf(digit, "%d", (int)i); \
						request.Error(_methodName + ": Invalid references " #d "[" + digit + "]"); \
						request.DoLock(); \
						for (size_t j = 0; j < sizeof(refs) / sizeof(refs[0]); j++) { \
							if (refs[i]) { \
								request.Dereference(refs[i]); \
							} \
						} \
						request.UnLock(); \
						return; \
					} \
				} \
			}

#define CHECK_REFERENCES_WITH_TYPE(d, t) \
			const int MUST_CHECK_REFERENCE_ONCE = 0; \
			{ \
				IScript::Request::Ref refs[] = { d }; \
				IScript::Request::TYPE types[] = { t }; \
				static_assert(sizeof(refs) / sizeof(refs[0]) == sizeof(types) / sizeof(types[0]), "Unmatched type checking count"); \
				request.DoLock(); \
				for (size_t i = 0; i < sizeof(refs) / sizeof(refs[0]); i++) { \
					IScript::Request::TYPE p = request.GetReferenceType(refs[i]); \
					if (types[i] != IScript::Request::ANY && !request.GetScript()->IsTypeCompatible(types[i], p)) {\
						char digit[32]; \
						sprintf(digit, "%d", (int)i); \
						request.Error(_methodName + ": Invalid (or wrong type) references (" #d ") [" + digit + "], expect (" #t ")"); \
						for (size_t j = 0; j < sizeof(refs) / sizeof(refs[0]); j++) { \
							if (refs[i]) { \
								request.Dereference(refs[i]); \
							} \
						} \
						request.UnLock(); \
						return; \
					} \
				} \
				request.UnLock(); \
			}

		virtual void Reset();
		virtual const char* GetFileExt() const = 0;
		virtual IScript* NewScript() const = 0;
		virtual Request* NewRequest(const String& entry = "") = 0;
		virtual Request& GetDefaultRequest() = 0;
		virtual bool IsTypeCompatible(Request::TYPE target, Request::TYPE source) const;

		enum { DEBUG_LINE = 1, DEBUG_CALL = 2, DEBUG_TAILCALL = 4, DEBUG_RETURN = 8};
		virtual void SetDebugHandler(const TWrapper<void, Request&, int, int>& handler, int mask = DEBUG_LINE);
		virtual void SetErrorHandler(const TWrapper<void, Request&, String&>& errorHandler);
		virtual void SetDispatcher(const TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>& >& disp);
		virtual const char* QueryUniformResource(const String& path, size_t& length);
		const TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>& >& GetDispatcher() const;
		const TWrapper<void, Request&, int, int>& GetDebugHandler() const;
		// virtual void DoLock();
		// virtual void UnLock();

	protected:
		TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>& > dispatcher;
		TWrapper<void, Request&, String&> errorHandler;
		TWrapper<void, Request&, int, int> debugHandler;
		int debugMask;
		friend class Request;
	};

	extern IScript::MetaLibrary ScriptLibrary;
	extern IScript::MetaMethod ScriptMethod;
	extern IScript::MetaVariable ScriptVariable;
	extern IScript::MetaVariableIterator ScriptVariableIterator;
	extern IScript::MetaRemoteEntry ScriptWrapper;
	extern IScript::Request::TableStart begintable;
	extern IScript::Request::TableEnd endtable;
	extern IScript::Request::Key key;
	extern IScript::BaseDelegate delegate;
	extern IScript::Request::Nil nil;
	extern IScript::Request::Global global;
	extern IScript::Request::Local local;
	extern IScript::Request::Ref ref;
	extern IScript::Request::Skip skip;
	extern IScript::Request::Sync Sync;
	extern IScript::Request::Async Async;
}

#endif // __ISCRIPT_H__
