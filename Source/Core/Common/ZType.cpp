#include "ZType.h"
#include <iostream>
#include <cfloat>
#include <iconv.h>
// #include <Math/LinearEquation.h>

#ifdef MATH_SSE
#include <xmmintrin.h>
#endif

#ifdef UNIFY_STRING_LAYOUT
const size_t PaintsNow::String::npos = -1;
#endif
namespace PaintsNow {
	std::vector<String> Split(const String& str, char sep) {
		std::vector<String> result;
		String current;
		for (size_t i = 0; i < str.length(); i++) {
			char ch = str[i];
			if (ch == sep) {
				if (!current.empty()) {
					result.push_back(current);
					current = "";
				}
			} else {
				current += ch;
			}
		}

		if (!current.empty())
			result.push_back(std::move(current));

		return result;
	}

	uint32_t HashBuffer(const void* buffer, size_t length) {
		unsigned char *key = (unsigned char *)buffer;
		unsigned char *end = key + length;
		uint32_t seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
		int ch;
		while (key != end) {
			ch = *key++;
			seed1 = ch ^ (seed1 + seed2);
			seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
		}

		return seed1;
	}

	ZMatrixFloat4x4 Rotate3D(const ZMatrixFloat4x4& input, float degree, const ZFloat3& d) {
		const TVector<float, 3>& dir = d;
		const double PI = 3.1415926;
		float c = (float)cos(degree / 180 * PI);
		float s = (float)sin(degree / 180 * PI);

		float mat[16] = {
			c + (1 - c) * dir[0] * dir[0], (1 - c) * dir[1] * dir[0] + s * dir[2], (1 - c) * dir[2] * dir[0] - s * dir[1], 0,
			(1 - c) * dir[0] * dir[1] - s * dir[2], c + (1 - c) * dir[1] * dir[1], (1 - c) * dir[2] * dir[1] + s * dir[0], 0,
			(1 - c) * dir[0] * dir[2] + s * dir[1], (1 - c) * dir[1] * dir[2] - s * dir[0], c + (1 - c) * dir[2] * dir[2], 0,
			0, 0, 0, 1
		};

		return ZMatrixFloat4x4(mat) * input;
	}


	ZMatrixFloat4x4 Translate3D(const ZMatrixFloat4x4& input, const ZFloat3& v) {
		float trans[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, v.x(), v.y(), v.z(), 1 };
		return ZMatrixFloat4x4(trans) * input;
	}

	bool Capture3D(const ZMatrixFloat4x4& matrix, const ZFloat3Pair& vec, const ZFloat2Pair& size) {
		ZFloat2Pair rect;
		float dist = FLT_MAX;
		float maxDist = -FLT_MAX;

		for (int i = 0; i < 8; i++) {
			ZFloat4 v = ZFloat4((i & 1) ? vec.second.x() : vec.first.x(), (i & 2) ? vec.second.y() : vec.first.y(), (i & 4) ? vec.second.z() : vec.first.z(), 1) * matrix;
			dist = dist < v.z() ? dist : v.z();
			maxDist = maxDist > v.z() ? maxDist : v.z();
			ZFloat2 p(v.x() / v.w(), v.y() / v.w());
			if (i == 0) rect = ZFloat2Pair(p, p);
			else Include(rect, p);
		}

		return maxDist >= 0 && Overlap(rect, size);
	}

	struct Convert {
	public:
		Convert(const char* to, const char* from) {
			conv = iconv_open(to, from);
		}
		~Convert() {
			iconv_close(conv);
		}

		iconv_t conv;
	};

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define ICONVCHAR const char
#else
#define ICONVCHAR char
#endif

	String Utf8ToSystem(const String& str) {
#if defined(_WIN32) || defined(WIN32)
		static const char* encode = "UCS-2LE";

		static Convert convert(encode, "UTF-8");
		char* buffer = new char[str.size() * 3 + 3];
		char* p = (char*)buffer;
		size_t ps = str.size() * 3;
		ICONVCHAR* q = (ICONVCHAR*)str.c_str();
		size_t qs = str.size();

		iconv(convert.conv, &q, &qs, &p, &ps);
		*p = '\0'; *(p + 1) = '\0';
		String ret(buffer, p - buffer + 2);
		delete[] buffer;
		return ret;
#else
		// static const char* encode = setlocale(LC_ALL, "");

		return str;
#endif

	}

