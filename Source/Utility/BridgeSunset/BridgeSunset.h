// BridgeSunset.h -- Asynchronous routine dispatching module for script
// By PaintDream (paintdream@paintdream.com)
// 2015-1-2
//


#ifndef __BRIDGESUNSET_H__
#define __BRIDGESUNSET_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/ITimer.h"
#include "../../Core/Common/ZThreadPool.h"
#include "../../Core/Template/TFactory.h"
#include <stack>

namespace PaintsNow {
	namespace NsBridgeSunset {
		class BridgeSunset : public IScript::Library {
		public:
			BridgeSunset(IThread& threadApi, IScript& script, size_t threadCount);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual ~BridgeSunset();
			virtual void Require(IScript::Request& request);
			virtual void ScriptUninitialize(IScript::Request& request);
			void Reset(bool reload);
			void Dispatch(ITask* task);

			ZThreadPool& GetThreadPool();
			IScript& GetScript();

		protected:
			void BindRequests();
			void UnbindRequests();
			IScript& script;
			ZThreadPool threadPool;
		};

#if defined(_MSC_VER) && _MSC_VER <= 1200
		class ScriptTaskTemplate : public QuickTask {
		public:
			ScriptTaskTemplate(IScript::Request::Ref ref) : callback(ref) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback);
				req.Pop();
				req.UnLock();

				delete this;
			}

