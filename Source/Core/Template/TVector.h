// TVector -- Basic abstract template vector
// By PaintDream (paintdream@paintdream.com)
// 2014-11-30
//

#ifndef __TVECTOR_H__
#define __TVECTOR_H__


#include "../PaintsNow.h"
#include "TAlgorithm.h"
#include <cstring>
#include <cmath>
#include <algorithm>

namespace PaintsNow {
	template <class T, size_t n>
	class TVector {
	public:
		typedef T type;
		TVector(const std::pair<TVector<T, n / 2>, TVector<T, n / 2> >& p) {
			memcpy(data, &p.first.data[0], n * sizeof(T) / 2);
			memcpy(data + n / 2, &p.second.data[0], n * sizeof(T) / 2);
		}

		TVector(T* v = nullptr) {
			if (v != nullptr) {
				std::copy(v, v + n, data);
			}
		}

		enum { size = n };
		operator const T* () const {
			return data;
		}
		operator T* () {
			return data;
		}

		bool operator == (const TVector& rhs) const {
			for (size_t i = 0; i < n; i++) {
				if (data[i] != rhs.data[i]) return false;
			}
			return true;
		}

		const T operator [] (size_t d) const {
			return data[d];
		}

		T& operator [] (size_t d) {
			return data[d];
		}

		const T operator [] (int d) const {
			return data[d];
		}

		T& operator [] (int d) {
			return data[d];
		}

		template <class D>
		TVector<T, n>& Foreach(D op) {
			for (size_t i = 0; i < n; i++) {
				op(data[i]);
			}

			return *this;
		}

		T Length() const {
			return (T)sqrt(SquareLength());
		}

		TVector<T, n> operator - () const {
			TVector<T, n> ret;
			for (size_t i = 0; i < n; i++) {
				ret[i] = - data[i];
			}

			return ret;
		}

		TVector<T, n>& operator += (const TVector<T, n>& data) {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] += data[i];
			}

			return ret;
		}

		TVector<T, n>& operator -= (const TVector<T, n>& data) {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] -= data[i];
			}

			return ret;
		}

		TVector<T, n>& operator *= (const TVector<T, n>& data) {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] *= data[i];
			}

			return ret;
		}

		TVector<T, n>& operator /= (const TVector<T, n>& data) {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] /= data[i];
			}

			return ret;
		}

		TVector<T, n>& operator *= (const T& data) {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] *= data;
			}

			return ret;
		}

		TVector<T, n>& operator /= (const T& data) const {
			TVector<T, n>& ret = *this;
			for (size_t i = 0; i < n; i++) {
				ret[i] /= data;
			}

			return ret;
		}

		bool operator < (const TVector<T, n>& rhs) const {
			const TVector<T, n>& lhs = *this;
			for (size_t i = 0; i < n; i++) {
				if (lhs[i] < rhs[i])
					return true;
				else if (rhs[i] < lhs[i])
					return false;
			}

			return false;
		}

		T SquareLength() const {
			T t(0);
			for (size_t i = 0; i < n; i++) {
				t += data[i] * data[i];
			}
			
			return t;
		}

		TVector<T, n>& Normalize() {
			T length = Length();
			for (size_t j = 0; j < n; j++) {
				data[j] /= length;
			}

			return *this;
		}

		T data[n];
	};

