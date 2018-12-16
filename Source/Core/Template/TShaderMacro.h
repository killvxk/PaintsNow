// TShaderMacro().h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-10
//

#ifndef __TSHADERMACRO_H__
#define __TSHADERMACRO_H__


#ifdef _MSC_VER
#pragma warning(disable:4305) // 1.0 to double
#pragma warning(disable:4244)
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace PaintsNow {
	namespace ShaderMacro {

#define IF (f)
#define ELSE
#define ENDIF

	typedef TShaderVariable<ZFloat4> vec4;
	typedef TShaderVariable<ZFloat3> vec3;
	typedef TShaderVariable<ZFloat2> vec2;
	typedef TShaderVariable<ZMatrixFloat3x3> mat3;
	typedef TShaderVariable<ZMatrixFloat4x4> mat4;

	// just prototypes to make compiler happy. No actual effects.
#define FILTER_FUNCTION(func) \
	template <class T> \
	T func(T input) { return input; } \
	template <class T> \
	TType2<T> func(const TVector<T, 2>& input) { return input;  } \
	template <class T> \
	TType3<T> func(const TVector<T, 3>& input) { return input;  } \
	template <class T> \
	TType4<T> func(const TVector<T, 4>& input) { return input;  }

	FILTER_FUNCTION(normalize);
	FILTER_FUNCTION(saturate);
	FILTER_FUNCTION(radians);
	FILTER_FUNCTION(degrees);
	FILTER_FUNCTION(sin);
	FILTER_FUNCTION(cos);
	FILTER_FUNCTION(tan);
	FILTER_FUNCTION(asin);
	FILTER_FUNCTION(acos);
	FILTER_FUNCTION(atan);
	FILTER_FUNCTION(exp);
	FILTER_FUNCTION(log);
	FILTER_FUNCTION(exp2);
	FILTER_FUNCTION(log2);
	FILTER_FUNCTION(sqrt);
	FILTER_FUNCTION(inversesqrt);
	FILTER_FUNCTION(abs);
	FILTER_FUNCTION(sign);
	FILTER_FUNCTION(floor);
	FILTER_FUNCTION(ceil);
	FILTER_FUNCTION(fract);

#define FILTER_FUNCTION_RET_T(func) \
	template <class T> \
	T func(const TVector<T, 2>& input) { return T(0);  } \
	template <class T> \
	T func(const TVector<T, 3>& input) { return T(0);  } \
	template <class T> \
	T func(const TVector<T, 4>& input) { return T(0);  }

	FILTER_FUNCTION_RET_T(length);


#define FILTER_FUNCTION_TWO(func) \
	template <class T> \
	T func(T input, float other) { return input;  } \
	template <class T> \
	TType2<T> func(const TVector<T, 2>& input, const TVector<T, 2>& other) { return input;  } \
	template <class T> \
	TType3<T> func(const TVector<T, 3>& input, const TVector<T, 3>& other) { return input;  } \
	template <class T> \
	TType4<T> func(const TVector<T, 4>& input, const TVector<T, 4>& other) { return input;  }

	FILTER_FUNCTION_TWO(atan);
	FILTER_FUNCTION_TWO(pow);
	FILTER_FUNCTION_TWO(mod);
	FILTER_FUNCTION_TWO(min);
	FILTER_FUNCTION_TWO(max);
	FILTER_FUNCTION_TWO(step);
	FILTER_FUNCTION_TWO(distance);
	FILTER_FUNCTION_TWO(reflect);
	FILTER_FUNCTION_TWO(refract);

#define FILTER_FUNCTION_TWO_RET_T(func) \
	template <class T> \
	T func(const TVector<T, 2>& input, const TVector<T, 2>& other) { return T(0);  } \
	template <class T> \
	T func(const TVector<T, 3>& input, const TVector<T, 3>& other) { return T(0);  } \
	template <class T> \
	T func(const TVector<T, 4>& input, const TVector<T, 4>& other) { return T(0);  }

	FILTER_FUNCTION_TWO_RET_T(dot);
	template <class T>
	TType3<T> cross(const TVector<T, 3>& input, const TVector<T, 3>& other) {
		return input;
	}

#define FILTER_FUNCTION_THREE(func) \
	template <class T> \
	T func(T input, T other, T last) { return input;  } \
	template <class T> \
	TType2<T> func(const TVector<T, 2>& input, const TVector<T, 2>& other, const TVector<T, 2>& last) { return input;  } \
	template <class T> \
	TType3<T> func(const TVector<T, 3>& input, const TVector<T, 3>& other, const TVector<T, 2>& last) { return input;  } \
	template <class T> \
	TType4<T> func(const TVector<T, 4>& input, const TVector<T, 4>& other, const TVector<T, 2>& last) { return input;  }

	FILTER_FUNCTION_THREE(smoothstep);

	template <class T>
	TType4<T> texture(TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE>& tex, const TVector<T, 2>& v, T bias = 0) {
		return TType4<T>(0, 0, 0, 0);
	}

	template <class T>
	TType4<T> texture(TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE>& tex, const TVector<T, 3>& v, T bias = 0) {
		return TType4<T>(0, 0, 0, 0);
	}

	template <class T>
	TType4<T> texture(TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE>& tex, const TVector<T, 4>& v, T bias = 0) {
		return TType4<T>(0, 0, 0, 0);
	}

	template <class T>
	T lerp(T r, T v, float value) {
		return T();
	}

	template <class T>
	T lerp(T r, T v, T c) {
		return T();
	}

	template <class T>
	TType2<T> lerp(const TVector<T, 2>& r, const TVector<T, 2>& v, T value) {
		return TType2<T>(0, 0);
	}

	template <class T>
	TType3<T> lerp(const TVector<T, 3>& r, const TVector<T, 3>& v, T value) {
		return TType3<T>(0, 0, 0);
	}

	template <class T>
	TType4<T> lerp(const TVector<T, 4>& r, const TVector<T, 4>& v, T value) {
		return TType4<T>(0, 0, 0, 0);
	}

	template <class T, size_t n>
	TMatrix<T, n, n> mult_mat(const TMatrix<T, n, n>& mat, const TMatrix<T, n, n>& rhs) {
		return mat;
	}

	template <class T>
	TType3<T> mult_vec(const ZMatrixFloat3x3& mat, const TVector<T, 3>& value) {
		return value;
	}

	template <class T>
	TType4<T> mult_vec(const ZMatrixFloat4x4& mat, const TVector<T, 4>& value) {
		return value;
	}

	template <class T>
	TType4<T> _EncodeNormalDepth(const TVector<T, 4>& input) {
		return input;
	}

	template <class T>
	TType4<T> _DecodeNormalDepth(const TVector<T, 4>& output) {
		return output;
	}
	}
}


