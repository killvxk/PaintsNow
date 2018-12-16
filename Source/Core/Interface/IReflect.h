// IReflect.h
// By PaintDream (paintdream@paintdream.com)
// 2014-7-16
//

#ifndef __IREFLECT_H__
#define __IREFLECT_H__

#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "../Template/TObject.h"
#include "../Template/TProxy.h"
#include "../Template/TFactory.h"
#include "IThread.h"
#include "IDevice.h"
#include <vector>
#include <cstring>
#include <map>
#include <cassert>
#ifndef _MSC_VER
#include <typeinfo>
#endif

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif


#ifdef _MSC_VER
#pragma warning(disable:4172)
#endif

namespace PaintsNow {
	class IStreamBase;
	class IReflectObject;
	class IUniqueAllocator;

	struct IUniqueInfo {
		virtual ~IUniqueInfo() {}
		virtual size_t GetSize() const;
		virtual IReflectObject* Create() const;
		virtual IUniqueAllocator* GetAllocator() const;
		void Endianize(void* buffer) {
			if (IsStdEndian() && IsInteger()) {
				char* p = reinterpret_cast<char*>(buffer);
				for (size_t i = 0; i < size / 2; i++) {
					std::swap(p[i], p[size - i - 1]);
				}
			}
		}
		typedef void* (*Cast)(IReflectObject* from);
		bool IsInteger();
		String GetSubName() const;
		enum INT_STATE { IS_INTEGER, NOT_INTEGER, TBD };

		IUniqueAllocator* allocator;
		size_t size;
		String typeName;
		TWrapper<IReflectObject*> creator;
		std::map<IUniqueInfo*, Cast> interfaces;

		INT_STATE intState;
	};

	struct Unique {
		Unique(IUniqueInfo* f = nullptr);
		bool operator == (const Unique& unique) const;
		bool operator != (const Unique& unique) const;
		bool operator < (const Unique& unique) const;
		const IUniqueInfo* operator -> () const;
		IUniqueInfo* operator -> ();

		IUniqueInfo* info;
	};

	class IUniqueAllocator {
	public:
		virtual ~IUniqueAllocator();
		virtual IUniqueInfo* Alloc(const String& name, size_t size) = 0;
		virtual IUniqueInfo* Get(const String& name) = 0;
	};

	class UniqueAllocator : public IUniqueAllocator {
	public:
		UniqueAllocator(IThread& thread);
		virtual ~UniqueAllocator();
		virtual IUniqueInfo* Alloc(const String& name, size_t size);
		virtual IUniqueInfo* Get(const String& name);
		virtual IThread& GetThreadApi();

	private:
		IThread& thread;
		IThread::Lock* lock;
		std::map<String, IUniqueInfo> mapType;
	};

	IUniqueAllocator* GetGlobalUniqueAllocator();
	void SetGlobalUniqueAllocator(IUniqueAllocator* alloc);

	template <class T>
	struct UniqueTypeEx {
		static std::string Demangle(const char* name) {
		#ifdef __GNUG__
			int status = -4; // some arbitrary value to eliminate the compiler warning
			// enable c++11 by passing the flag -std=c++11 to g++
			std::unique_ptr<char, void(*)(void*)> res {
				abi::__cxa_demangle(name, nullptr, nullptr, &status),
					std::free
			};

			return (status==0) ? res.get() : name;
		#else
			return name;
		#endif
		}

		static void RemovePatterns(String& s, const String& p) {
			size_t size = p.size();
			for (size_t i = s.find(p); i != String::npos; i = s.find(p))
				s.erase(i, size);
		}

		static Unique Get(IUniqueAllocator* allocator = GetGlobalUniqueAllocator()) {
			assert(allocator != nullptr);
			String className = Demangle(typeid(T).name());
			// remove 'class ' or 'struct ' prefix
			const String skipPrefixes[] = { "class ", "struct " };
			for (size_t i = 0; i < sizeof(skipPrefixes) / sizeof(skipPrefixes[0]); i++) {
				RemovePatterns(className, skipPrefixes[i]);
			}
			static IUniqueInfo* value = allocator->Alloc(className, sizeof(typename ReturnType<T>::type));
			return value;
		}
	};