#define VISIT(X, index) \
	inline const T X() const { return (*this)[index]; } \
	inline T& X() { return (*this)[index]; }

	template <class T>
	struct TType2 : public TVector<T, 2> {
		TType2() { memset(this, 0, sizeof(*this)); }
		TType2(const std::pair<TVector<T, 1>, TVector<T, 1> >& p) : TVector<T, 2>(p) {}
		TType2(const TVector<T, 2>& v) : TVector<T, 2>(v) {}
		TType2(T xx, T yy) { x() = xx; y() = yy; }

		VISIT(x, 0);
		VISIT(y, 1);
		VISIT(s, 0);
		VISIT(t, 1);
	};

	template <class T>
	struct TType3 : public TVector<T, 3> {
		TType3() { memset(this, 0, sizeof(*this)); }
		TType3(const TVector<T, 3>& v) : TVector<T, 3>(v) {}
		TType3(T xx, T yy, T zz) { x() = xx; y() = yy; z() = zz; }

		VISIT(x, 0);
		VISIT(y, 1);
		VISIT(z, 2);

		VISIT(r, 0);
		VISIT(g, 1);
		VISIT(b, 2);
	};

	template <class T>
	struct TType4 : public TVector<T, 4> {
		TType4() { memset(this, 0, sizeof(*this)); }
		TType4(const std::pair<TVector<T, 2>, TVector<T, 2> >& p) : TVector<T, 4>(p) {}
		TType4(const TVector<T, 4>& v) : TVector<T, 4>(v) {}
		TType4(T xx, T yy, T zz, T ww) { x() = xx; y() = yy; z() = zz; w() = ww; }

		VISIT(x, 0);
		VISIT(y, 1);
		VISIT(z, 2);
		VISIT(w, 3);

		VISIT(r, 0);
		VISIT(g, 1);
		VISIT(b, 2);
		VISIT(a, 3);
	};

	// +
	template <class T, size_t n>
	TVector<T, n> operator + (const TVector<T, n>& lhs, T rhs) {
		TVector<T, n> v = lhs;
		for (size_t i = 0; i < n; i++) {
			v[i] += rhs;
		}

		return v;
	}

	template <class T, size_t n>
	TVector<T, n>& operator += (TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] += rhs[i];
		}
		
		return lhs;
	}

	template <class T, size_t n>
	TVector<T, n> operator + (const TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		TVector<T, n> res(lhs);
		res += rhs;
		return res;
	}
	
	template <class T, size_t n>
	TVector<T, n> operator += (TVector<T, n>& lhs, T t) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] += t;
		}
		return lhs;
	}

	// -
	template <class T, size_t n>
	TVector<T, n> operator - (const TVector<T, n>& lhs, T rhs) {
		TVector<T, n> v = lhs;
		for (size_t i = 0; i < n; i++) {
			v[i] -= rhs;
		}

		return v;
	}

	template <class T, size_t n>
	TVector<T, n>& operator -= (TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] -= rhs[i];
		}
		
		return lhs;
	}

	template <class T, size_t n>
	TVector<T, n> operator - (const TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		TVector<T, n> res(lhs);
		res -= rhs;
		return res;
	}
	
	template <class T, size_t n>
	TVector<T, n> operator -= (TVector<T, n>& lhs, T t) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] -= t;
		}
		return lhs;
	}

	// *


	template <class T, size_t n>
	TVector<T, n>& operator *= (TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] *= rhs[i];
		}
		
		return lhs;
	}

	template <class T, size_t n>
	TVector<T, n> operator * (const TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		TVector<T, n> res(lhs);
		res *= rhs;
		return  res;
	}
	
	template <class T, size_t n>
	TVector<T, n> operator * (const TVector<T, n>& lhs, T t) {
		TVector<T, n> result = lhs;
		for (size_t i = 0; i < n; i++) {
			result[i] *= t;
		}
		return result;
	}

	template <class T, size_t n>
	TVector<T, n> operator *= (TVector<T, n>& lhs, T t) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] *= t;
		}
		return lhs;
	}

	// /


	template <class T, size_t n>
	TVector<T, n> operator / (const TVector<T, n>& lhs, T t) {
		TVector<T, n> result = lhs;
		for (size_t i = 0; i < n; i++) {
			result[i] /= t;
		}
		return result;
	}

	template <class T, size_t n>
	TVector<T, n>& operator /= (TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] /= rhs[i];
		}
		
		return lhs;
	}

	template <class T, size_t n>
	TVector<T, n> operator / (const TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		TVector<T, n> res(lhs);
		res /= rhs;
		return res;
	}
	
	template <class T, size_t n>
	TVector<T, n> operator /= (TVector<T, n>& lhs, T t) {
		for (size_t i = 0; i < n; i++) {
			lhs[i] /= t;
		}
		return lhs;
	}

	template <class T, size_t n>
	T DotProduct(const TVector<T, n>& lhs, const TVector<T, n>& rhs) {
		T res(0);
		for (size_t i = 0; i < n; i++) {
			res += lhs[i] * rhs[i];
		}

		return res;
	}

	template <class T>
	T CrossProduct(const TVector<T, 2>& lhs, const TVector<T, 2>& rhs) {
		return lhs[0] * rhs[1] - lhs[1] * rhs[0];
	}

	template <class T>
	TVector<T, 3> CrossProduct(const TVector<T, 3>& lhs, const TVector<T, 3>& rhs) {
		TVector<T, 3> t;
		t[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
		t[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
		t[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];

		return t;
	}

	template <class T>
	bool CrossPoint(TVector<T, 2>& result, const TVector<T, 2>& ps, const TVector<T, 2>& pt, const TVector<T, 2>& qs, const TVector<T, 2>& qt, T& alpha, T& beta) {
		T z = CrossProduct(pt - ps, qt - qs);
		if (fabs(z) < 1e-10)
			return false;

		T A1 = pt[0] - ps[0], A2 = qs[0] - qt[0]; 
		T B1 = pt[1] - ps[1], B2 = qs[1] - qt[1]; 
		T C1 = qs[0] - ps[0];
		T C2 = qs[1] - ps[1];
		T D = A1 * B2 - A2 * B1;

		alpha = (B1 * C2 - B2 * C1) / D;
		beta = (C1 * A2 - C2 * A1) / D;

		result = ((ps + (pt - ps) * alpha) + (qs + (qt - qs) * beta)) / T(2);

		return true;
	}


	template <class T, size_t n>
	class TRange : public std::pair<TVector<T, n>, TVector<T, n> > {};

	template <class T>
	bool Clip(std::pair<T, T>& lhs, const std::pair<T, T>& rhs) {
		bool b = true;
		for (size_t i = 0; i < T::size; i++) {
			lhs.first[i] = Max(lhs.first[i], rhs.first[i]);
			lhs.second[i] = Min(lhs.second[i], rhs.second[i]);

			if (lhs.first[i] > lhs.second[i])
				b = false;
		}

		return b;
	}

	template <class T>
	std::pair<T, T>& Merge(std::pair<T, T>& host, const std::pair<T, T>& rhs) {
		Include(host, rhs.first);
		Include(host, rhs.second);
		return host;
	}

	template <class T>
	std::pair<T, T>& Include(std::pair<T, T>& host, const T& value) {
		for (size_t i = 0; i < T::size; i++) {
			host.first[i] = Min(host.first[i], value[i]);
			host.second[i] = Max(host.second[i], value[i]);
		}

		return host;
	}

	template <class T>
	bool Overlap(const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) {
		for (size_t i = 0; i < T::size; i++) {
			if (rhs.second[i] < lhs.first[i] || lhs.second[i] < rhs.first[i])
				return false;
		}

		return true;
	}

	template <class T>
	bool Contain(const std::pair<T, T>& host, const T& value) {
		for (size_t i = 0; i < T::size; i++) {
			if (value[i] < host.first[i] || host.second[i] < value[i]) {
				return false;
			}
		}

		return true;
	}

	template <class T>
	bool Contain(const std::pair<T, T>& host, const std::pair<T, T>& value) {
		return Contain(host, value.first) && Contain(host, value.second);
	}

	template <class T>
	T ToLocal(const std::pair<T, T>& val, const T& t) {
		T r;
		for (size_t i = 0; i < T::size; i++) {
			r[i] = (t[i] - val.first[i]) / (val.second[i] - val.first[i]);
		}

		return r;
	}

	template <class T>
	T FromLocal(const std::pair<T, T>& val, const T& t) {
		T r;
		for (size_t i = 0; i < T::size; i++) {
			r[i] = (val.second[i] - val.first[i]) * t[i] + val.first[i];
		}

		return r;
	}

	template <class T, class D>
	std::pair<T, D> operator * (const std::pair<T, D>& lhs, double t) {
		return std::pair<T, D>(lhs.first * t, lhs.second * t);
	}

	template <class T, class D>
	std::pair<T, D> operator / (const std::pair<T, D>& lhs, double t) {
		return std::pair<T, D>(lhs.first / t, lhs.second / t);
	}
}


#endif // __TVECTOR_H__