#ifndef USE_SHADER_CODE_DEBUG
#define UnifyShaderCode(code) \
	#code; \
	/* grammar checking */ \
	if (false) { \
		code; \
	}
#else
#define UnifyShaderCode(code) \
	code;
#endif

// one componenjt
#define x vecType1().Check(ShaderVarCheckBound1<0>())
#define y vecType1().Check(ShaderVarCheckBound1<1>())
#define z vecType1().Check(ShaderVarCheckBound1<2>())
#define w vecType1().Check(ShaderVarCheckBound1<3>())

// two components

#define xx vecType2().Check(ShaderVarCheckBound2<0, 0>())
#define xy vecType2().Check(ShaderVarCheckBound2<0, 1>())
#define xz vecType2().Check(ShaderVarCheckBound2<0, 2>())
#define xw vecType2().Check(ShaderVarCheckBound2<0, 3>())
#define yx vecType2().Check(ShaderVarCheckBound2<1, 0>())
#define yy vecType2().Check(ShaderVarCheckBound2<1, 1>())
#define yz vecType2().Check(ShaderVarCheckBound2<1, 2>())
#define yw vecType2().Check(ShaderVarCheckBound2<1, 3>())
#define zx vecType2().Check(ShaderVarCheckBound2<2, 0>())
#define zy vecType2().Check(ShaderVarCheckBound2<2, 1>())
#define zz vecType2().Check(ShaderVarCheckBound2<2, 2>())
#define zw vecType2().Check(ShaderVarCheckBound2<2, 3>())
#define wx vecType2().Check(ShaderVarCheckBound2<3, 0>())
#define wy vecType2().Check(ShaderVarCheckBound2<3, 1>())
#define wz vecType2().Check(ShaderVarCheckBound2<3, 2>())
#define ww vecType2().Check(ShaderVarCheckBound2<3, 3>())

