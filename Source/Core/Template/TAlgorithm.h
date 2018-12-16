// TAlgorithm.h -- Basic algorithms
// By PaintDream (paintdream@paintdream.com)
// 2014-12-2
//

#ifndef __TALGORITHM_H__
#define __TALGORITHM_H__

#include "../PaintsNow.h"
#include <cstring>
#include <algorithm>


namespace PaintsNow {
	template <class T>
	T Max(T a, T b) {
		return a > b ? a : b;
	}


	template <class T>
	T AllMin(const T& lhs, const T& rhs) {
		T result;
		for (size_t i = 0; i < T::size; i++) {
			result[i] = Min(lhs[i], rhs[i]);
		}

		return result;
	}

	template <class T>
	T AllMax(const T& lhs, const T& rhs) {
		T result;
		for (size_t i = 0; i < T::size; i++) {
			result[i] = Max(lhs[i], rhs[i]);
		}

		return result;
	}

	template <class T, class D>
	T Interpolate(const T& a, const T& b, D alpha) {
		return a * (1 - alpha) + b * alpha;
	}

	template <class T>
	T Min(T a, T b) {
		return a < b ? a : b;
	}

	template <class T>
	T Alignment(T a) {
		return a & (~a + 1);
	}

	template <class T>
	T LogAlignmentTop(T t) {
		T z = 0;
		while (t != 0) {
			t >>= 1;
			z++;
		}

		return z;
	}

	template <class T>
	T AlignmentTop(T a) {
		T z = 1, t = a;
		while ((t >>= 1) != 0) {
			z <<= 1;
		}

		return z;
	}

	template <class T>
	T AlignmentRange(T a, T b) {
		T top = AlignmentTop(a ^ b);
		return (a & ~(top - 1)) | top;
	}

	template <class T>
	T SiteCount(T a, T b) {
		T c = AlignmentRange(a, b);
		T count = 0;
		if (a != 0) {
			while (a < c) {
				a += Alignment(a);
				count++;
			}
		} else {
			count++;
		}

		if (b != 0) {
			while (b > c) {
				b -= Alignment(b);
				count++;
			}
		}

		return count;
	}

	template <class T>
	T ReverseBytes(const T& src) {
		T dst = src;
		char* ptr = reinterpret_cast<char*>(&dst);
		for (size_t i = 0; i < sizeof(T) / 2; i++) {
			std::swap(ptr[i], ptr[sizeof(T) - i - 1]);
		}

		return dst;
	}
}


#if !(defined(_MSC_VER) && _MSC_VER <= 1200)
#include <tuple>
#include <utility>
#endif

// seq from stackoverflow http://stackoverflow.com/questions/17424477/implementation-c14-make-integer-sequence By Xeo

#if !(defined(_MSC_VER) && _MSC_VER < 1800) || defined(COMPATIBLE_PROXY)
#ifdef _MSC_VER
	template<unsigned...> struct seq{ typedef seq type; };
	template<class S1, class S2> struct concat;
	template<unsigned... I1, unsigned... I2>
	struct concat<seq<I1...>, seq<I2...>>
		: public seq<I1..., (sizeof...(I1)+I2)...>{};
	template<unsigned N> struct gen_seq;
	template<unsigned N>
	struct gen_seq : public concat<typename gen_seq<N / 2>::type, typename gen_seq<N - N / 2>::type>::type{};
	template<> struct gen_seq<0> : seq<>{};
	template<> struct gen_seq<1> : seq<0>{};
#else
	template< std::size_t ... i >
	struct seq
	{
	    typedef std::size_t value_type;

	    typedef seq<i...> type;

	    // gcc-4.4.7 doesn't support `constexpr` and `noexcept`.
	    static /*constexpr*/ std::size_t size() /*noexcept*/
	    {
	        return sizeof ... (i);
	    }
	};


	// this structure doubles seq elements.
	// s- is number of template arguments in IS.
	template< std::size_t s, typename IS >
	struct doubled_seq;

	template< std::size_t s, std::size_t ... i >
	struct doubled_seq< s, seq<i... > >
	{
	    typedef seq<i..., (s + i)... > type;
	};

	// this structure incremented by one seq, iff NEED-is true,
	// otherwise returns IS
	template< bool NEED, typename IS >
	struct inc_seq;

	template< typename IS >
	struct inc_seq<false,IS>{ typedef IS type; };

	template< std::size_t ... i >
	struct inc_seq< true, seq<i...> >
	{
	    typedef seq<i..., sizeof...(i)> type;
	};



	// helper structure for make_seq.
	template< std::size_t N >
	struct make_seq_impl :
	           inc_seq< (N % 2 != 0),
	                typename doubled_seq< N / 2,
	                           typename make_seq_impl< N / 2> ::type
	               >::type
	       >
	{};

	 // helper structure needs specialization only with 0 element.
	template<>struct make_seq_impl<0>{ typedef seq<> type; };

	// OUR make_seq,  gcc-4.4.7 doesn't support `using`,
	// so we use struct instead of it.
	template< std::size_t N >
	struct gen_seq  : make_seq_impl<N>::type {};

#endif
#endif


#endif // __TALGORITHM_H__
