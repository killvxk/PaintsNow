#ifndef __TATOMIC_H__
#define __TATOMIC_H__

#include <string>
#include <exception>
#include <stdexcept>
#include <algorithm>

#if defined(_MSC_VER) && _MSC_VER <= 1200
#include "../Common/ZType.h"
#include <windows.h>
#include <winbase.h>

namespace std {
	enum memory_order {
		memory_order_relaxed,
		memory_order_consume,
		memory_order_acquire,
		memory_order_release,
		memory_order_acq_rel,
		memory_order_seq_cst
	};
}

#else
#include <atomic>
#endif

namespace PaintsNow {
	// From Microsoft Visual C++ Header file.
	// implement std::atomic<> for Visual C++ 6.0
#if defined(_MSC_VER) && _MSC_VER <= 1200
	template <size_t width>
	class AtomicImpl {};

	// only support 32 bit atomic operations ..
	template <>
	class AtomicImpl<sizeof(uint32_t)> {
	public:
		AtomicImpl(uint32_t v = 0) : value(v) {}

		// always returns void here
		void operator ++ (int) {
			InterlockedIncrement(&value);
		}

		uint32_t operator ++ () {
			return InterlockedIncrement(&value);
		}

		void operator -- (int) {
			InterlockedDecrement(&value);
		}

		uint32_t operator -- () {
			return InterlockedDecrement(&value);
		}

		void operator += (uint32_t arg) {
			InterlockedExchangeAdd(&value, arg);
		}

		void operator -= (uint32_t arg) {
			InterlockedExchangeAdd(&value, (uint32_t)-(int32_t)arg);
		}

		void operator |= (uint32_t arg) {
			LONG Old;

			do {
				Old = value;
			} while (InterlockedCompareExchange(&value,
				Old | arg,
				Old) != Old);
		}

		void operator &= (uint32_t arg) {
			LONG Old;

			do {
				Old = value;
			} while (InterlockedCompareExchange(&value,
				Old & arg,
				Old) != Old);
		}

		void operator ^= (uint32_t arg) {
			LONG Old;

			do {
				Old = value;
			} while (InterlockedCompareExchange(&value,
				Old ^ arg,
				Old) != Old);
		}

		uint32_t load(std::memory_order order = std::memory_order_seq_cst) const {
			return value;
		}

		template <class D>
		void store(D v, std::memory_order order = std::memory_order_seq_cst) {
			value = (uint32_t)v;
		}

		uint32_t operator & (uint32_t t) const {
			return value & t;
		}

		uint32_t operator | (uint32_t t) const {
			return value | t;
		}

		uint32_t operator ^ (uint32_t t) const {
			return value ^ t;
		}

		uint32_t exchange(uint32_t t, std::memory_order order = std::memory_order_seq_cst) {
			return InterlockedExchange(&value, t);
		}

		template <class D>
		bool compare_exchange_weak(D& old, D u, std::memory_order order = std::memory_order_seq_cst) {
			return InterlockedCompareExchange((volatile long*)&value, (uint32_t)u, (long)old) == (long)old;
		}

		template <class D>
		bool compare_exchange_strong(D& old, D u, std::memory_order order = std::memory_order_seq_cst) {
			return InterlockedCompareExchange((volatile long*)&value, (uint32_t)u, (long)old) == (long)old;
		}

	private:
		volatile LONG value;
	};

	template <class T>
	class TAtomic : public AtomicImpl<sizeof(T)> {
	public:
		TAtomic(uint32_t v = 0) : AtomicImpl<sizeof(T)>(v) {}
	};

#else
	template <class T>
	using TAtomic = std::atomic<T>;
#endif

	template <class T, class D>
	bool Verify(TAtomic<T>& var, D bits) {
		return (var & bits) == bits;
	}
}

#endif // __TATOMIC_H__