// three components
#define xxx vecType3().Check(ShaderVarCheckBound3<0, 0, 0>())
#define xxy vecType3().Check(ShaderVarCheckBound3<0, 0, 1>())
#define xxz vecType3().Check(ShaderVarCheckBound3<0, 0, 2>())
#define xxw vecType3().Check(ShaderVarCheckBound3<0, 0, 3>())
#define xyx vecType3().Check(ShaderVarCheckBound3<0, 1, 0>())
#define xyy vecType3().Check(ShaderVarCheckBound3<0, 1, 1>())
#define xyz vecType3().Check(ShaderVarCheckBound3<0, 1, 2>())
#define xyw vecType3().Check(ShaderVarCheckBound3<0, 1, 3>())
#define xzx vecType3().Check(ShaderVarCheckBound3<0, 2, 0>())
#define xzy vecType3().Check(ShaderVarCheckBound3<0, 2, 1>())
#define xzz vecType3().Check(ShaderVarCheckBound3<0, 2, 2>())
#define xzw vecType3().Check(ShaderVarCheckBound3<0, 2, 3>())
#define xwx vecType3().Check(ShaderVarCheckBound3<0, 3, 0>())
#define xwy vecType3().Check(ShaderVarCheckBound3<0, 3, 1>())
#define xwz vecType3().Check(ShaderVarCheckBound3<0, 3, 2>())
#define xww vecType3().Check(ShaderVarCheckBound3<0, 3, 3>())
#define yxx vecType3().Check(ShaderVarCheckBound3<1, 0, 0>())
#define yxy vecType3().Check(ShaderVarCheckBound3<1, 0, 1>())
#define yxz vecType3().Check(ShaderVarCheckBound3<1, 0, 2>())
#define yxw vecType3().Check(ShaderVarCheckBound3<1, 0, 3>())
#define yyx vecType3().Check(ShaderVarCheckBound3<1, 1, 0>())
#define yyy vecType3().Check(ShaderVarCheckBound3<1, 1, 1>())
#define yyz vecType3().Check(ShaderVarCheckBound3<1, 1, 2>())
#define yyw vecType3().Check(ShaderVarCheckBound3<1, 1, 3>())
#define yzx vecType3().Check(ShaderVarCheckBound3<1, 2, 0>())
#define yzy vecType3().Check(ShaderVarCheckBound3<1, 2, 1>())
#define yzz vecType3().Check(ShaderVarCheckBound3<1, 2, 2>())
#define yzw vecType3().Check(ShaderVarCheckBound3<1, 2, 3>())
#define ywx vecType3().Check(ShaderVarCheckBound3<1, 3, 0>())
#define ywy vecType3().Check(ShaderVarCheckBound3<1, 3, 1>())
#define ywz vecType3().Check(ShaderVarCheckBound3<1, 3, 2>())
#define yww vecType3().Check(ShaderVarCheckBound3<1, 3, 3>())
#define zxx vecType3().Check(ShaderVarCheckBound3<2, 0, 0>())
#define zxy vecType3().Check(ShaderVarCheckBound3<2, 0, 1>())
#define zxz vecType3().Check(ShaderVarCheckBound3<2, 0, 2>())
#define zxw vecType3().Check(ShaderVarCheckBound3<2, 0, 3>())
#define zyx vecType3().Check(ShaderVarCheckBound3<2, 1, 0>())
#define zyy vecType3().Check(ShaderVarCheckBound3<2, 1, 1>())
#define zyz vecType3().Check(ShaderVarCheckBound3<2, 1, 2>())
#define zyw vecType3().Check(ShaderVarCheckBound3<2, 1, 3>())
#define zzx vecType3().Check(ShaderVarCheckBound3<2, 2, 0>())
#define zzy vecType3().Check(ShaderVarCheckBound3<2, 2, 1>())
#define zzz vecType3().Check(ShaderVarCheckBound3<2, 2, 2>())
#define zzw vecType3().Check(ShaderVarCheckBound3<2, 2, 3>())
#define zwx vecType3().Check(ShaderVarCheckBound3<2, 3, 0>())
#define zwy vecType3().Check(ShaderVarCheckBound3<2, 3, 1>())
#define zwz vecType3().Check(ShaderVarCheckBound3<2, 3, 2>())
#define zww vecType3().Check(ShaderVarCheckBound3<2, 3, 3>())
#define wxx vecType3().Check(ShaderVarCheckBound3<3, 0, 0>())
#define wxy vecType3().Check(ShaderVarCheckBound3<3, 0, 1>())
#define wxz vecType3().Check(ShaderVarCheckBound3<3, 0, 2>())
#define wxw vecType3().Check(ShaderVarCheckBound3<3, 0, 3>())
#define wyx vecType3().Check(ShaderVarCheckBound3<3, 1, 0>())
#define wyy vecType3().Check(ShaderVarCheckBound3<3, 1, 1>())
#define wyz vecType3().Check(ShaderVarCheckBound3<3, 1, 2>())
#define wyw vecType3().Check(ShaderVarCheckBound3<3, 1, 3>())
#define wzx vecType3().Check(ShaderVarCheckBound3<3, 2, 0>())
#define wzy vecType3().Check(ShaderVarCheckBound3<3, 2, 1>())
#define wzz vecType3().Check(ShaderVarCheckBound3<3, 2, 2>())
#define wzw vecType3().Check(ShaderVarCheckBound3<3, 2, 3>())
#define wwx vecType3().Check(ShaderVarCheckBound3<3, 3, 0>())
#define wwy vecType3().Check(ShaderVarCheckBound3<3, 3, 1>())
#define wwz vecType3().Check(ShaderVarCheckBound3<3, 3, 2>())
#define www vecType3().Check(ShaderVarCheckBound3<3, 3, 3>())