	template <class T>
	struct UniqueType : public String {
		UniqueType(const String& filter = "") : String(filter) {}
		static Unique Get(IUniqueAllocator* allocator = GetGlobalUniqueAllocator()) {
#if defined(_MSC_VER) && _MSC_VER <= 1200
			return UniqueTypeEx<std::remove_reference<T>::type>::Get(allocator);
#else
			return UniqueTypeEx<typename std::remove_reference<T>::type>::Get(allocator);
#endif
		}
	};


	class IReflect;
	class IIterator;
	class IReflectObject : public TObject<IReflect> {
	public:
		IReflectObject();
		// general object construction
		template <class T>
		IReflectObject(const T& t) {}

		virtual ~IReflectObject();
		virtual TObject<IReflect>& operator () (IReflect& reflect);
		virtual const TObject<IReflect>& operator () (IReflect& reflect) const;
		virtual bool IsBaseObject() const; // returns true, any sub-class should return false;
		virtual bool IsIterator() const; // only for IIterators
		virtual Unique GetUnique() const;

		template <class T>
		static T* QueryInterfaceEx(IReflectObject* object, Unique unique, UniqueType<T> target) {
			if (unique == target.Get()) return static_cast<T*>(object);

			std::map<IUniqueInfo*, IUniqueInfo::Cast>::const_iterator p = unique.info->interfaces.find(target.Get().info);
			if (p != unique.info->interfaces.end()) {
				return reinterpret_cast<T*>(p->second(object));
			} else {
				T* result = nullptr;
				for (std::map<IUniqueInfo*, IUniqueInfo::Cast>::const_iterator t = unique.info->interfaces.begin(); result == nullptr && t != unique.info->interfaces.end(); ++t) {
					result = QueryInterfaceEx(object, (*t).first, target);
				}

				return result;
			}
		}

		template <class T>
		T* QueryInterface(UniqueType<T> t) {
			return QueryInterfaceEx(this, GetUnique(), t);
		}

		template <class T>
		const T* QueryInterface(UniqueType<T> t) const {
			return QueryInterfaceEx(const_cast<IReflectObject*>(this), GetUnique(), t);
		}

		virtual IUniqueAllocator* GetUniqueAllocator() const;
		virtual void ReleaseObject();

		// These two functions are work-arounds for gcc compiler.
		static const IReflectObject& TransformReflectObject(IReflectObject& t) {
			return t;
		}

		static const IReflectObject& TransformReflectObject(const IReflectObject& t) {
			static const IReflectObject s;
			return t.IsBaseObject() ? s : t;
		}
		
		// Not recommended for frequently calls
		template <class T>
		T* Inspect(UniqueType<T> unique, const String& key = "") {
			static Unique u = UniqueType<T>::Get();
			return reinterpret_cast<T*>(InspectEx(u, key));
		}

		virtual bool operator >> (IStreamBase& stream) const;
		virtual bool operator << (IStreamBase& stream);

	protected:
		template <class T>
		static Unique GetUniqueByValue(const T& t, IUniqueAllocator* allocator = GetGlobalUniqueAllocator()) {
			return UniqueType<T>::Get(allocator);
		}

	private:
		virtual void* InspectEx(Unique unique, const String& key);
	};

	class IReflectObjectComplex : public IReflectObject {
	public:
		virtual bool IsBaseObject() const;
		virtual TObject<IReflect>& operator () (IReflect& reflect);
	};

	class IIterator : public IReflectObject {
	public:
		IIterator();
		virtual ~IIterator();
		virtual bool IsBaseObject() const;
		virtual bool IsIterator() const;
		virtual IIterator* New() const = 0;
		virtual void Attach(void* base) = 0;
		virtual void* GetHost() const = 0;
		virtual void Initialize(size_t count) = 0;
		virtual size_t GetTotalCount() const = 0;
		virtual void* Get() = 0;
		virtual const IReflectObject& GetPrototype() const = 0;
		virtual Unique GetPrototypeUnique() const = 0;
		virtual bool IsLayoutLinear() const = 0;
		virtual bool IsLayoutPinned() const = 0;
		virtual bool Next() = 0;
	};

	// for static arrays
	template <class T, class S>
	class ArrayIterator : public IIterator {
	public:
		typedef S value_type;
		ArrayIterator() : pointer(nullptr) {}
		ArrayIterator(T& p) : pointer(p), count(sizeof(p) / sizeof(p[0])), i(0) {}
		virtual void Initialize(size_t c) {
			// cannot create a larger count
			static T p;
			count = Min(c, sizeof(p) / sizeof(p[0]));
		}

