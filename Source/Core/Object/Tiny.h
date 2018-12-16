// Tiny.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-15
//

#ifndef __TINY_H__
#define __TINY_H__

#include "../Interface/IScript.h"
#include "../Interface/IReflect.h"
#include "../Template/TAtomic.h"

namespace PaintsNow {
	class Tiny : public IScript::Object {
	public:
		typedef uint32_t FLAG;
		enum {
			TINY_ACTIVATED = 1U << 0,
			TINY_DESTROYED = 1U << 1,
			TINY_MODIFIED = 1 << 2,
			TINY_UPDATING = 1 << 3,
			TINY_PINNED = 1U << 4,
			TINY_READONLY = 1U << 5,
			TINY_UNIQUE = 1U << 6,
			TINY_SHARED = 1U << 7,
			TINY_CUSTOM_BEGIN = 1U << 8,
			TINY_CUSTOM_END = 1U << 31,
			TINY_SIZE_BOUND = 1U << 31
		};

		Tiny(FLAG flag = 0);
		virtual size_t ReportMemoryUsage() const;
		TAtomic<FLAG>& Flag();
		FLAG Flag() const;

		virtual Tiny* Duplicate() const;
		virtual TObject<IReflect>& operator () (IReflect& reflect);

	protected:
		TAtomic<FLAG> flag;
	};

	class SharedTiny : public Tiny {
	public:
		SharedTiny(FLAG f = 0) : Tiny(f), referCount(1) {}

		// Make a reference when script object initialized.
		virtual void ScriptInitialize(IScript::Request& request) override {
			ReferenceObject();
		}

		virtual void ReleaseObject() override {
			if (--referCount == 0) {
				Tiny::ReleaseObject();
			}
		}

		virtual void ReferenceObject() {
			++referCount;
		}

		uint32_t GetReferCount() const {
			return referCount.load();
		}

	protected:
		TAtomic<uint32_t> referCount;
	};

	template <class T>
	class TShared {
	public:
		typedef T Type;
		TShared(T* t = nullptr) : tiny(t) {
			if (tiny != nullptr)
				tiny->ReferenceObject();
		}

#if defined(_MSC_VER) && _MSC_VER <= 1200
		static inline TShared Make() {
			TShared s;
			s.tiny = new T();
			return s;
		}

		template <class A>
		static inline TShared Make(A& a) {
			TShared s;
			s.tiny = new T(a);
			return s;
		}

		template <class A, class B>
		static inline TShared Make(A& a, B& b) {
			TShared s;
			s.tiny = new T(a, b);
			return s;
		}

		template <class A, class B, class C>
		static inline TShared Make(A& a, B& b, C& c) {
			TShared s;
			s.tiny = new T(a, b, c);
			return s;
		}

		template <class A, class B, class C, class D>
		static inline TShared Make(A& a, B& b, C& c, D& d) {
			TShared s;
			s.tiny = new T(a, b, c, d);
			return s;
		}

		template <class A, class B, class C, class D, class E>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e) {
			TShared s;
			s.tiny = new T(a, b, c, d, e);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i, J& j) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i, j);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i, J& j, K& k) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i, j, k);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i, J& j, K& k, L& l) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i, j, k, l);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i, J& j, K& k, L& l, M& m) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i, j, k, l, m);
			return s;
		}

		template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N>
		static inline TShared Make(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h, I& i, J& j, K& k, L& l, M& m, N& n) {
			TShared s;
			s.tiny = new T(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
			return s;
		}
#else
		template <typename... Args>
		static TShared Make(Args&&... args) {
			TShared s;
			s.tiny = new T(std::forward<Args>(args)...);
			return s;
		}
#endif

		~TShared() {
			Release();
		}

		TShared(const TShared& s) {
			tiny = s.tiny;
			if (tiny != nullptr)
				tiny->ReferenceObject();
		}

		template <class D>
		operator TShared<D>() {
			return TShared<D>(tiny);
		}

		TShared& operator = (const TShared& s) {
			if (s.tiny == tiny) return *this;

			Release();
			tiny = s.tiny;
			tiny->ReferenceObject();

			return *this;
		}

		bool operator == (const TShared& m) const {
			return tiny == m.tiny;
		}

		bool operator < (const TShared& m) const {
			return tiny < m.tiny;
		}

		static TShared Move(T* t) {
			TShared s;
			s.tiny = t;
			return s;
		}

		template <class D>
		TShared& Reset(D* t) {
			if (t == tiny) return *this;
			Release();
			tiny = t;

			return *this;
		}

#if !defined(_MSC_VER) || _MSC_VER > 1200
		TShared(TShared&& s) {
			tiny = s.tiny;
			s.tiny = nullptr;
		}

		TShared& operator = (TShared&& s) {
			std::swap(tiny, s.tiny);
			return *this;
		}
#endif

		void Release() {
			if (tiny != nullptr) {
				tiny->ReleaseObject();
				tiny = nullptr;
			}
		}

		T* operator -> () const {
			assert(tiny);
			return tiny;
		}

		T* operator ()() const {
			return tiny;
		}

		operator bool() const {
			return tiny != nullptr;
		}

	private:
		T* tiny;
	};
}


#endif // __TINY_H__