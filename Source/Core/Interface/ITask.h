// ITask.h
// By PaintDream (paintdream@paintdream.com)
// 2016-4-6
//

#ifndef __ITASK_H__
#define __ITASK_H__

#include "../Template/TAlgorithm.h"

namespace PaintsNow {
	class ITask {
	public:
		virtual ~ITask();

		virtual void Execute(void* context) = 0;
		virtual void Suspend(void* context) = 0;
		virtual void Resume(void* context) = 0;
		virtual void Abort(void* context) = 0;
		virtual bool Continue() const = 0;
	};

	class QuickTask : public ITask {
	public:
		virtual void Execute(void* context) = 0;
		virtual void Suspend(void* context);
		virtual void Resume(void* context);
		virtual void Abort(void* context);
		virtual bool Continue() const;
	};

#if defined(_MSC_VER) && _MSC_VER <= 1200
	template <class T>
	class ClosureTaskTemplate : public QuickTask {
	public:
		ClosureTaskTemplate(T ref) : callback(ref) {}
		virtual void Execute(void* request) override {
			callback(request);
			delete this;
		}

		T callback;
	};

	template <class T>
	ITask* WrapTask(T ref) {
		return new ClosureTaskTemplate<T>(ref);
	}

	template <class T, class A>
	class ClosureTaskTemplateA : public QuickTask {
	public:
		ClosureTaskTemplateA(T ref, A a) : callback(ref), pa(a) {}
		virtual void Execute(void* request) override {
			callback(request, pa);
			delete this;
		}

		T callback;
		A pa;
	};

	template <class T, class A>
	ITask* WrapTask(T ref, A a) {
		return new ClosureTaskTemplateA<T, A>(ref, a);
	}

	template <class T, class A, class B>
	class ClosureTaskTemplateB : public QuickTask {
	public:
		ClosureTaskTemplateB(T ref, A a, B b) : callback(ref), pa(a), pb(b) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb);
			delete this;
		}

		T callback;
		A pa;
		B pb;
	};

	template <class T, class A, class B>
	ITask* WrapTask(T ref, A a, B b) {
		return new ClosureTaskTemplateB<T, A, B>(ref, a, b);
	}

	template <class T, class A, class B, class C>
	class ClosureTaskTemplateC : public QuickTask {
	public:
		ClosureTaskTemplateC(T ref, A a, B b, C c) : callback(ref), pa(a), pb(b), pc(c) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb, pc);
			delete this;
		}

		T callback;
		A pa;
		B pb;
		C pc;
	};

	template <class T, class A, class B, class C>
	ITask* WrapTask(T ref, A a, B b, C c) {
		return new ClosureTaskTemplateC<T, A, B, C>(ref, a, b, c);
	}

	template <class T, class A, class B, class C, class D>
	class ClosureTaskTemplateD : public QuickTask {
	public:
		ClosureTaskTemplateD(T ref, A a, B b, C c, D d) : callback(ref), pa(a), pb(b), pc(c), pd(d) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb, pc, pd);
			delete this;
		}

		T callback;
		A pa;
		B pb;
		C pc;
		D pd;
	};

	template <class T, class A, class B, class C, class D>
	ITask* WrapTask(T ref, A a, B b, C c, D d) {
		return new ClosureTaskTemplateD<T, A, B, C, D>(ref, a, b, c, d);
	}

	template <class T, class A, class B, class C, class D, class E>
	class ClosureTaskTemplateE : public QuickTask {
	public:
		ClosureTaskTemplateE(T ref, A a, B b, C c, D d, E e) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb, pc, pd, pe);
			delete this;
		}

		T callback;
		A pa;
		B pb;
		C pc;
		D pd;
		E pe;
	};

	template <class T, class A, class B, class C, class D, class E>
	ITask* WrapTask(T ref, A a, B b, C c, D d, E e) {
		return new ClosureTaskTemplateE<T, A, B, C, D, E>(ref, a, b, c, d, e);
	}

	template <class T, class A, class B, class C, class D, class E, class F>
	class ClosureTaskTemplateF : public QuickTask {
	public:
		ClosureTaskTemplateF(T ref, A a, B b, C c, D d, E e, F f) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb, pc, pd, pe, pf);
			delete this;
		}

		T callback;
		A pa;
		B pb;
		C pc;
		D pd;
		E pe;
		F pf;
	};

	template <class T, class A, class B, class C, class D, class E, class F>
	ITask* WrapTask(T ref, A a, B b, C c, D d, E e, F f) {
		return new ClosureTaskTemplateF<T, A, B, C, D, E, F>(ref, a, b, c, d, e, f);
	}

	template <class T, class A, class B, class C, class D, class E, class F, class G>
	class ClosureTaskTemplateG : public QuickTask {
	public:
		ClosureTaskTemplateG(T ref, A a, B b, C c, D d, E e, F f, G g) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f), pg(g) {}
		virtual void Execute(void* request) override {
			callback(request, pa, pb, pc, pd, pe, pf, pg);
			delete this;
		}

		T callback;
		A pa;
		B pb;
		C pc;
		D pd;
		E pe;
		F pf;
		G pg;
	};

	template <class T, class A, class B, class C, class D, class E, class F, class G>
	ITask* WrapTask(T ref, A a, B b, C c, D d, E e, F f, G g) {
		return new ClosureTaskTemplateG<T, A, B, C, D, E, F, G>(ref, a, b, c, d, e, f, g);
	}
#else

	template <typename T, typename... Args>
	class ClosureTaskTemplate : public QuickTask {
	public:
		ClosureTaskTemplate(T c, Args&&... args) : callback(c), arguments(std::forward<Args>(args)...) {}

		template <size_t... S>
		void Apply(void* context, seq<S...>) {
			callback(context, std::move(std::get<S>(std::move(arguments)))...);
		}

		virtual void Execute(void* request) override {
			Apply(request, gen_seq<sizeof...(Args)>());
			delete this;
		}

		T callback;
		std::tuple<Args...> arguments;
	};

	template <typename T, typename... Args>
	ITask* WrapTask(T closure, Args&&... args) {
		return new ClosureTaskTemplate<T, Args...>(closure, std::forward<Args>(args)...);
	}
#endif
}


#endif // __ITASK_H__