		template <class D>
		Unique GetPrototypeUniqueEx(D* d) const {
			return UniqueType<D>::Get();
		}

		virtual Unique GetPrototypeUnique() const {
			static T p;
			return GetPrototypeUniqueEx(p);
		}

		virtual size_t GetTotalCount() const {
			return count;
		}

		virtual void* Get() {
			assert(i != 0);
			return &(*reinterpret_cast<T*>(pointer))[i - 1];
		}

		virtual const IReflectObject& GetPrototype() const {
			static T t;
			static const IReflectObject& object = IReflectObject::TransformReflectObject(t[0]);
			return object;
		}

		virtual bool Next() {
			if (i >= count) {
				return false;
			}

			i++;
			return true;
		}

		virtual IIterator* New() const {
			ArrayIterator* iter = new ArrayIterator();
			iter->pointer = pointer;
			iter->count = count;
			iter->i = 0;
			return iter;
		}

		virtual void Attach(void* p) {
			pointer = p;
			i = 0;
		}

		virtual bool IsLayoutLinear() const {
			return true;
		}

		virtual bool IsLayoutPinned() const {
			return false;
		}

		virtual void* GetHost() const {
			return pointer;
		}

	private:
		void* pointer;
		size_t count;
		size_t i;
	};

	template <class T, class D>
	ArrayIterator<T, D> IterateArrayEx(T& t, D& d) {
		return ArrayIterator<T, D>(t);
	}

#define IterateArray(t) \
	IterateArrayEx(t, t[0])

	// for vector arrays
	template <class T>
	class VectorIterator : public IIterator {
	public:
#if defined(_MSC_VER) && _MSC_VER <= 1200
		typedef T::value_type value_type;
#else
		typedef typename T::value_type value_type;
#endif
		VectorIterator() : base(nullptr), i(0) {}
		VectorIterator(T& p) : base(&p), i(0) {}
		virtual void Initialize(size_t c) {
			assert(base != nullptr);
			base->resize(c);
		}

		virtual size_t GetTotalCount() const {
			assert(base != nullptr);
			return base->size();
		}

		virtual Unique GetPrototypeUnique() const {
#if defined(_MSC_VER) && _MSC_VER <= 1200
			typedef const T::value_type t;
#else
			typedef const typename T::value_type t;
#endif
			return UniqueType<t>::Get();
		}

		virtual const IReflectObject& GetPrototype() const {
#if defined(_MSC_VER) && _MSC_VER <= 1200
			static const T::value_type t;
#else
			static const typename T::value_type t = typename T::value_type();
#endif
			static const IReflectObject& object = IReflectObject::TransformReflectObject(t);
			return object;
		}

		virtual void* Get() {
			assert(i != 0);
			return &(*base)[i - 1];
		}

		virtual bool Next() {
			if (i >= (*base).size()) {
				return false;
			}

			i++;
			return true;
		}

		virtual IIterator* New() const {
			VectorIterator* iter = new VectorIterator(*base);
			return iter;
		}

		virtual void Attach(void* p) {
			base = reinterpret_cast<T*>(p);
			i = 0;
		}

		virtual bool IsLayoutLinear() const {
			return true;
		}

		virtual bool IsLayoutPinned() const {
			return false;
		}

		virtual void* GetHost() const {
			return base;
		}

	private:
		T* base;
		size_t i;
	};

	template <class T>
	VectorIterator<T> IterateVector(T& t) {
		return VectorIterator<T>(t);
	}

	class MetaNodeBase : public IReflectObjectComplex {
	public:
		virtual TObject<IReflect>& operator () (IReflect& reflect) {
			// Do nothing ..
			return *this;
		}
	};

	class MetaChainBase : public IReflectObjectComplex {
	public:
		virtual const MetaChainBase* GetNext() const {
			return nullptr;
		}

		virtual const MetaNodeBase* GetNode() const {
			return nullptr;
		}

	private:
		virtual TObject<IReflect>& operator () (IReflect& reflect) {
			assert(false);
			return *this;
		}
	};

	template <class T, class D, class K, class Base>
	class MetaChain : public MetaChainBase {
	public:
		MetaChain(const K& k, const Base& b) : chainNode(k), base(b) {}

#if defined(_MSC_VER) && _MSC_VER <= 1200
		template <class N>
		inline MetaChain<T, D, N::VC6Type, MetaChain<T, D, K, Base> > operator [] (const N& t) const {
			return MetaChain<T, D, N::VC6Type, MetaChain<T, D, K, Base> >(t(GetObject(), GetMember()), *this);
		}
#else
		template <class N>
		inline MetaChain<T, D, typename N::template RealType<T, D>::Type, MetaChain<T, D, K, Base> > operator [] (const N& t) const {
			return MetaChain<T, D, typename N::template RealType<T, D>::Type, MetaChain<T, D, K, Base> >(t(GetObject(), GetMember()), *this);
		}
#endif