	String SystemToUtf8(const String& str) {
#if defined(_WIN32) || defined(WIN32)
		static const char* encode = "UCS-2LE";
		static Convert convert("UTF-8", encode);
		char* buffer = new char[str.size() * 3 + 3];
		char* p = buffer;
		size_t ps = str.size() * 3;
		ICONVCHAR* q = (ICONVCHAR*)str.c_str();
		size_t qs = str.size();

		iconv(convert.conv, &q, &qs, &p, &ps);
		*p = '\0'; *(p + 1) = '\0';

		String ret(buffer);
		delete[] buffer;

		return ret;
#else
		// static const char* encode = setlocale(LC_ALL, "");
		return str;
#endif
	}

	template <class T>
	bool FloatEqual(T a, T b) {
		return fabs(a - b) < 1e-6;
	}

	template <class T, size_t n>
	class LinearEquation {
	public:
		static int TotalChoiceGauss(TMatrix<T, n, n>& a, T b[]) {
			T MaxValue, tmp;
			int l(1), i, j, is;
			bool yn;

			int js[n];
			int k;

			for (k = 0; k < n - 1; k++) {
				MaxValue = 0.0;
				js[k] = 0;

				for (i = k; i < n; i++)
					for (j = k; j < n; j++) {
						tmp = fabs(a(i, j));
						if (tmp > MaxValue) {
							MaxValue = tmp;
							js[k] = j;
							is = i;
						}
					}

				yn = FloatEqual(MaxValue, (T)0);
				if (yn) l = 0;
				else {
					if (js[k] != k)
						for (i = 0; i < n; i++) std::swap(a(i, k), a(i, js[k]));

					if (is != k) {
						for (j = k; j < n; j++)	std::swap(a(k, j), a(is, j));

						std::swap(b[k], b[is]);
					}
				}

				if (l == 0) {
					return 0;
				}

				MaxValue = fabs(a(k, k));

				for (j = k + 1; j < n; j++)	a(k, j) /= a(k, k);

				b[k] /= a(k, k);
				for (i = k + 1; i < n; i++) {
					for (j = k + 1; j < n; j++) {
						a(i, j) = a(i, j) - a(i, k) * a(k, j);
					}

					b[i] = b[i] - a(i, k) * b[k];
				}
			}

			MaxValue = fabs(a((n - 1), (n - 1)));

			yn = FloatEqual(MaxValue, (T)0);
			if (yn) {
				return(0);
			}

			b[n - 1] /= a((n - 1), (n - 1));

			for (i = n - 2; i >= 0; i--) {
				T t = 0.0;

				for (j = i + 1; j < n; j++)	t = t + a(i, j) * b[j];

				b[i] = b[i] - t;
			}

			js[n - 1] = n - 1;
			for (k = n - 2; k >= 0; k--)
				if (js[k] != k)
					std::swap(b[k], b[js[k]]);

			return (1);
		}
	};

	bool Intersect3D(ZFloat3& res, ZFloat2& uv, const ZFloat3 face[3], const ZFloat3Pair& vec) {
		// handle size!!!!
		const ZFloat3& v = vec.second;
		const ZFloat3& u = vec.first;
		const ZFloat3& base = face[0];
		const ZFloat3& m = face[1];
		const ZFloat3& n = face[2];

		ZFloat3 N = n - base;
		ZFloat3 M = m - base;
		// make linear equations
		ZMatrixFloat3x3 mat;
		mat(0, 0) = M.x(); mat(0, 1) = N.x(); mat(0, 2) = u.x() - v.x();
		mat(1, 0) = M.y(); mat(1, 1) = N.y(); mat(1, 2) = u.y() - v.y();
		mat(2, 0) = M.z(); mat(2, 1) = N.z(); mat(2, 2) = u.z() - v.z();
		float target[3];
		target[0] = u.x() - base.x(); target[1] = u.y() - base.y(); target[2] = u.z() - base.z();

		if (!LinearEquation<float, 3>::TotalChoiceGauss(mat, target)) {
			return false;
		}

		float alpha = target[0];
		float beta = target[1];

		res = base + M * alpha + N * beta;

		const double EPSILON = 1e-4;
		uv = ZFloat2(alpha, beta);
		return (alpha >= -EPSILON && beta >= -EPSILON && (alpha + beta <= 1 + EPSILON));
	}
}