			IScript::Request::Ref callback;
		};

		inline ITask* WrapTaskScript(IScript::Request::Ref ref) {
			return new ScriptTaskTemplate(ref);
		}
		
		template <class A>
		class ScriptTaskTemplateA : public QuickTask {
		public:
			ScriptTaskTemplateA(IScript::Request::Ref ref, A a) : callback(ref), pa(a) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa);
				req.Pop();
				req.UnLock();

				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
		};

		template <class A>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a) {
			return new ScriptTaskTemplateA<A>(ref, a);
		}

		template <class A, class B>
		class ScriptTaskTemplateB : public QuickTask {
		public:
			ScriptTaskTemplateB(IScript::Request::Ref ref, A a, B b) : callback(ref), pa(a), pb(b) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
		};

		template <class A, class B>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b) {
			return new ScriptTaskTemplateB<A, B>(ref, a, b);
		}

		template <class A, class B, class C>
		class ScriptTaskTemplateC : public QuickTask {
		public:
			ScriptTaskTemplateC(IScript::Request::Ref ref, A a, B b, C c) : callback(ref), pa(a), pb(b), pc(c) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb, pc);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
			C pc;
		};

		template <class A, class B, class C>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b, C c) {
			return new ScriptTaskTemplateC<A, B, C>(ref, a, b, c);
		}

		template <class A, class B, class C, class D>
		class ScriptTaskTemplateD : public QuickTask {
		public:
			ScriptTaskTemplateD(IScript::Request::Ref ref, A a, B b, C c, D d) : callback(ref), pa(a), pb(b), pc(c), pd(d) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb, pc, pd);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
			C pc;
			D pd;
		};

		template <class A, class B, class C, class D>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b, C c, D d) {
			return new ScriptTaskTemplateD<A, B, C, D>(ref, a, b, c, d);
		}

		template <class A, class B, class C, class D, class E>
		class ScriptTaskTemplateE : public QuickTask {
		public:
			ScriptTaskTemplateE(IScript::Request::Ref ref, A a, B b, C c, D d, E e) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb, pc, pd, pe);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
			C pc;
			D pd;
			E pe;
		};

		template <class A, class B, class C, class D, class E>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b, C c, D d, E e) {
			return new ScriptTaskTemplateE<A, B, C, D, E>(ref, a, b, c, d, e);
		}

		template <class A, class B, class C, class D, class E, class F>
		class ScriptTaskTemplateF : public QuickTask {
		public:
			ScriptTaskTemplateF(IScript::Request::Ref ref, A a, B b, C c, D d, E e, F f) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb, pc, pd, pe, pf);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
			C pc;
			D pd;
			E pe;
			F pf;
		};

		template <class A, class B, class C, class D, class E, class F>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b, C c, D d, E e, F f) {
			return new ScriptTaskTemplateF<A, B, C, D, E, F>(ref, a, b, c, d, e, f);
		}

		template <class A, class B, class C, class D, class E, class F, class G>
		class ScriptTaskTemplateG : public QuickTask {
		public:
			ScriptTaskTemplateG(IScript::Request::Ref ref, A a, B b, C c, D d, E e, F f, G g) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f), pg(g) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				req.Call(Sync, callback, pa, pb, pc, pd, pe, pf, pg);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			A pa;
			B pb;
			C pc;
			D pd;
			E pe;
			F pf;
			G pg;
		};

		template <class A, class B, class C, class D, class E, class F, class G>
		ITask* WrapTaskScript(IScript::Request::Ref ref, A a, B b, C c, D d, E e, F f, G g) {
			return new ScriptTaskTemplateG<A, B, C, D, E, F, G>(ref, a, b, c, d, e, f, g);
		}

		// Routine tasks

		template <class T>
		class RoutineTaskTemplate : public QuickTask {
		public:
			RoutineTaskTemplate(T ref) : callback(ref) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req);
				delete this;
			}

			T callback;
		};

		template <class T>
		ITask* WrapTaskRoutine(T ref) {
			return new RoutineTaskTemplate(ref);
		}
		
		template <class T, class A>
		class RoutineTaskTemplateA : public QuickTask {
		public:
			RoutineTaskTemplateA(T ref, A a) : callback(ref), pa(a) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa);
				delete this;
			}

			T callback;
			A pa;
		};

		template <class T, class A>
		ITask* WrapTaskRoutine(T ref, A a) {
			return new RoutineTaskTemplateA<T, A>(ref, a);
		}

		template <class T, class A, class B>
		class RoutineTaskTemplateB : public QuickTask {
		public:
			RoutineTaskTemplateB(T ref, A a, B b) : callback(ref), pa(a), pb(b) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa, pb);
				delete this;
			}

			T callback;
			A pa;
			B pb;
		};

		template <class T, class A, class B>
		ITask* WrapTaskRoutine(T ref, A a, B b) {
			return new RoutineTaskTemplateB<T, A, B>(ref, a, b);
		}

		template <class T, class A, class B, class C>
		class RoutineTaskTemplateC : public QuickTask {
		public:
			RoutineTaskTemplateC(T ref, A a, B b, C c) : callback(ref), pa(a), pb(b), pc(c) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa, pb, pc);
				delete this;
			}

			T callback;
			A pa;
			B pb;
			C pc;
		};

		template <class T, class A, class B, class C>
		ITask* WrapTaskRoutine(T ref, A a, B b, C c) {
			return new RoutineTaskTemplateC<T, A, B, C>(ref, a, b, c);
		}

		template <class T, class A, class B, class C, class D>
		class RoutineTaskTemplateD : public QuickTask {
		public:
			RoutineTaskTemplateD(T ref, A a, B b, C c, D d) : callback(ref), pa(a), pb(b), pc(c), pd(d) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(pa, pb, pc, pd);
				delete this;
			}

			T callback;
			A pa;
			B pb;
			C pc;
			D pd;
		};

		template <class T, class A, class B, class C, class D>
		ITask* WrapTaskRoutine(T ref, A a, B b, C c, D d) {
			return new RoutineTaskTemplateD<T, A, B, C, D>(ref, a, b, c, d);
		}

		template <class T, class A, class B, class C, class D, class E>
		class RoutineTaskTemplateE : public QuickTask {
		public:
			RoutineTaskTemplateE(T ref, A a, B b, C c, D d, E e) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa, pb, pc, pd, pe);
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
		ITask* WrapTaskRoutine(T ref, A a, B b, C c, D d, E e) {
			return new RoutineTaskTemplateE<T, A, B, C, D, E>(ref, a, b, c, d, e);
		}

		template <class T, class A, class B, class C, class D, class E, class F>
		class RoutineTaskTemplateF : public QuickTask {
		public:
			RoutineTaskTemplateF(T ref, A a, B b, C c, D d, E e, F f) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa, pb, pc, pd, pe, pf);
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
		ITask* WrapTaskRoutine(T ref, A a, B b, C c, D d, E e, F f) {
			return new RoutineTaskTemplateF<T, A, B, C, D, E, F>(ref, a, b, c, d, e, f);
		}

		template <class T, class A, class B, class C, class D, class E, class F, class G>
		class RoutineTaskTemplateG : public QuickTask {
		public:
			RoutineTaskTemplateG(T ref, A a, B b, C c, D d, E e, F f, G g) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f), pg(g) {}
			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				callback(req, pa, pb, pc, pd, pe, pf, pg);
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
		ITask* WrapTaskRoutine(T ref, A a, B b, C c, D d, E e, F f, G g) {
			return new RoutineTaskTemplateG<T, A, B, C, D, E, F, G>(ref, a, b, c, d, e, f, g);
		}

		// QuickRoutine tasks

		template <class T>
		class QuickRoutineTaskTemplate : public QuickTask {
		public:
			QuickRoutineTaskTemplate(T ref) : callback(ref) {}
			virtual void Execute(void* request) override {
				callback();
				delete this;
			}

			T callback;
		};

		template <class T>
		ITask* WrapTaskQuickRoutine(T ref) {
			return new QuickRoutineTaskTemplate(ref);
		}
		
		template <class T, class A>
		class QuickRoutineTaskTemplateA : public QuickTask {
		public:
			QuickRoutineTaskTemplateA(T ref, A a) : callback(ref), pa(a) {}
			virtual void Execute(void* request) override {
				callback(pa);
				delete this;
			}

			T callback;
			A pa;
		};

		template <class T, class A>
		ITask* WrapTaskQuickRoutine(T ref, A a) {
			return new QuickRoutineTaskTemplateA<T, A>(ref, a);
		}

		template <class T, class A, class B>
		class QuickRoutineTaskTemplateB : public QuickTask {
		public:
			QuickRoutineTaskTemplateB(T ref, A a, B b) : callback(ref), pa(a), pb(b) {}
			virtual void Execute(void* request) override {
				callback(pa, pb);
				delete this;
			}

			T callback;
			A pa;
			B pb;
		};

		template <class T, class A, class B>
		ITask* WrapTaskQuickRoutine(T ref, A a, B b) {
			return new QuickRoutineTaskTemplateB<T, A, B>(ref, a, b);
		}

		template <class T, class A, class B, class C>
		class QuickRoutineTaskTemplateC : public QuickTask {
		public:
			QuickRoutineTaskTemplateC(T ref, A a, B b, C c) : callback(ref), pa(a), pb(b), pc(c) {}
			virtual void Execute(void* request) override {
				callback(pa, pb, pc);
				delete this;
			}

			T callback;
			A pa;
			B pb;
			C pc;
		};

		template <class T, class A, class B, class C>
		ITask* WrapTaskQuickRoutine(T ref, A a, B b, C c) {
			return new QuickRoutineTaskTemplateC<T, A, B, C>(ref, a, b, c);
		}

		template <class T, class A, class B, class C, class D>
		class QuickRoutineTaskTemplateD : public QuickTask {
		public:
			QuickRoutineTaskTemplateD(T ref, A a, B b, C c, D d) : callback(ref), pa(a), pb(b), pc(c), pd(d) {}
			virtual void Execute(void* request) override {
				callback(pa, pb, pc, pd);
				delete this;
			}

			T callback;
			A pa;
			B pb;
			C pc;
			D pd;
		};

		template <class T, class A, class B, class C, class D>
		ITask* WrapTaskQuickRoutine(T ref, A a, B b, C c, D d) {
			return new QuickRoutineTaskTemplateD<T, A, B, C, D>(ref, a, b, c, d);
		}

		template <class T, class A, class B, class C, class D, class E>
		class QuickRoutineTaskTemplateE : public QuickTask {
		public:
			QuickRoutineTaskTemplateE(T ref, A a, B b, C c, D d, E e) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e) {}
			virtual void Execute(void* request) override {
				callback(pa, pb, pc, pd, pe);
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
		ITask* WrapTaskQuickRoutine(T ref, A a, B b, C c, D d, E e) {
			return new QuickRoutineTaskTemplateE<T, A, B, C, D, E>(ref, a, b, c, d, e);
		}

		template <class T, class A, class B, class C, class D, class E, class F>
		class QuickRoutineTaskTemplateF : public QuickTask {
		public:
			QuickRoutineTaskTemplateF(T ref, A a, B b, C c, D d, E e, F f) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f) {}
			virtual void Execute(void* request) override {
				callback(pa, pb, pc, pd, pe, pf);
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
		ITask* WrapTaskQuickRoutine(T ref, A a, B b, C c, D d, E e, F f) {
			return new QuickRoutineTaskTemplateF<T, A, B, C, D, E, F>(ref, a, b, c, d, e, f);
		}

		template <class T, class A, class B, class C, class D, class E, class F, class G>
		class QuickRoutineTaskTemplateG : public QuickTask {
		public:
			QuickRoutineTaskTemplateG(T ref, A a, B b, C c, D d, E e, F f, G g) : callback(ref), pa(a), pb(b), pc(c), pd(d), pe(e), pf(f), pg(g) {}
			virtual void Execute(void* request) override {
				callback(pa, pb, pc, pd, pe, pf, pg);
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
		ITask* WrapTaskQuickRoutine(T ref, A a, B b, C c, D d, E e, F f, G g) {
			return new QuickRoutineTaskTemplateG<T, A, B, C, D, E, F, G>(ref, a, b, c, d, e, f, g);
		}
#else
		template <typename... Args>
		class ScriptTaskTemplate : public QuickTask {
		public:
			ScriptTaskTemplate(IScript::Request::Ref c, Args&&... args) : callback(c), arguments(std::forward<Args>(args)...) {}

			template <typename T, size_t index>
			struct Writer {
				void operator () (IScript::Request& request, T& arg) {
					request << std::get<std::tuple_size<T>::value - index>(arg);
					Writer<T, index - 1>()(request, arg);
				}
			};

			template <typename T>
			struct Writer<T, 0> {
				void operator () (IScript::Request& request, T& arg) {}
			};

			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				req.DoLock();
				req.Push();
				Writer<decltype(arguments), sizeof...(Args)>()(req, arguments);
				req.Call(Sync, callback);
				req.Pop();
				req.UnLock();
				delete this;
			}

			IScript::Request::Ref callback;
			std::tuple<Args...> arguments;
		};

		template <typename... Args>
		ITask* WrapTaskScript(IScript::Request::Ref ref, Args&&... args) {
			return new ScriptTaskTemplate<Args...>(ref, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		class RoutineTaskTemplate : public QuickTask {
		public:
			RoutineTaskTemplate(T t, Args&&... args) : callback(t), arguments(std::forward<Args>(args)...) {}

			template <size_t... S>
			void Apply(IScript::Request& context, seq<S...>) {
				callback(context, std::move(std::get<S>(std::move(arguments)))...);
			}

			virtual void Execute(void* request) override {
				IScript::Request& req = *reinterpret_cast<IScript::Request*>(request);
				Apply(req, gen_seq<sizeof...(Args)>());
				delete this;
			}

			T callback;
			std::tuple<Args...> arguments;
		};

		template <typename T, typename... Args>
		ITask* WrapTaskRoutine(T t, Args&&... args) {
			return new RoutineTaskTemplate<T, Args...>(t, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		class QuickRoutineTaskTemplate : public QuickTask {
		public:
			QuickRoutineTaskTemplate(T t, Args&&... args) : callback(t), arguments(std::forward<Args>(args)...) {}

			template <size_t... S>
			void Apply( seq<S...>) {
				callback(std::move(std::get<S>(std::move(arguments)))...);
			}

			virtual void Execute(void* request) override {
				Apply(gen_seq<sizeof...(Args)>());
				delete this;
			}

			T callback;
			std::tuple<Args...> arguments;
		};

		template <typename T, typename... Args>
		ITask* WrapTaskQuickRoutine(T t, Args&&... args) {
			return new QuickRoutineTaskTemplate<T, Args...>(t, std::forward<Args>(args)...);
		}
#endif		
	}
}


#endif // __BRIDGESUNSET_H__