		inline bool IsChain() const {
			return true;
		}

		virtual const MetaChainBase* GetNext() const {
			return base.IsChain() ? &base : nullptr;
		}

		virtual const MetaNodeBase* GetNode() const {
			return &chainNode;
		}

		inline void operator * () const {
			Finish(this);
		}

		void Finish(const MetaChainBase* head) const {
			(const_cast<Base&>(base)).Finish(head);
		}

		inline T* GetObject() const {
			return base.GetObject();
		}

		inline D* GetMember() const {
			return base.GetMember();
		}

		K chainNode;
		const Base& base;
	};

	class IReflect {
	public:
		IReflect(bool a, bool m);
		virtual ~IReflect();

		bool IsReflectProperty() const;
		bool IsReflectMethod() const;

		void RegisterBuiltinTypes(bool useStdintType = false);
		struct Param {
			Param(Unique t = UniqueType<void>::Get(), const String& n = "") : type(t), name(n) {}
			operator Unique () const {
				return type;
			}

			Unique type;
			String name;
		};

		// static size_t GetUniqueLength(Unique id);

		template <class T>
		inline void OnClass(T& t, const char* name, const char* path, const MetaChainBase* meta) {
			Unique u = UniqueType<T>::Get();
			Class(t, u, name, path, meta);
		}

		template <class T>
		inline void OnProperty(const T& t, const char* name, void* base, const MetaChainBase* meta) {
			Unique u = UniqueType<T>::Get();
			ForwardProcessProperty(const_cast<T&>(t), u, name, base, (void*)&t, meta);
		}

		inline void ForwardProcessProperty(const IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
			ProcessProperty(const_cast<IReflectObject&>(s), typeID, name, base, ptr, meta);
		}

#if (defined(_MSC_VER) && _MSC_VER < 1800) || defined(COMPATIBLE_PROXY)
		// T == TWrapper<...>
		template <class R, class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P>
		inline void OnMethod(const TWrapper<R, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P>& t, const char* name, const MetaChainBase* meta) {
			static Unique params[] = {
				UniqueType<A>::Get(),
				UniqueType<B>::Get(),
				UniqueType<C>::Get(),
				UniqueType<D>::Get(),
				UniqueType<E>::Get(),
				UniqueType<F>::Get(),
				UniqueType<G>::Get(),
				UniqueType<H>::Get(),
				UniqueType<I>::Get(),
				UniqueType<J>::Get(),
				UniqueType<K>::Get(),
				UniqueType<L>::Get(),
				UniqueType<M>::Get(),
				UniqueType<N>::Get(),
				UniqueType<O>::Get(),
				UniqueType<P>::Get()
			};

			static Param retValue = UniqueType<R>::Get();
			std::vector<Param> p;
			for (size_t i = 0; i < sizeof(params) / sizeof(params[0]) && (!(params[i] == UniqueType<Void>::Get())); i++) {
				p.push_back(Param(params[i]));
			}

			static Unique unique = UniqueType<TWrapper<R, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P> >::Get();
			ProcessMethod(unique, name, reinterpret_cast<const TProxy<>*>(&t.GetProxy()), retValue, p, meta);
		}
#else
		template <typename R, typename... Args>
		inline void OnMethod(const TWrapper<R, Args...>& t, const char* name, const MetaChainBase* meta) {
			std::vector<Param> params;
			ParseParams(params, t);
			static Unique u = UniqueType<R>::Get();
			static Param retValue(u);
			static Unique unique = UniqueType<TWrapper<R, Args...> >::Get();
			ProcessMethod(unique, name, reinterpret_cast<const TProxy<>*>(&t.GetProxy()), retValue, params, meta);
		}

		template <typename R, typename V, typename... Args>
		inline void ParseParams(std::vector<Param>& params, const TWrapper<R, V, Args...>&) {
			static Unique u = UniqueType<V>::Get();
			params.push_back(Param(u));
			ParseParams(params, TWrapper<R, Args...>());
		}