// four components
#define xxxx vecType4().Check(ShaderVarCheckBound4<0, 0, 0, 0>())
#define xxxy vecType4().Check(ShaderVarCheckBound4<0, 0, 0, 1>())
#define xxxz vecType4().Check(ShaderVarCheckBound4<0, 0, 0, 2>())
#define xxxw vecType4().Check(ShaderVarCheckBound4<0, 0, 0, 3>())
#define xxyx vecType4().Check(ShaderVarCheckBound4<0, 0, 1, 0>())
#define xxyy vecType4().Check(ShaderVarCheckBound4<0, 0, 1, 1>())
#define xxyz vecType4().Check(ShaderVarCheckBound4<0, 0, 1, 2>())
#define xxyw vecType4().Check(ShaderVarCheckBound4<0, 0, 1, 3>())
#define xxzx vecType4().Check(ShaderVarCheckBound4<0, 0, 2, 0>())
#define xxzy vecType4().Check(ShaderVarCheckBound4<0, 0, 2, 1>())
#define xxzz vecType4().Check(ShaderVarCheckBound4<0, 0, 2, 2>())
#define xxzw vecType4().Check(ShaderVarCheckBound4<0, 0, 2, 3>())
#define xxwx vecType4().Check(ShaderVarCheckBound4<0, 0, 3, 0>())
#define xxwy vecType4().Check(ShaderVarCheckBound4<0, 0, 3, 1>())
#define xxwz vecType4().Check(ShaderVarCheckBound4<0, 0, 3, 2>())
#define xxww vecType4().Check(ShaderVarCheckBound4<0, 0, 3, 3>())
#define xyxx vecType4().Check(ShaderVarCheckBound4<0, 1, 0, 0>())
#define xyxy vecType4().Check(ShaderVarCheckBound4<0, 1, 0, 1>())
#define xyxz vecType4().Check(ShaderVarCheckBound4<0, 1, 0, 2>())
#define xyxw vecType4().Check(ShaderVarCheckBound4<0, 1, 0, 3>())
#define xyyx vecType4().Check(ShaderVarCheckBound4<0, 1, 1, 0>())
#define xyyy vecType4().Check(ShaderVarCheckBound4<0, 1, 1, 1>())
#define xyyz vecType4().Check(ShaderVarCheckBound4<0, 1, 1, 2>())
#define xyyw vecType4().Check(ShaderVarCheckBound4<0, 1, 1, 3>())
#define xyzx vecType4().Check(ShaderVarCheckBound4<0, 1, 2, 0>())
#define xyzy vecType4().Check(ShaderVarCheckBound4<0, 1, 2, 1>())
#define xyzz vecType4().Check(ShaderVarCheckBound4<0, 1, 2, 2>())
#define xyzw vecType4().Check(ShaderVarCheckBound4<0, 1, 2, 3>())
#define xywx vecType4().Check(ShaderVarCheckBound4<0, 1, 3, 0>())
#define xywy vecType4().Check(ShaderVarCheckBound4<0, 1, 3, 1>())
#define xywz vecType4().Check(ShaderVarCheckBound4<0, 1, 3, 2>())
#define xyww vecType4().Check(ShaderVarCheckBound4<0, 1, 3, 3>())
#define xzxx vecType4().Check(ShaderVarCheckBound4<0, 2, 0, 0>())
#define xzxy vecType4().Check(ShaderVarCheckBound4<0, 2, 0, 1>())
#define xzxz vecType4().Check(ShaderVarCheckBound4<0, 2, 0, 2>())
#define xzxw vecType4().Check(ShaderVarCheckBound4<0, 2, 0, 3>())
#define xzyx vecType4().Check(ShaderVarCheckBound4<0, 2, 1, 0>())
#define xzyy vecType4().Check(ShaderVarCheckBound4<0, 2, 1, 1>())
#define xzyz vecType4().Check(ShaderVarCheckBound4<0, 2, 1, 2>())
#define xzyw vecType4().Check(ShaderVarCheckBound4<0, 2, 1, 3>())
#define xzzx vecType4().Check(ShaderVarCheckBound4<0, 2, 2, 0>())
#define xzzy vecType4().Check(ShaderVarCheckBound4<0, 2, 2, 1>())
#define xzzz vecType4().Check(ShaderVarCheckBound4<0, 2, 2, 2>())
#define xzzw vecType4().Check(ShaderVarCheckBound4<0, 2, 2, 3>())
#define xzwx vecType4().Check(ShaderVarCheckBound4<0, 2, 3, 0>())
#define xzwy vecType4().Check(ShaderVarCheckBound4<0, 2, 3, 1>())
#define xzwz vecType4().Check(ShaderVarCheckBound4<0, 2, 3, 2>())
#define xzww vecType4().Check(ShaderVarCheckBound4<0, 2, 3, 3>())
#define xwxx vecType4().Check(ShaderVarCheckBound4<0, 3, 0, 0>())
#define xwxy vecType4().Check(ShaderVarCheckBound4<0, 3, 0, 1>())
#define xwxz vecType4().Check(ShaderVarCheckBound4<0, 3, 0, 2>())
#define xwxw vecType4().Check(ShaderVarCheckBound4<0, 3, 0, 3>())
#define xwyx vecType4().Check(ShaderVarCheckBound4<0, 3, 1, 0>())
#define xwyy vecType4().Check(ShaderVarCheckBound4<0, 3, 1, 1>())
#define xwyz vecType4().Check(ShaderVarCheckBound4<0, 3, 1, 2>())
#define xwyw vecType4().Check(ShaderVarCheckBound4<0, 3, 1, 3>())
#define xwzx vecType4().Check(ShaderVarCheckBound4<0, 3, 2, 0>())
#define xwzy vecType4().Check(ShaderVarCheckBound4<0, 3, 2, 1>())
#define xwzz vecType4().Check(ShaderVarCheckBound4<0, 3, 2, 2>())
#define xwzw vecType4().Check(ShaderVarCheckBound4<0, 3, 2, 3>())
#define xwwx vecType4().Check(ShaderVarCheckBound4<0, 3, 3, 0>())
#define xwwy vecType4().Check(ShaderVarCheckBound4<0, 3, 3, 1>())
#define xwwz vecType4().Check(ShaderVarCheckBound4<0, 3, 3, 2>())
#define xwww vecType4().Check(ShaderVarCheckBound4<0, 3, 3, 3>())
#define yxxx vecType4().Check(ShaderVarCheckBound4<1, 0, 0, 0>())
#define yxxy vecType4().Check(ShaderVarCheckBound4<1, 0, 0, 1>())
#define yxxz vecType4().Check(ShaderVarCheckBound4<1, 0, 0, 2>())
#define yxxw vecType4().Check(ShaderVarCheckBound4<1, 0, 0, 3>())
#define yxyx vecType4().Check(ShaderVarCheckBound4<1, 0, 1, 0>())
#define yxyy vecType4().Check(ShaderVarCheckBound4<1, 0, 1, 1>())
#define yxyz vecType4().Check(ShaderVarCheckBound4<1, 0, 1, 2>())
#define yxyw vecType4().Check(ShaderVarCheckBound4<1, 0, 1, 3>())
#define yxzx vecType4().Check(ShaderVarCheckBound4<1, 0, 2, 0>())
#define yxzy vecType4().Check(ShaderVarCheckBound4<1, 0, 2, 1>())
#define yxzz vecType4().Check(ShaderVarCheckBound4<1, 0, 2, 2>())
#define yxzw vecType4().Check(ShaderVarCheckBound4<1, 0, 2, 3>())
#define yxwx vecType4().Check(ShaderVarCheckBound4<1, 0, 3, 0>())
#define yxwy vecType4().Check(ShaderVarCheckBound4<1, 0, 3, 1>())
#define yxwz vecType4().Check(ShaderVarCheckBound4<1, 0, 3, 2>())
#define yxww vecType4().Check(ShaderVarCheckBound4<1, 0, 3, 3>())
#define yyxx vecType4().Check(ShaderVarCheckBound4<1, 1, 0, 0>())
#define yyxy vecType4().Check(ShaderVarCheckBound4<1, 1, 0, 1>())
#define yyxz vecType4().Check(ShaderVarCheckBound4<1, 1, 0, 2>())
#define yyxw vecType4().Check(ShaderVarCheckBound4<1, 1, 0, 3>())
#define yyyx vecType4().Check(ShaderVarCheckBound4<1, 1, 1, 0>())
#define yyyy vecType4().Check(ShaderVarCheckBound4<1, 1, 1, 1>())
#define yyyz vecType4().Check(ShaderVarCheckBound4<1, 1, 1, 2>())
#define yyyw vecType4().Check(ShaderVarCheckBound4<1, 1, 1, 3>())
#define yyzx vecType4().Check(ShaderVarCheckBound4<1, 1, 2, 0>())
#define yyzy vecType4().Check(ShaderVarCheckBound4<1, 1, 2, 1>())
#define yyzz vecType4().Check(ShaderVarCheckBound4<1, 1, 2, 2>())
#define yyzw vecType4().Check(ShaderVarCheckBound4<1, 1, 2, 3>())
#define yywx vecType4().Check(ShaderVarCheckBound4<1, 1, 3, 0>())
#define yywy vecType4().Check(ShaderVarCheckBound4<1, 1, 3, 1>())
#define yywz vecType4().Check(ShaderVarCheckBound4<1, 1, 3, 2>())
#define yyww vecType4().Check(ShaderVarCheckBound4<1, 1, 3, 3>())
#define yzxx vecType4().Check(ShaderVarCheckBound4<1, 2, 0, 0>())
#define yzxy vecType4().Check(ShaderVarCheckBound4<1, 2, 0, 1>())
#define yzxz vecType4().Check(ShaderVarCheckBound4<1, 2, 0, 2>())
#define yzxw vecType4().Check(ShaderVarCheckBound4<1, 2, 0, 3>())
#define yzyx vecType4().Check(ShaderVarCheckBound4<1, 2, 1, 0>())
#define yzyy vecType4().Check(ShaderVarCheckBound4<1, 2, 1, 1>())
#define yzyz vecType4().Check(ShaderVarCheckBound4<1, 2, 1, 2>())
#define yzyw vecType4().Check(ShaderVarCheckBound4<1, 2, 1, 3>())
#define yzzx vecType4().Check(ShaderVarCheckBound4<1, 2, 2, 0>())
#define yzzy vecType4().Check(ShaderVarCheckBound4<1, 2, 2, 1>())
#define yzzz vecType4().Check(ShaderVarCheckBound4<1, 2, 2, 2>())
#define yzzw vecType4().Check(ShaderVarCheckBound4<1, 2, 2, 3>())
#define yzwx vecType4().Check(ShaderVarCheckBound4<1, 2, 3, 0>())
#define yzwy vecType4().Check(ShaderVarCheckBound4<1, 2, 3, 1>())
#define yzwz vecType4().Check(ShaderVarCheckBound4<1, 2, 3, 2>())
#define yzww vecType4().Check(ShaderVarCheckBound4<1, 2, 3, 3>())
#define ywxx vecType4().Check(ShaderVarCheckBound4<1, 3, 0, 0>())
#define ywxy vecType4().Check(ShaderVarCheckBound4<1, 3, 0, 1>())
#define ywxz vecType4().Check(ShaderVarCheckBound4<1, 3, 0, 2>())
#define ywxw vecType4().Check(ShaderVarCheckBound4<1, 3, 0, 3>())
#define ywyx vecType4().Check(ShaderVarCheckBound4<1, 3, 1, 0>())
#define ywyy vecType4().Check(ShaderVarCheckBound4<1, 3, 1, 1>())
#define ywyz vecType4().Check(ShaderVarCheckBound4<1, 3, 1, 2>())
#define ywyw vecType4().Check(ShaderVarCheckBound4<1, 3, 1, 3>())
#define ywzx vecType4().Check(ShaderVarCheckBound4<1, 3, 2, 0>())
#define ywzy vecType4().Check(ShaderVarCheckBound4<1, 3, 2, 1>())
#define ywzz vecType4().Check(ShaderVarCheckBound4<1, 3, 2, 2>())
#define ywzw vecType4().Check(ShaderVarCheckBound4<1, 3, 2, 3>())
#define ywwx vecType4().Check(ShaderVarCheckBound4<1, 3, 3, 0>())
#define ywwy vecType4().Check(ShaderVarCheckBound4<1, 3, 3, 1>())
#define ywwz vecType4().Check(ShaderVarCheckBound4<1, 3, 3, 2>())
#define ywww vecType4().Check(ShaderVarCheckBound4<1, 3, 3, 3>())
#define zxxx vecType4().Check(ShaderVarCheckBound4<2, 0, 0, 0>())
#define zxxy vecType4().Check(ShaderVarCheckBound4<2, 0, 0, 1>())
#define zxxz vecType4().Check(ShaderVarCheckBound4<2, 0, 0, 2>())
#define zxxw vecType4().Check(ShaderVarCheckBound4<2, 0, 0, 3>())
#define zxyx vecType4().Check(ShaderVarCheckBound4<2, 0, 1, 0>())
#define zxyy vecType4().Check(ShaderVarCheckBound4<2, 0, 1, 1>())
#define zxyz vecType4().Check(ShaderVarCheckBound4<2, 0, 1, 2>())
#define zxyw vecType4().Check(ShaderVarCheckBound4<2, 0, 1, 3>())
#define zxzx vecType4().Check(ShaderVarCheckBound4<2, 0, 2, 0>())
#define zxzy vecType4().Check(ShaderVarCheckBound4<2, 0, 2, 1>())
#define zxzz vecType4().Check(ShaderVarCheckBound4<2, 0, 2, 2>())
#define zxzw vecType4().Check(ShaderVarCheckBound4<2, 0, 2, 3>())
#define zxwx vecType4().Check(ShaderVarCheckBound4<2, 0, 3, 0>())
#define zxwy vecType4().Check(ShaderVarCheckBound4<2, 0, 3, 1>())
#define zxwz vecType4().Check(ShaderVarCheckBound4<2, 0, 3, 2>())
#define zxww vecType4().Check(ShaderVarCheckBound4<2, 0, 3, 3>())
#define zyxx vecType4().Check(ShaderVarCheckBound4<2, 1, 0, 0>())
#define zyxy vecType4().Check(ShaderVarCheckBound4<2, 1, 0, 1>())
#define zyxz vecType4().Check(ShaderVarCheckBound4<2, 1, 0, 2>())
#define zyxw vecType4().Check(ShaderVarCheckBound4<2, 1, 0, 3>())
#define zyyx vecType4().Check(ShaderVarCheckBound4<2, 1, 1, 0>())
#define zyyy vecType4().Check(ShaderVarCheckBound4<2, 1, 1, 1>())
#define zyyz vecType4().Check(ShaderVarCheckBound4<2, 1, 1, 2>())
#define zyyw vecType4().Check(ShaderVarCheckBound4<2, 1, 1, 3>())
#define zyzx vecType4().Check(ShaderVarCheckBound4<2, 1, 2, 0>())
#define zyzy vecType4().Check(ShaderVarCheckBound4<2, 1, 2, 1>())
#define zyzz vecType4().Check(ShaderVarCheckBound4<2, 1, 2, 2>())
#define zyzw vecType4().Check(ShaderVarCheckBound4<2, 1, 2, 3>())
#define zywx vecType4().Check(ShaderVarCheckBound4<2, 1, 3, 0>())
#define zywy vecType4().Check(ShaderVarCheckBound4<2, 1, 3, 1>())
#define zywz vecType4().Check(ShaderVarCheckBound4<2, 1, 3, 2>())
#define zyww vecType4().Check(ShaderVarCheckBound4<2, 1, 3, 3>())
#define zzxx vecType4().Check(ShaderVarCheckBound4<2, 2, 0, 0>())
#define zzxy vecType4().Check(ShaderVarCheckBound4<2, 2, 0, 1>())
#define zzxz vecType4().Check(ShaderVarCheckBound4<2, 2, 0, 2>())
#define zzxw vecType4().Check(ShaderVarCheckBound4<2, 2, 0, 3>())
#define zzyx vecType4().Check(ShaderVarCheckBound4<2, 2, 1, 0>())
#define zzyy vecType4().Check(ShaderVarCheckBound4<2, 2, 1, 1>())
#define zzyz vecType4().Check(ShaderVarCheckBound4<2, 2, 1, 2>())
#define zzyw vecType4().Check(ShaderVarCheckBound4<2, 2, 1, 3>())
#define zzzx vecType4().Check(ShaderVarCheckBound4<2, 2, 2, 0>())
#define zzzy vecType4().Check(ShaderVarCheckBound4<2, 2, 2, 1>())
#define zzzz vecType4().Check(ShaderVarCheckBound4<2, 2, 2, 2>())
#define zzzw vecType4().Check(ShaderVarCheckBound4<2, 2, 2, 3>())
#define zzwx vecType4().Check(ShaderVarCheckBound4<2, 2, 3, 0>())
#define zzwy vecType4().Check(ShaderVarCheckBound4<2, 2, 3, 1>())
#define zzwz vecType4().Check(ShaderVarCheckBound4<2, 2, 3, 2>())
#define zzww vecType4().Check(ShaderVarCheckBound4<2, 2, 3, 3>())
#define zwxx vecType4().Check(ShaderVarCheckBound4<2, 3, 0, 0>())
#define zwxy vecType4().Check(ShaderVarCheckBound4<2, 3, 0, 1>())
#define zwxz vecType4().Check(ShaderVarCheckBound4<2, 3, 0, 2>())
#define zwxw vecType4().Check(ShaderVarCheckBound4<2, 3, 0, 3>())
#define zwyx vecType4().Check(ShaderVarCheckBound4<2, 3, 1, 0>())
#define zwyy vecType4().Check(ShaderVarCheckBound4<2, 3, 1, 1>())
#define zwyz vecType4().Check(ShaderVarCheckBound4<2, 3, 1, 2>())
#define zwyw vecType4().Check(ShaderVarCheckBound4<2, 3, 1, 3>())
#define zwzx vecType4().Check(ShaderVarCheckBound4<2, 3, 2, 0>())
#define zwzy vecType4().Check(ShaderVarCheckBound4<2, 3, 2, 1>())
#define zwzz vecType4().Check(ShaderVarCheckBound4<2, 3, 2, 2>())
#define zwzw vecType4().Check(ShaderVarCheckBound4<2, 3, 2, 3>())
#define zwwx vecType4().Check(ShaderVarCheckBound4<2, 3, 3, 0>())
#define zwwy vecType4().Check(ShaderVarCheckBound4<2, 3, 3, 1>())
#define zwwz vecType4().Check(ShaderVarCheckBound4<2, 3, 3, 2>())
#define zwww vecType4().Check(ShaderVarCheckBound4<2, 3, 3, 3>())
#define wxxx vecType4().Check(ShaderVarCheckBound4<3, 0, 0, 0>())
#define wxxy vecType4().Check(ShaderVarCheckBound4<3, 0, 0, 1>())
#define wxxz vecType4().Check(ShaderVarCheckBound4<3, 0, 0, 2>())
#define wxxw vecType4().Check(ShaderVarCheckBound4<3, 0, 0, 3>())
#define wxyx vecType4().Check(ShaderVarCheckBound4<3, 0, 1, 0>())
#define wxyy vecType4().Check(ShaderVarCheckBound4<3, 0, 1, 1>())
#define wxyz vecType4().Check(ShaderVarCheckBound4<3, 0, 1, 2>())
#define wxyw vecType4().Check(ShaderVarCheckBound4<3, 0, 1, 3>())
#define wxzx vecType4().Check(ShaderVarCheckBound4<3, 0, 2, 0>())
#define wxzy vecType4().Check(ShaderVarCheckBound4<3, 0, 2, 1>())
#define wxzz vecType4().Check(ShaderVarCheckBound4<3, 0, 2, 2>())
#define wxzw vecType4().Check(ShaderVarCheckBound4<3, 0, 2, 3>())
#define wxwx vecType4().Check(ShaderVarCheckBound4<3, 0, 3, 0>())
#define wxwy vecType4().Check(ShaderVarCheckBound4<3, 0, 3, 1>())
#define wxwz vecType4().Check(ShaderVarCheckBound4<3, 0, 3, 2>())
#define wxww vecType4().Check(ShaderVarCheckBound4<3, 0, 3, 3>())
#define wyxx vecType4().Check(ShaderVarCheckBound4<3, 1, 0, 0>())
#define wyxy vecType4().Check(ShaderVarCheckBound4<3, 1, 0, 1>())
#define wyxz vecType4().Check(ShaderVarCheckBound4<3, 1, 0, 2>())
#define wyxw vecType4().Check(ShaderVarCheckBound4<3, 1, 0, 3>())
#define wyyx vecType4().Check(ShaderVarCheckBound4<3, 1, 1, 0>())
#define wyyy vecType4().Check(ShaderVarCheckBound4<3, 1, 1, 1>())
#define wyyz vecType4().Check(ShaderVarCheckBound4<3, 1, 1, 2>())
#define wyyw vecType4().Check(ShaderVarCheckBound4<3, 1, 1, 3>())
#define wyzx vecType4().Check(ShaderVarCheckBound4<3, 1, 2, 0>())
#define wyzy vecType4().Check(ShaderVarCheckBound4<3, 1, 2, 1>())
#define wyzz vecType4().Check(ShaderVarCheckBound4<3, 1, 2, 2>())
#define wyzw vecType4().Check(ShaderVarCheckBound4<3, 1, 2, 3>())
#define wywx vecType4().Check(ShaderVarCheckBound4<3, 1, 3, 0>())
#define wywy vecType4().Check(ShaderVarCheckBound4<3, 1, 3, 1>())
#define wywz vecType4().Check(ShaderVarCheckBound4<3, 1, 3, 2>())
#define wyww vecType4().Check(ShaderVarCheckBound4<3, 1, 3, 3>())
#define wzxx vecType4().Check(ShaderVarCheckBound4<3, 2, 0, 0>())
#define wzxy vecType4().Check(ShaderVarCheckBound4<3, 2, 0, 1>())
#define wzxz vecType4().Check(ShaderVarCheckBound4<3, 2, 0, 2>())
#define wzxw vecType4().Check(ShaderVarCheckBound4<3, 2, 0, 3>())
#define wzyx vecType4().Check(ShaderVarCheckBound4<3, 2, 1, 0>())
#define wzyy vecType4().Check(ShaderVarCheckBound4<3, 2, 1, 1>())
#define wzyz vecType4().Check(ShaderVarCheckBound4<3, 2, 1, 2>())
#define wzyw vecType4().Check(ShaderVarCheckBound4<3, 2, 1, 3>())
#define wzzx vecType4().Check(ShaderVarCheckBound4<3, 2, 2, 0>())
#define wzzy vecType4().Check(ShaderVarCheckBound4<3, 2, 2, 1>())
#define wzzz vecType4().Check(ShaderVarCheckBound4<3, 2, 2, 2>())
#define wzzw vecType4().Check(ShaderVarCheckBound4<3, 2, 2, 3>())
#define wzwx vecType4().Check(ShaderVarCheckBound4<3, 2, 3, 0>())
#define wzwy vecType4().Check(ShaderVarCheckBound4<3, 2, 3, 1>())
#define wzwz vecType4().Check(ShaderVarCheckBound4<3, 2, 3, 2>())
#define wzww vecType4().Check(ShaderVarCheckBound4<3, 2, 3, 3>())
#define wwxx vecType4().Check(ShaderVarCheckBound4<3, 3, 0, 0>())
#define wwxy vecType4().Check(ShaderVarCheckBound4<3, 3, 0, 1>())
#define wwxz vecType4().Check(ShaderVarCheckBound4<3, 3, 0, 2>())
#define wwxw vecType4().Check(ShaderVarCheckBound4<3, 3, 0, 3>())
#define wwyx vecType4().Check(ShaderVarCheckBound4<3, 3, 1, 0>())
#define wwyy vecType4().Check(ShaderVarCheckBound4<3, 3, 1, 1>())
#define wwyz vecType4().Check(ShaderVarCheckBound4<3, 3, 1, 2>())
#define wwyw vecType4().Check(ShaderVarCheckBound4<3, 3, 1, 3>())
#define wwzx vecType4().Check(ShaderVarCheckBound4<3, 3, 2, 0>())
#define wwzy vecType4().Check(ShaderVarCheckBound4<3, 3, 2, 1>())
#define wwzz vecType4().Check(ShaderVarCheckBound4<3, 3, 2, 2>())
#define wwzw vecType4().Check(ShaderVarCheckBound4<3, 3, 2, 3>())
#define wwwx vecType4().Check(ShaderVarCheckBound4<3, 3, 3, 0>())
#define wwwy vecType4().Check(ShaderVarCheckBound4<3, 3, 3, 1>())
#define wwwz vecType4().Check(ShaderVarCheckBound4<3, 3, 3, 2>())
#define wwww vecType4().Check(ShaderVarCheckBound4<3, 3, 3, 3>())



#endif // __TSHADERMACRO_H__