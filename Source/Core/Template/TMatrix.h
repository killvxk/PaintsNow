// TMatrix.h -- Basic operations on matrix
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __TMATRIX_H__
#define __TMATRIX_H__


#include "../PaintsNow.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include "TAlgorithm.h"
#include "TVector.h"

namespace PaintsNow {
	template <class T, size_t m = 4, size_t n = m>
	class TMatrix {
	public:
		enum { M = m, N = n };
		TMatrix(T* value) {
			for (size_t i = 0; i < m; i++) {
				for (size_t j = 0; j < n; j++) {
					data[i][j] = *value++;
				}
			}
		}

		template <class D, size_t s, size_t t>
		explicit TMatrix(const TMatrix<D, s, t>& mat) {
			*this = TMatrix<T, m, n>();

			for (size_t i = 0; i < Min(m, s); i++) {
				for (size_t j = 0; j < Min(n, t); j++) {
					data[i][j] = mat.data[i][j];
				}
			}
		}

		TMatrix() {
			size_t z = Min(m, n);
			memset(data, 0, sizeof(data));
			for (size_t i = 0; i < z; i++) {
				data[i][i] = 1;
			}
		}

		TMatrix<T, n, m> Transpose() const {
			TMatrix<T, n, m> target;
			for (size_t i = 0; i < m; i++) {
				for (size_t j = 0; j < n; j++) {
					target(j, i) = (*this)(i, j);
				}
			}

			return target;
		}

		static T Distance(const TMatrix<T, m, n>& lhs, const TMatrix<T, m, n>& rhs) {
			TMatrix<T, m, n> diff;
			TMatrix<T, n, m> star;
			for (size_t i = 0; i < m; i++) {
				for (size_t j = 0; j < n; j++) {
					star(j, i) = diff(i, j) = lhs(i, j) - rhs(i, j);
				}
			}

			TMatrix<T, n, n> result = star * diff;
			T length = 0;
			for (size_t k = 0; k < n; k++) {
				length += result(k, k);
			}

			return (T)sqrt(length);
		}

		operator T* () {
			return data;
		}

		bool operator == (const TMatrix& rhs) const {
			for (size_t i = 0; i < m; i++) {
				for (size_t j = 0; j < n; j++) {
					if (data[i][j] != rhs.data[i][j])
						return false;
				}
			}

			return true;
		}

		bool operator != (const TMatrix& rhs) const {
			return !(*this == rhs);
		}

		T& operator () (size_t i, size_t j) {
			assert(i < m && j < n);
			return data[i][j];
		}

		const T& operator () (size_t i, size_t j) const {
			assert(i < m && j < n);
			return data[i][j];
		}

		T data[m][n]; // m: col index, n: row index
	};

	template <class T, size_t m, size_t n>
	TMatrix<T, m, n> Scale(const TMatrix<T, m, n>& lhs, const TVector<T, m>& v) {
		TMatrix<T, m, n> mat;
		for (size_t i = 0; i < Min(m, n); i++) {
			mat.data[i][i] = v[i];
		}

		return mat * lhs;
	}


#if defined(_MSC_VER) && _MSC_VER <= 1200
	template <class T, size_t m, size_t n>
	TMatrix<T, m, m> operator * (const TMatrix<T, m, n>& lhs, const TMatrix<T, n, m>& rhs) {
		TMatrix<T, m, m> ret;
		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < m; j++) {
				T sum(0);
				for (size_t k = 0; k < n; k++) {
					sum += lhs(i, k) * rhs(k, j);
				}

				ret(i, j) = sum;
			}
		}

		return ret;
	}
#else
	template <class T, size_t m, size_t n, size_t p>
	TMatrix<T, m, p> operator * (const TMatrix<T, m, n>& lhs, const TMatrix<T, n, p>& rhs) {
		TMatrix<T, m, p> ret;
		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < p; j++) {
				T sum(0);
				for (size_t k = 0; k < n; k++) {
					sum += lhs(i, k) * rhs(k, j);
				}

				ret(i, j) = sum;
			}
		}

		return ret;
	}
#endif

	template <class T, size_t m, size_t n>
	TVector<T, n> operator * (const TVector<T, m>& value, const TMatrix<T, m, n>& rhs) {
		TVector<T, n> ret;
		for (size_t i = 0; i < n; i++) {
			ret[i] = 0;
			for (size_t j = 0; j < m; j++) {
				ret[i] += value[j] * rhs(j, i);
			}
		}

		return ret;
	}

	template <class T, size_t n, size_t m>
	TMatrix<T, m, n> Inverse(const TMatrix<T, m, n>& from) {
		static_assert(n == m, "Must be squared matrix.");
		size_t is[n] = { 0 };
		size_t js[n] = { 0 };
		const double EPSILON = 1e-8;

		TMatrix<T, n, n> target = from;

		for (size_t k = 0; k < n; k++) {
			long double MaxValue(0);
			size_t i;
			for (i = k; i < n; i++) {
				for (size_t j = k; j < n; j++) {
					long double tmp(fabs(target(i, j)));
					if (tmp > MaxValue) {
						MaxValue = tmp;
						is[k] = i;
						js[k] = j;
					}
				}
			}

			if (fabs(MaxValue) < EPSILON)
				return target;

			if (is[k] != k) {
				for (size_t j = 0; j < n; j++)
					std::swap(target(k, j), target(is[k], j));
			}

			if (js[k] != k) {
				for (size_t i = 0; i < n; i++)
					std::swap(target(i, k), target(i, js[k]));
			}

			target(k, k) = T(1.0) / target(k, k);
			for (size_t j = 0; j < n; j++) {
				if (j != k)
					target(k, j) *= target(k, k);
			}

			for (i = 0; i < n; i++) {
				if (i != k) {
					for (size_t j = 0; j < n; j++) {
						if (j != k)
							target(i, j) = target(i, j) - target(i, k) * target(k, j);
					}
				}
			}

			for (i = 0; i < n; i++) {
				if (i != k)
					target(i, k) = -target(i, k) * target(k, k);
			}
		}

		for (int r = n - 1; r >= 0; r--) {
			if (js[r] != r) {
				for (size_t j = 0; j < n; j++)
					std::swap(target(r, j), target(js[r], j));
			}

			if (is[r] != r) {
				for (size_t i = 0; i < n; i++)
					std::swap(target(i, r), target(i, is[r]));
			}
		}

		return target;
	}
}


#endif // __TMATRIX_H__