		template <typename R>
		inline void ParseParams(std::vector<Param>& params, const TWrapper<R>&) {}
#endif

		virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) = 0;
		virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) = 0;
		virtual void Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta);

	private:
		bool isReflectProperty;
		bool isReflectMethod;
	};

	class MetaNote : public MetaNodeBase {
	public:
		MetaNote(const String& v);
		MetaNote operator = (const String& value);

		template <class T, class D>
		inline const MetaNote& operator () (T* t, D* d) const {
			return *this; // do nothing
		}

		template <class T, class D>
		struct RealType {
			typedef MetaNote Type;
		};

		typedef MetaNote VC6Type;

		virtual TObject<IReflect>& operator () (IReflect& reflect);
		const String& value;
	};

	extern MetaNote Note;

	template <class T, class D>
	class WriterBase : public MetaChainBase {
	public:
		WriterBase(IReflect& r, T* p, D* v, const char* n) : reflect(r), object(p), member(v), name(n) {}
		virtual ~WriterBase() {}

		inline bool IsChain() const { return false; }

		virtual void Finish(const MetaChainBase* head) {}

#if defined(_MSC_VER) && _MSC_VER <= 1200
		template <class N>
		inline MetaChain<T, D, N::VC6Type, WriterBase<T, D> > operator [] (const N& t) const {
			return MetaChain<T, D, N::VC6Type, WriterBase<T, D> >(t(object, member), *this);
		}
#else
		template <class N>
		inline MetaChain<T, D, typename N::template RealType<T, D>::Type, WriterBase<T, D> > operator [] (const N& t) const {
			return MetaChain<T, D, typename N::template RealType<T, D>::Type, WriterBase<T, D> >(t(object, member), *this);
		}
#endif

		inline T* GetObject() const {
			return object;
		}

		inline D* GetMember() const {
			return member;
		}

		IReflect& reflect;
		T* object;
		D* member;
		const char* name;
	};

	template <class T, class D>
	class AttribWriter : public WriterBase<T, D> {
	public:
		AttribWriter(IReflect& r, T* p, D* v, const char* n) : WriterBase<T, D>(r, p, v, n) {}
		~AttribWriter() {}

		void operator * () {
			Finish(nullptr);
		}

		virtual void Finish(const MetaChainBase* head) {
			WriterBase<T, D>::reflect.OnProperty(*WriterBase<T, D>::member, WriterBase<T, D>::name, WriterBase<T, D>::object, head == this ? nullptr : head);
		}
	};

	template <class T, class D>
	AttribWriter<T, D> CreateAttribWriter(IReflect& r, T* p, const D& v, const char* name) {
		return AttribWriter<T, D>(r, p, const_cast<D*>(&v), name);
	}

	template <class T, class D>
	class MethodWriter : public WriterBase<T, D> {
	public:
		MethodWriter(IReflect& r, T* p, D* v, const char* n) : WriterBase<T, D>(r, p, v, n) {}
		virtual ~MethodWriter() {}

		void operator * () {
			Finish(nullptr);
		}

		virtual void Finish(const MetaChainBase* head) {
			WriterBase<T, D>::reflect.OnMethod(Wrap(WriterBase<T, D>::object, *WriterBase<T, D>::member), WriterBase<T, D>::name, head == this ? nullptr : head);
		}
	};

	template <class T, class D>
	MethodWriter<T, D> CreateMethodWriter(IReflect& r, T* p, const D& v, const char* name) {
		return MethodWriter<T, D>(r, p, const_cast<D*>(&v), name);
	}

	template <class T>
	class ClassWriter : public WriterBase<T, const char>  {
	public:
		ClassWriter(IReflect& r, T* p, const char* n, const char* l) : WriterBase<T, const char>(r, p, l, n) {}
		virtual ~ClassWriter() {}

		void operator * () {
			Finish(nullptr);
		}

		virtual void Finish(const MetaChainBase* head) {
			WriterBase<T, const char>::reflect.OnClass(*WriterBase<T, const char>::object, WriterBase<T, const char>::name, WriterBase<T, const char>::member, head);
		}
	};

	template <class T>
	ClassWriter<T> CreateClassWriter(IReflect& r, T* v, const char* name, const char* line) {
		return ClassWriter<T>(r, v, name, line);
	}


	template <class T>
	struct Creatable {
		Creatable() {
			static Unique u = UniqueType<T>::Get();
			u->creator = &Creatable::Create;
		}
		static void Init() {
			static Creatable<T> theInstance;
		}
		static IReflectObject* Create() {
			return new T();
		}
	};

	class MetaConstructable : public MetaNodeBase {
	public:
		template <class T, class D>
		inline const MetaConstructable& operator () (T* t, D* d) const {
			static Creatable<T> theClass;
			return *this; // do nothing
		}

		template <class T, class D>
		struct RealType {
			typedef MetaConstructable Type;
		};

		typedef MetaConstructable VC6Type;
	};

	extern MetaConstructable Constructable;

	template <class T, class P>
	class RegisterInterface {
	public:
		static void* Convert(IReflectObject* v) {
			return static_cast<P*>(static_cast<T*>(v));
		}
		RegisterInterface() {
			static Unique u = UniqueType<T>::Get();
			static Unique p = UniqueType<P>::Get();
			u->interfaces[p.info] = &RegisterInterface<T, P>::Convert;
		}
	};

	template <class P>
	class MetaInterface : public MetaNodeBase {
	public:
		P& object;
		class DummyReflect : public IReflect {
		public:
			DummyReflect() : IReflect(false, false) {}
			virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {};
			virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {};
		};

		MetaInterface(P& obj) : object(obj) {
			static DummyReflect dummyReflect;
			object.P::operator () (dummyReflect);
		}

		template <class T, class D>
		inline const MetaInterface<P>& operator () (T* t, D* d) const {
			// class T is Interfaced form class P
			static RegisterInterface<T, P> reg;
			
			return *this; // do nothing
		}

		template <class T, class D>
		struct RealType {
			typedef MetaInterface<P> Type;
		};

		typedef MetaInterface<P> VC6Type;

		virtual TObject<IReflect>& operator () (IReflect& reflect) {
			return object.P::operator () (reflect);
		}
	};

	template <class T>
	class BuiltinTypeWrapper : public IReflectObject {};

