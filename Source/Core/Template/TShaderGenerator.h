// TShaderGenerator.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-9
//

#ifndef __TSHADERGENERATOR_H__
#define __TSHADERGENERATOR_H__

#include "../Common/ZType.h"

namespace PaintsNow {

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define SHADER_INNER_TYPE T::type
#else
#define SHADER_INNER_TYPE typename T::type
#endif

	struct NotNumber {};
	template <class T>
	struct ShaderVarCheckType1 {
		typedef NotNumber type;
		enum { n = 0 };
	};

	template <>
	struct ShaderVarCheckType1<ZFloat2> {
		typedef float type;
		enum { n = 2 };
		template <class T>
		type& Check(const T& checker) {
			checker.Check(*this);	
			static type t;
			return t;
		}
	};

	template <>
	struct ShaderVarCheckType1<ZFloat3> {
		typedef float type;
		enum { n = 3 };
		template <class T>
		type& Check(const T& checker) {
			checker.Check(*this);	
			static type t;
			return t;
		}
	};

	template <>
	struct ShaderVarCheckType1<ZFloat4> {
		typedef float type;
		enum { n = 4 };
		template <class T>
		type& Check(const T& checker) {
			checker.Check(*this);	
			static type t;
			return t;
		}
	};

	template <size_t n>
	struct ShaderVarCheckBound1 {
		template <class T>
		static SHADER_INNER_TYPE Check(const T&) {
			static_assert(n < T::n, "Invalid subscript.");
			return SHADER_INNER_TYPE();
		}
	};

	template <class T>
	struct ShaderVarCheckType2 {
		typedef NotNumber type;
		enum { n = 0 };
	};

	template <>
	struct ShaderVarCheckType2<ZFloat2> {
		typedef ZFloat2 type;
		enum { n = 2 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <>
	struct ShaderVarCheckType2<ZFloat3> {
		typedef ZFloat2 type;
		enum { n = 3 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <>
	struct ShaderVarCheckType2<ZFloat4> {
		typedef ZFloat2 type;
		enum { n = 4 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <size_t n, size_t m>
	struct ShaderVarCheckBound2 {
		template <class T>
		static SHADER_INNER_TYPE Check(const T&) {
			static_assert(n < T::n && m < T::n, "Invalid subscript.");
			return SHADER_INNER_TYPE();
		}
	};

	template <class T>
	struct ShaderVarCheckType3 {
		typedef NotNumber type;
		enum { n = 0 };
	};

	template <>
	struct ShaderVarCheckType3<ZFloat2> {
		typedef ZFloat3 type;
		enum { n = 2 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);
		}
	};

	template <>
	struct ShaderVarCheckType3<ZFloat3> {
		typedef ZFloat3 type;
		enum { n = 3 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <>
	struct ShaderVarCheckType3<ZFloat4> {
		typedef ZFloat3 type;
		enum { n = 4 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <size_t n, size_t m, size_t k>
	struct ShaderVarCheckBound3 {
		template <class T>
		static SHADER_INNER_TYPE Check(const T&) {
			static_assert(n < T::n && m < T::n && k < T::n, "Invalid subscript.");
			return SHADER_INNER_TYPE();
		}
	};

	template <class T>
	struct ShaderVarCheckType4 {
		typedef NotNumber type;
		enum { n = 0 };
	};

	template <>
	struct ShaderVarCheckType4<ZFloat2> {
		typedef ZFloat4 type;
		enum { n = 2 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);
		}
	};

	template <>
	struct ShaderVarCheckType4<ZFloat3> {
		typedef ZFloat4 type;
		enum { n = 3 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);
		}
	};

	template <>
	struct ShaderVarCheckType4<ZFloat4> {
		typedef ZFloat4 type;
		enum { n = 4 };
		template <class T>
		type Check(const T& checker) {
			return checker.Check(*this);	
		}
	};

	template <size_t n, size_t m, size_t k, size_t j>
	struct ShaderVarCheckBound4 {
			template <class T>
			static SHADER_INNER_TYPE Check(const T&) {
				// no need to check this
				return SHADER_INNER_TYPE();
			}
	};
}


#endif // __TSHADERGENERATOR_H__