#define ReflectBuiltinSubtype(v) \
	{ static BuiltinTypeWrapper<v> proto; reflect.Class(proto, UniqueType<v>::Get(), #v, "C++", nullptr); }

#define ReflectBuiltinType(v) \
	ReflectBuiltinSubtype(v); \
	ReflectBuiltinSubtype(const v); \
	ReflectBuiltinSubtype(v&); \
	ReflectBuiltinSubtype(const v&); \
	ReflectBuiltinSubtype(v*); \
	ReflectBuiltinSubtype(const v*); \
	ReflectBuiltinSubtype(v*&); \
	ReflectBuiltinSubtype(const v*&); \

#define Interface(c) \
	MetaInterface<c>(*this)

#define ReflectClass(v) \
	typedef v CurrentClass; \
	*CreateClassWriter(reflect, this, #v, __FILE__)

#define ReflectProperty(v) \
	*CreateAttribWriter(reflect, this, v, #v)

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define ReflectMethod(v) \
	*CreateMethodWriter(reflect, this, &CurrentClass::v, #v)
#else
#define ReflectMethod(v) \
	*CreateMethodWriter(reflect, this, &std::remove_reference<decltype(*this)>::type::v, #v)
#endif

	class ISyncObject : public IDevice {
	public:
		ISyncObject(IThread& threadApi);
		virtual ~ISyncObject();
		virtual void DoLock();
		virtual void UnLock();
		virtual bool TryLock();
		size_t GetLockCount() const;
		IThread& GetThreadApi();

	protected:
		IThread& threadApi;
		IThread::Lock* lock;
	};

	class Inspector : public IReflect {
	public:
		Inspector(const IReflectObject& r);

		template <class T>
		T* operator [](UniqueType<T> ut) {
			static Unique u = UniqueType<T>::Get();
			return reinterpret_cast<T*>(Find(u, ut));
		}

		virtual void* Find(Unique unique, const String& key) const;
		virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
		virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);

	private:
		std::map<Unique, std::map<String, void*> > entries;
	};

	class MetaRuntime : public MetaNodeBase {
	public:
		template <class T, class D>
		inline const MetaRuntime& operator () (T* t, D* d) const {
			return *this; // do nothing
		}

		template <class T, class D>
		struct RealType {
			typedef MetaRuntime Type;
		};

		typedef MetaRuntime VC6Type;
		virtual TObject<IReflect>& operator () (IReflect& reflect);
	};

	extern MetaRuntime Runtime;
}

#endif // __IREFLECT_H__
