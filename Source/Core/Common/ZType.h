// ZType.h -- Basic type instances
// By PaintDream (paintdream@paintdream.com)
// 2014-11-30
//

#ifndef __ZTYPE_H__
#define __ZTYPE_H__

#ifdef _MSC_VER
#pragma warning (disable:4786)
#pragma warning (disable:4503)
#endif // _MSC_VER

#include "../Template/TVector.h"
#include "../Template/TMatrix.h"
#include <string>
#include <vector>
#include <stdexcept>

// #define UNIFY_STRING_LAYOUT

#ifndef __STD_TYPES__
#define __STD_TYPES__
#ifdef _MSC_VER
#if (_MSC_VER <= 1200)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif
#else
#include <stdint.h>
// typedef unsigned long long uint64_t;
#endif
#endif


#if defined(_MSC_VER) && _MSC_VER <= 1200
/*
* Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
*/

/*
* This file is derived from software bearing the following
* restrictions:
*
* Copyright (c) 1994
* Hewlett-Packard Company
*
* Permission to use, copy, modify, distribute and sell this
* software and its documentation for any purpose is hereby
* granted without fee, provided that the above copyright notice
* appear in all copies and that both that copyright notice and
* this permission notice appear in supporting documentation.
* Hewlett-Packard Company makes no representations about the
* suitability of this software for any purpose. It is provided
* "as is" without express or implied warranty.
*/


#include <climits>
#include <memory>
#include <stdexcept>
#include <xutility>
#else
#include <vector>
#endif

namespace PaintsNow {
#if defined(_MSC_VER) && _MSC_VER <= 1200
	// shrink from 16 bytes to 12 bytes on VC6
	template<class _Ty, class _A = std::allocator<_Ty> >
	class Vector {
	public:
		typedef Vector<_Ty, _A> _Myt;
		typedef _A allocator_type;
		typedef typename _A::size_type size_type;
		typedef typename _A::difference_type difference_type;
		typedef typename _A::pointer _Tptr;
		typedef typename _A::const_pointer _Ctptr;
		typedef typename _A::reference reference;
		typedef typename _A::const_reference const_reference;
		typedef typename _A::value_type value_type;
		typedef _Tptr iterator;
		typedef _Ctptr const_iterator;
		
		explicit Vector()
			: _First(0), _Last(0), _End(0) {}
		explicit Vector(size_type _N, const _Ty& _V = _Ty())
			:
		{_First = allocator_type().allocate(_N, (void *)0);
		_Ufill(_First, _N, _V);
		_Last = _First + _N;
		_End = _Last; }
		Vector(const _Myt& _X)
		{_First = allocator_type().allocate(_X.size(), (void *)0);
		_Last = _Ucopy(_X.begin(), _X.end(), _First);
		_End = _Last; }
		typedef const_iterator _It;
		Vector(_It _F, _It _L)
			: _First(0), _Last(0), _End(0) {
			insert(begin(), _F, _L);
		}
		~Vector() {
			_Destroy(_First, _Last);
			allocator_type().deallocate(_First, _End - _First);
			_First = 0, _Last = 0, _End = 0;
		}
		_Myt& operator=(const _Myt& _X) {
			if (this == &_X)
				;
			else if (_X.size() <= size()) {
				iterator _S = std::copy(_X.begin(), _X.end(), _First);
				_Destroy(_S, _Last);
				_Last = _First + _X.size();
			} else if (_X.size() <= capacity()) {
				const_iterator _S = _X.begin() + size();
				std::copy(_X.begin(), _S, _First);
				_Ucopy(_S, _X.end(), _Last);
				_Last = _First + _X.size();
			} else {
				_Destroy(_First, _Last);
				allocator_type().deallocate(_First, _End - _First);
				_First = allocator_type().allocate(_X.size(), (void *)0);
				_Last = _Ucopy(_X.begin(), _X.end(),
					_First);
				_End = _Last;
			}
			return (*this);
		}
		void reserve(size_type _N) {
			if (capacity() < _N) {
				iterator _S = allocator_type().allocate(_N, (void *)0);
				_Ucopy(_First, _Last, _S);
				_Destroy(_First, _Last);
				allocator_type().deallocate(_First, _End - _First);
				_End = _S + _N;
				_Last = _S + size();
				_First = _S;
			}
		}
		size_type capacity() const {
			return (_First == 0 ? 0 : _End - _First);
		}
		iterator begin() {
			return (_First);
		}
		const_iterator begin() const {
			return ((const_iterator)_First);
		}
		iterator end() {
			return (_Last);
		}
		const_iterator end() const {
			return ((const_iterator)_Last);
		}
		void resize(size_type _N, const _Ty& _X = _Ty()) {
			if (size() < _N)
				insert(end(), _N - size(), _X);
			else if (_N < size())
				erase(begin() + _N, end());
		}
		size_type size() const {
			return (_First == 0 ? 0 : _Last - _First);
		}
		size_type max_size() const {
			return (allocator_type().max_size());
		}
		bool empty() const {
			return (size() == 0);
		}
		_A get_allocator() const {
			return (allocator);
		}
		const_reference at(size_type _P) const {
			if (size() <= _P)
				_Xran();
			return (*(begin() + _P));
		}
		reference at(size_type _P) {
			if (size() <= _P)
				_Xran();
			return (*(begin() + _P));
		}
		const_reference operator[](size_type _P) const {
			return (*(begin() + _P));
		}
		reference operator[](size_type _P) {
			return (*(begin() + _P));
		}
		reference front() {
			return (*begin());
		}
		const_reference front() const {
			return (*begin());
		}
		reference back() {
			return (*(end() - 1));
		}
		const_reference back() const {
			return (*(end() - 1));
		}
		void push_back(const _Ty& _X) {
			insert(end(), _X);
		}
		void pop_back() {
			erase(end() - 1);
		}
		void assign(_It _F, _It _L) {
			erase(begin(), end());
			insert(begin(), _F, _L);
		}
		void assign(size_type _N, const _Ty& _X = _Ty()) {
			erase(begin(), end());
			insert(begin(), _N, _X);
		}
		iterator insert(iterator _P, const _Ty& _X = _Ty()) {
			size_type _O = _P - begin();
			insert(_P, 1, _X);
			return (begin() + _O);
		}
		void insert(iterator _P, size_type _M, const _Ty& _X) {
			if (_End - _Last < _M) {
				size_type _N = size() + (_M < size() ? size() : _M);
				iterator _S = allocator_type().allocate(_N, (void *)0);
				iterator _Q = _Ucopy(_First, _P, _S);
				_Ufill(_Q, _M, _X);
				_Ucopy(_P, _Last, _Q + _M);
				_Destroy(_First, _Last);
				allocator_type().deallocate(_First, _End - _First);
				_End = _S + _N;
				_Last = _S + size() + _M;
				_First = _S;
			} else if (_Last - _P < _M) {
				_Ucopy(_P, _Last, _P + _M);
				_Ufill(_Last, _M - (_Last - _P), _X);
				std::fill(_P, _Last, _X);
				_Last += _M;
			} else if (0 < _M) {
				_Ucopy(_Last - _M, _Last, _Last);
				std::copy_backward(_P, _Last - _M, _Last);
				std::fill(_P, _P + _M, _X);
				_Last += _M;
			}
		}
		void insert(iterator _P, _It _F, _It _L) {
			size_type _M = 0;
			_Distance(_F, _L, _M);
			if (_End - _Last < _M) {
				size_type _N = size() + (_M < size() ? size() : _M);
				iterator _S = allocator_type().allocate(_N, (void *)0);
				iterator _Q = _Ucopy(_First, _P, _S);
				_Q = _Ucopy(_F, _L, _Q);
				_Ucopy(_P, _Last, _Q);
				_Destroy(_First, _Last);
				allocator_type().deallocate(_First, _End - _First);
				_End = _S + _N;
				_Last = _S + size() + _M;
				_First = _S;
			} else if (_Last - _P < _M) {
				_Ucopy(_P, _Last, _P + _M);
				_Ucopy(_F + (_Last - _P), _L, _Last);
				copy(_F, _F + (_Last - _P), _P);
				_Last += _M;
			} else if (0 < _M) {
				_Ucopy(_Last - _M, _Last, _Last);
				copy_backward(_P, _Last - _M, _Last);
				copy(_F, _L, _P);
				_Last += _M;
			}
		}
		iterator erase(iterator _P) {
			std::copy(_P + 1, end(), _P);
			_Destroy(_Last - 1, _Last);
			--_Last;
			return (_P);
		}
		iterator erase(iterator _F, iterator _L) {
			iterator _S = std::copy(_L, end(), _F);
			_Destroy(_S, end());
			_Last = _S;
			return (_F);
		}
		void clear() {
			erase(begin(), end());
		}
		bool _Eq(const _Myt& _X) const {
			return (size() == _X.size()
				&& equal(begin(), end(), _X.begin()));
		}
		bool _Lt(const _Myt& _X) const {
			return (lexicographical_compare(begin(), end(),
				_X.begin(), _X.end()));
		}
		void swap(_Myt& _X) {
			if (allocator == _X.allocator) {
				std::swap(_First, _X._First);
				std::swap(_Last, _X._Last);
				std::swap(_End, _X._End);
			} else {
				_Myt _Ts = *this; *this = _X, _X = _Ts;
			}
		}
		friend void swap(_Myt& _X, _Myt& _Y) {
			_X.swap(_Y);
		}
	protected:
		void _Destroy(iterator _F, iterator _L) {
			for (; _F != _L; ++_F)
				allocator_type().destroy(_F);
		}
		iterator _Ucopy(const_iterator _F, const_iterator _L,
			iterator _P) {
			for (; _F != _L; ++_P, ++_F)
				allocator_type().construct(_P, *_F);
			return (_P);
		}
		void _Ufill(iterator _F, size_type _N, const _Ty &_X) {
			for (; 0 < _N; --_N, ++_F)
				allocator_type().construct(_F, _X);
		}
		void _Xran() const {
			_THROW(out_of_range, "invalid Vector<T> subscript");
		}
		iterator _First, _Last, _End;
	};

	// Vector TEMPLATE OPERATORS
	template<class _Ty, class _A> inline
		bool operator==(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (_X._Eq(_Y));
	}
	template<class _Ty, class _A> inline
		bool operator!=(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (!(_X == _Y));
	}
	template<class _Ty, class _A> inline
		bool operator<(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (_X._Lt(_Y));
	}
	template<class _Ty, class _A> inline
		bool operator>(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (_Y < _X);
	}
	template<class _Ty, class _A> inline
		bool operator<=(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (!(_Y < _X));
	}
	template<class _Ty, class _A> inline
		bool operator>=(const Vector<_Ty, _A>& _X,
			const Vector<_Ty, _A>& _Y) {
		return (!(_X < _Y));
	}
#else
	template <class T, class Alloc = std::allocator<T> >
	using Vector = std::vector<T, Alloc>;
#endif

#ifdef UNIFY_STRING_LAYOUT
	class SafeCharAlloc {
	public:
		typedef size_t size_type;
		typedef long difference_type;
		typedef char *pointer;
		typedef const char* const_pointer;
		typedef char& reference;
		typedef const char& const_reference;
		typedef char value_type;
		virtual ~SafeCharAlloc() {}
		virtual pointer address(reference _X) const { return (&_X); }
		virtual const_pointer address(const_reference _X) const {
			return (&_X);
		}
		virtual pointer allocate(size_type _N, const void *) {
			return ::new char[_N];
		}
		virtual void deallocate(void *_P, size_type) {
 delete[] (char*)(_P); }
	void construct(pointer _P, const char& _V)
		{ }
	void destroy(pointer _P) {}
	size_t max_size() const
		{ size_t _N = (size_t)(-1) / sizeof (char);
		return (0 < _N ? _N : 1); }
	bool operator == (const SafeCharAlloc& rhs) const {
		return mark() == rhs.mark();
	}
	virtual void* mark() const {
		return staticmark();
	}
	static void* staticmark() {
		static void* mc;
		return (void*)&mc;
	}
	};

	// From Microsoft Visual C++ Header file.;
	class String {
	public:
		typedef char _E;
		typedef std::char_traits<char> _Tr;
		typedef SafeCharAlloc _A;
		typedef String _Myt;
		typedef _A::size_type size_type;
		typedef _A::difference_type difference_type;
		typedef _A::pointer pointer;
		typedef _A::const_pointer const_pointer;
		typedef _A::reference reference;
		typedef _A::const_reference const_reference;
		typedef _A::value_type value_type;
		typedef _A::pointer iterator;
		typedef _A::const_pointer const_iterator;

		explicit String(const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy();
		}

		String(const std::string& input) { _Tidy();  assign(input.c_str(), input.length()); }
		String& operator = (const std::string& input) {
			assign(input.c_str(), input.length());
			return *this;
		}

		void _Xlen() const {	// report a length_error
			throw std::overflow_error("string too long");
		}

		void _Xran() const {	// report an out_of_range error
			throw std::range_error("invalid string position");
		}
		String(const _Myt& _X)
			: allocator(_X.allocator) {
			_Tidy(), assign(_X, 0, npos);
		}
		String(const _Myt& _X, size_type _P, size_type _M,
			const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy(), assign(_X, _P, _M);
		}
		String(const _E *_S, size_type _N,
			const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy(), assign(_S, _N);
		}
		String(const _E *_S, const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy(), assign(_S);
		}
		String(size_type _N, _E _C, const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy(), assign(_N, _C);
		}
		typedef const_iterator _It;
		String(_It _F, _It _L, const _A& _Al = _A())
			: allocator(_Al) {
			_Tidy(); assign(_F, _L);
		}
		~String(){
_Tidy(true); }
	typedef _Tr traits_type;
	typedef _A allocator_type;
	enum _Mref {_FROZEN = 255};
	static const size_type npos;
	bool operator==(const _Myt& _X) const {
		return compare(_X) == 0;
	}
	bool operator < (const _Myt& _X) const {
		return compare(_X) < 0;
	}
	_Myt& operator=(const _Myt& _X)
		{return (assign(_X)); }
	_Myt& operator=(const _E *_S)
		{return (assign(_S)); }
	_Myt& operator=(_E _C)
		{return (assign(1, _C)); }
	_Myt& operator+=(const _Myt& _X)
		{return (append(_X)); }

	_Myt& operator+=(const std::string& _X)  {
		return (append(_X.c_str(), _X.length())); }
	_Myt& operator+=(const _E *_S)
		{return (append(_S)); }
	_Myt& operator+=(_E _C)
		{return (append(1, _C)); }
	_Myt operator + (const _Myt& rhs) const {
		return _Myt(*this).append(rhs);
	}
	/*
	_Myt operator + (const std::string& _X) const {
		return _Myt(*this).append(_X.c_str(), _X.length());
	}*/
	_Myt operator + (const _E& rhs) const {
		return _Myt(*this).append(1, rhs);
	}
	bool operator !=(const _Myt& _X) const {
		return compare(_X) != 0;
	}
	_Myt& append(const _Myt& _X)
		{return (append(_X, 0, npos)); }
	_Myt& append(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S, size_type _M)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S)
		{return (append(_S, _Tr::length(_S))); }
	_Myt& append(size_type _M, _E _C)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::assign(_Ptr + _Len, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& append(_It _F, _It _L)
		{return (replace(end(), end(), _F, _L)); }
	_Myt& assign(const _Myt& _X)
		{return (assign(_X, 0, npos)); }
	_Myt& assign(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_M < _N)
			_N = _M;
		if (this == &_X)
			erase((size_type)(_P + _N)), erase(0, _P);
		else if (0 < _N && _N == _X.size()
			&& _Refcnt(_X.c_str()) < _FROZEN - 1
			&& allocator == _X.allocator)
			{_Tidy(true);
			_Ptr = (_E *)_X.c_str();
			_Len = _X.size();
			_Res = _X.capacity();
			++_Refcnt(_Ptr); }
		else if (_Grow(_N, true))
			{_Tr::copy(_Ptr, &_X.c_str()[_P], _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S, size_type _N)
		{if (_Grow(_N, true))
			{_Tr::copy(_Ptr, _S, _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S)
		{return (assign(_S, _Tr::length(_S))); }
	_Myt& assign(size_type _N, _E _C)
		{if (_N == npos)
			_Xlen();
		if (_Grow(_N, true))
			{_Tr::assign(_Ptr, _N, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(_It _F, _It _L)
		{return (replace(begin(), end(), _F, _L)); }
	_Myt& insert(size_type _P0, const _Myt& _X)
		{return (insert(_P0, _X, 0, npos)); }
	_Myt& insert(size_type _P0, const _Myt& _X, size_type _P,
		size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S, size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S)
		{return (insert(_P0, _S, _Tr::length(_S))); }
	_Myt& insert(size_type _P0, size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	iterator insert(iterator _P, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, 1, _C);
		return (begin() + _P0); }
	void insert(iterator _P, size_type _M, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, _M, _C); }
	void insert(iterator _P, _It _F, _It _L)
		{replace(_P, _P, _F, _L); }
	_Myt& erase(size_type _P0 = 0, size_type _M = npos)
		{if (_Len < _P0)
			_Xran();
		_Split();
		if (_Len - _P0 < _M)
			_M = _Len - _P0;
		if (0 < _M)
			{_Tr::move(_Ptr + _P0, _Ptr + _P0 + _M,
				_Len - _P0 - _M);
			size_type _N = _Len - _M;
			if (_Grow(_N))
				_Eos(_N); }
		return (*this); }
	iterator erase(iterator _P)
		{size_t _M = _Pdif(_P, begin());
		erase(_M, 1);
		return (_Psum(_Ptr, _M)); }
	iterator erase(iterator _F, iterator _L)
		{size_t _M = _Pdif(_F, begin());
		erase(_M, _Pdif(_L, _F));
		return (_Psum(_Ptr, _M)); }
	_Myt& replace(iterator _F1, iterator _L1,
		_It _F2, _It _L2)	{
		size_type _P0 = _Pdif(_F1, begin());
		size_type _M = _L2 - _F2;
		replace(_P0, _Pdif(_L1, _F1), _M, _E(0));
		for (_F1 = begin() + _P0; 0 < _M; ++_F1, ++_F2, --_M)
			*_F1 = *_F2;
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X)
		{return (replace(_P0, _N0, _X, 0, npos)); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S,
		size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S)
		{return (replace(_P0, _N0, _S, _Tr::length(_S))); }
	_Myt& replace(size_type _P0, size_type _N0,
		size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0,
					_Nm);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(iterator _F, iterator _L, const _Myt& _X)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _X)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S,
		size_type _M)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S, _M)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S)); }
	_Myt& replace(iterator _F, iterator _L,	size_type _M, _E _C)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _M, _C)); }
	iterator begin()
		{_Freeze();
		return (_Ptr); }
	const_iterator begin() const
		{return (_Ptr); }
	iterator end()
		{_Freeze();
		return ((iterator)_Psum(_Ptr, _Len)); }
	const_iterator end() const
		{return ((const_iterator)_Psum(_Ptr, _Len)); }
	reference at(size_type _P0)
		{if (_Len <= _P0)
			_Xran();
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference at(size_type _P0) const
		{if (_Len <= _P0)
			_Xran();
		return (_Ptr[_P0]); }
	reference operator[](size_type _P0)
		{if (_Len < _P0 || _Ptr == 0)
			return ((reference)*_nullptrstr());
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference operator[](size_type _P0) const
		{if (_Ptr == 0)
			return (*_nullptrstr());
		else
			return (_Ptr[_P0]); }
	const _E *c_str() const
		{return (_Ptr == 0 ? _nullptrstr() : _Ptr); }
	const _E *data() const
		{return (c_str()); }
	size_type length() const
		{return (_Len); }
	size_type size() const
		{return (_Len); }
	size_type max_size() const
		{size_type _N = allocator.max_size();
		return (_N <= 2 ? 1 : _N - 2); }
	void resize(size_type _N, _E _C)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _C); }
	void resize(size_type _N)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _E(0)); }
	size_type capacity() const
		{return (_Res); }
	void reserve(size_type _N = 0)
		{if (_Res < _N)
			_Grow(_N); }
	bool empty() const
		{return (_Len == 0); }
	size_type copy(_E *_S, size_type _N, size_type _P0 = 0) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N)
			_N = _Len - _P0;
		if (0 < _N)
			_Tr::copy(_S, _Ptr + _P0, _N);
		return (_N); }
	void swap(_Myt& _X)
		{if (allocator == _X.allocator)
			{std::swap(_Ptr, _X._Ptr);
			std::swap(_Len, _X._Len);
			std::swap(_Res, _X._Res); }
		else
			{_Myt _Ts = *this; *this = _X, _X = _Ts; }}
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	size_type find(const _Myt& _X, size_type _P = 0) const
		{return (find(_X.c_str(), _P, _X.size())); }
	size_type find(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0 && _P <= _Len)
			return (_P);
		size_type _Nm;
		if (_P < _Len && _N <= (_Nm = _Len - _P))
			{const _E *_U, *_V;
			for (_Nm -= _N - 1, _V = _Ptr + _P;
				(_U = _Tr::find(_V, _Nm, *_S)) != 0;
				_Nm -= _U - _V + 1, _V = _U + 1)
				if (_Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find(const _E *_S, size_type _P = 0) const
		{return (find(_S, _P, _Tr::length(_S))); }
	size_type find(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type rfind(const _Myt& _X, size_type _P = npos) const
		{return (rfind(_X.c_str(), _P, _X.size())); }
	size_type rfind(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0)
			return (_P < _Len ? _P : _Len);
		if (_N <= _Len)
			for (const _E *_U = _Ptr +
				+ (_P < _Len - _N ? _P : _Len - _N); ; --_U)
				if (_Tr::eq(*_U, *_S)
					&& _Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type rfind(const _E *_S, size_type _P = npos) const
		{return (rfind(_S, _P, _Tr::length(_S))); }
	size_type rfind(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_of(_X.c_str(), _P, _X.size())); }
	size_type find_first_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && _P < _Len)
			{const _E *const _V = _Ptr + _Len;
			for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_of(const _E *_S, size_type _P = 0) const
		{return (find_first_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_of(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type find_last_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && 0 < _Len)
			for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_of(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_not_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_not_of(_X.c_str(), _P,
			_X.size())); }
	size_type find_first_not_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (_P < _Len)
			{const _E *const _V = _Ptr + _Len;
			for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_not_of(const _E *_S,
		size_type _P = 0) const
		{return (find_first_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_not_of(_E _C, size_type _P = 0) const
		{return (find_first_not_of((const _E *)&_C, _P, 1)); }
	size_type find_last_not_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_not_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_not_of(const _E *_S, size_type _P,
		 size_type _N) const
		{if (0 < _Len)
			for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_not_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_not_of(_E _C, size_type _P = npos) const
		{return (find_last_not_of((const _E *)&_C, _P, 1)); }
	_Myt substr(size_type _P = 0, size_type _M = npos) const
		{return (_Myt(*this, _P, _M)); }
	int compare(const _Myt& _X) const
		{return (compare(0, _Len, _X.c_str(), _X.size())); }
	int compare(size_type _P0, size_type _N0,
		const _Myt& _X) const
		{return (compare(_P0, _N0, _X, 0, npos)); }
	int compare(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M) const
		{if (_X.size() < _P)
			_Xran();
		if (_X._Len - _P < _M)
			_M = _X._Len - _P;
		return (compare(_P0, _N0, _X.c_str() + _P, _M)); }
	int compare(const _E *_S) const
		{return (compare(0, _Len, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S) const
		{return (compare(_P0, _N0, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S,
		size_type _M) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _Ans = _Tr::compare(_Psum(_Ptr, _P0), _S,
			_N0 < _M ? _N0 : _M);
		return (int)(_Ans != 0 ? _Ans : _N0 < _M ? -1
			: _N0 == _M ? 0 : +1); }
	_A get_allocator() const
		{return (allocator); }
protected:
	_A allocator;
private:
	enum {_MIN_SIZE = sizeof (_E) <= 32 ? 31 : 7};
	void _Copy(size_type _N)
		{size_type _Ns = _N | _MIN_SIZE;
	
		if (max_size() < _Ns)
			_Ns = _N;
		_E *_S;
		try {
			_S = allocator.allocate(_Ns + 2, (void *)0);
		} catch (...) {
			_Ns = _N;
			_S = allocator.allocate(_Ns + 2, (void *)0);
		}
		if (0 < _Len)
			_Tr::copy(_S + 1, _Ptr,_Len>_Ns?_Ns:_Len);
		size_type _Olen = _Len;
		_Tidy(true);
		_Ptr = _S + 1;
		_Refcnt(_Ptr) = 0;
		_Res = _Ns;
		_Eos(_Olen>_Ns?_Ns:_Olen); }
	void _Eos(size_type _N)
		{_Tr::assign(_Ptr[_Len = _N], _E(0)); }
	void _Freeze()
		{if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			_Grow(_Len);
		if (_Ptr != 0)
			_Refcnt(_Ptr) = _FROZEN; }
	bool _Grow(size_type _N, bool _Trim = false)
		{if (max_size() < _N)
			_Xlen();
		if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN) {
			if (_N == 0)
				{--_Refcnt(_Ptr), _Tidy();
				return (false); }
			else
				{_Copy(_N);
				return (true); }
				}
		if (_N == 0)
			{if (_Trim)
				_Tidy(true);
			else if (_Ptr != 0)
				_Eos(0);
			return (false); }
		else
			{if (_Trim && (_MIN_SIZE < _Res || _Res < _N))
				{_Tidy(true);
				_Copy(_N); }
			else if (!_Trim && _Res < _N)
				_Copy(_N);
			return (true); }}
	static const _E * _nullptrstr()
		{static const _E _C = _E(0);
		return (&_C); }
	static size_type _Pdif(const_pointer _P2, const_pointer _P1)
		{return (_P2 == 0 ? 0 : _P2 - _P1); }
	static const_pointer _Psum(const_pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	static pointer _Psum(pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	unsigned char& _Refcnt(const _E *_U)
		{return (((unsigned char *)_U)[-1]); }
	void _Split()
		{if (_Ptr != 0 && _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			{_E *_Temp = _Ptr;
			_Tidy(true);
			assign(_Temp); }}
	void _Tidy(bool _Built = false)
		{if (!_Built || _Ptr == 0)
			;
		else if (_Refcnt(_Ptr) == 0 || _Refcnt(_Ptr) == _FROZEN)
			allocator.deallocate(_Ptr - 1, _Res + 2);
		else
			--_Refcnt(_Ptr);
		_Ptr = 0, _Len = 0, _Res = 0; }

	_E *_Ptr;
	size_type _Len, _Res;
#if !defined(_MSC_VER) || _MSC_VER > 1200
	public:
	String(String&& str) {
		_Tidy();
		*this = std::move(str);
	}

	String& operator = (String&& str) {
		this->swap(str);
		return *this;
	}
#endif
	};
#else
	typedef std::string String;
#endif
}

namespace PaintsNow {
	uint32_t HashBuffer(const void* buffer, size_t length);

	inline const bool IsStdEndian() {
		static const int a = 0x01;
		return *(const char*)&a == 0x01;
	}

	template <class T>
	T Endianize(const T& src) {
		return IsStdEndian() ? src : ReverseBytes(src);
	}
}


#if defined(UNIFY_STRING_LAYOUT) && (!defined(_MSC_VER) || _MSC_VER > 1200)
namespace std {
	template <>
	struct hash<PaintsNow::String> {
		size_t operator () (const PaintsNow::String& _Keyval) const {	// hash _Keyval to size_t value by pseudorandomizing transform
			return PaintsNow::HashBuffer(_Keyval.data(), _Keyval.length());
		}
	};
}
#endif

namespace std {
	inline void swap(PaintsNow::String& a, PaintsNow::String& b) {
		a.swap(b);
	}
}


namespace PaintsNow {
	typedef TType2<char> ZChar2;
	typedef TType3<char> ZChar3;
	typedef TType4<char> ZChar4;
	typedef TType2<unsigned char> ZUChar2;
	typedef TType3<unsigned char> ZUChar3;
	typedef TType4<unsigned char> ZUChar4;
	typedef TType2<short> ZShort2;
	typedef TType3<short> ZShort3;
	typedef TType4<short> ZShort4;
	typedef TType2<unsigned short> ZUShort2;
	typedef TType3<unsigned short> ZUShort3;
	typedef TType4<unsigned short> ZUShort4;
	typedef TType2<int> ZInt2;
	typedef TType3<int> ZInt3;
	typedef TType4<int> ZInt4;
	typedef TType2<unsigned int> ZUInt2;
	typedef TType3<unsigned int> ZUInt3;
	typedef TType4<unsigned int> ZUInt4;
	typedef TType2<float> ZFloat2;
	typedef TType3<float> ZFloat3;
	typedef TType4<float> ZFloat4;
	typedef TType2<double> ZDouble2;
	typedef TType3<double> ZDouble3;
	typedef TType4<double> ZDouble4;
	typedef std::pair<ZFloat4, ZFloat4> ZFloat4Pair;
	typedef std::pair<ZFloat3, ZFloat3> ZFloat3Pair;
	typedef std::pair<ZFloat2, ZFloat2> ZFloat2Pair;
	typedef std::pair<ZDouble4, ZDouble4> ZDouble4Pair;
	typedef std::pair<ZDouble3, ZDouble3> ZDouble3Pair;
	typedef std::pair<ZDouble2, ZDouble2> ZDouble2Pair;
	typedef std::pair<ZInt4, ZInt4> ZInt4Pair;
	typedef std::pair<ZInt3, ZInt3> ZInt3Pair;
	typedef std::pair<ZInt2, ZInt2> ZInt2Pair;
	typedef std::pair<ZUInt4, ZUInt4> ZUInt4Pair;
	typedef std::pair<ZUInt3, ZUInt3> ZUInt3Pair;
	typedef std::pair<ZUInt2, ZUInt2> ZUInt2Pair;
	typedef std::pair<ZShort4, ZShort4> ZShort4Pair;
	typedef std::pair<ZShort3, ZShort3> ZShort3Pair;
	typedef std::pair<ZShort2, ZShort2> ZShort2Pair;
	typedef std::pair<ZUShort4, ZUShort4> ZUShort4Pair;
	typedef std::pair<ZUShort3, ZUShort3> ZUShort3Pair;
	typedef std::pair<ZUShort2, ZUShort2> ZUShort2Pair;
	typedef TMatrix<float, 4U, 4U> ZMatrixFloat4x4;
	typedef TMatrix<float, 3U, 4U> ZMatrixFloat3x4;
	typedef TMatrix<float, 3U, 3U> ZMatrixFloat3x3;
	typedef TMatrix<int, 3U, 3U> ZMatrixInt3x3;

	ZMatrixFloat4x4 Rotate3D(const ZMatrixFloat4x4& input, float degree, const ZFloat3& d);
	ZMatrixFloat4x4 Translate3D(const ZMatrixFloat4x4& input, const ZFloat3& v);

	bool Capture3D(const ZMatrixFloat4x4& matrix, const ZFloat3Pair& vec, const ZFloat2Pair& size);
	String Utf8ToSystem(const String& str);
	String SystemToUtf8(const String& str);
	bool Intersect3D(ZFloat3& res, ZFloat2& uv, const ZFloat3 face[3], const ZFloat3Pair& line);

	template <class T>
	class Quaternion : public TType4<T> {
	public:
		Quaternion(T ww = 1, T xx = 0, T yy = 0, T zz = 0) : ZFloat4(xx, yy, zz, ww) {}
		static T Distance(const Quaternion& lhs, const Quaternion& rhs) {
			Quaternion conj = rhs;
			conj.Conjugate();
			return (lhs * conj).w();
		}

		Quaternion(const TMatrix<T, 4, 4>& m) { // from OGRE
			T s;
			T tq[4];
			int i, j;

			// Use tq to store the largest trace
			tq[0] = 1 + m(0,0) + m(1,1) + m(2,2);
			tq[1] = 1 + m(0,0) - m(1,1) - m(2,2);
			tq[2] = 1 - m(0,0) + m(1,1) - m(2,2);
			tq[3] = 1 - m(0,0) - m(1,1) + m(2,2);

			// Find the maximum (could also use stacked if's later)
			j = 0;
			for (i = 1; i < 4; i++) j = (tq[i] > tq[j]) ? i : j;

			// check the diagonal
			if (j == 0) {
				/* perform instant calculation */
				this->w() = tq[0];
				this->x() = m(1,2) - m(2,1);
				this->y() = m(2,0) - m(0,2);
				this->z() = m(0,1) - m(1,0);
			} else if (j == 1) {
				this->w() = m(1,2) - m(2,1);
				this->x() = tq[1];
				this->y() = m(0,1) + m(1,0);
				this->z() = m(2,0) + m(0,2);
			} else if (j == 2) {
				this->w() = m(2,0) - m(0,2);
				this->x() = m(0,1) + m(1,0);
				this->y() = tq[2];
				this->z() = m(1,2) + m(2,1);
			} else {
				this->w() = m(0,1) - m(1,0);
				this->x() = m(2,0) + m(0,2);
				this->y() = m(1,2) + m(2,1);
				this->z() = tq[3];
			}

			s = sqrt((T)0.25 / tq[j]);
			this->w() *= s;
			this->x() *= s;
			this->y() *= s;
			this->z() *= s;
		}

		static Quaternion Flip() {
			return Quaternion(0, 0, 0, 0);
		}

		static Quaternion Align(const ZFloat3& from, const ZFloat3& to) {
			const T EPSILON = (T)1e-3;
			TType3<T> axis = CrossProduct(to, from);
			T pcos = DotProduct(from, to);
			T halfcos = (T)sqrt(0.5 + pcos * 0.5);
			T ratio = halfcos > EPSILON ? (T)(0.5 / halfcos) : 0;

			return Quaternion(halfcos, axis.x() * ratio, axis.y() * ratio, axis.z() * ratio);
		}

		TType3<T> ToEulerAngle() const {
			T xx = (T)atan2(2 * (this->w() * this->x() + this->y() * this->z()), 1 - 2.0 * (this->x() * this->x() + this->y() * this->y()));
			T yy = (T)asin(2 * (this->w() * this->y() - this->z() * this->x()));
			T zz = (T)atan2(2 * (this->w() * this->z() + this->x() * this->y()), 1 - 2.0 * (this->y() * this->y() + this->z() * this->z()));

			return TType3<T>(xx, yy, zz);
		}

		Quaternion(const TType4<T>& quat) : ZFloat4(quat) {}
		Quaternion(const TType3<T>& rot) {
			const T fSinPitch((T)sin(rot.y() * 0.5));
			const T fCosPitch((T)cos(rot.y() * 0.5));
			const T fSinYaw((T)sin(rot.z() * 0.5));
			const T fCosYaw((T)cos(rot.z() * 0.5));
			const T fSinRoll((T)sin(rot.x() * 0.5));
			const T fCosRoll((T)cos(rot.x() * 0.5));
			const T fCosPitchCosYaw(fCosPitch * fCosYaw);
			const T fSinPitchSinYaw(fSinPitch * fSinYaw);

			this->x() = fSinRoll * fCosPitchCosYaw - fCosRoll * fSinPitchSinYaw;
			this->y() = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
			this->z() = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
			this->w() = fCosRoll * fCosPitchCosYaw + fSinRoll * fSinPitchSinYaw;

			Normalize();
		}

		Quaternion& Normalize() {
			T mag = sqrt(this->x() * this->x() + this->y() * this->y() + this->z() * this->z() + this->w() * this->w());
			if (mag > 1e-6) {
				mag = 1.0f / mag;
				this->x() *= mag;
				this->y() *= mag;
				this->z() *= mag;
				this->w() *= mag;
			}

			return *this;
		}

		bool IsFlip() const {
			return this->x() == 0 && this->y() == 0 && this->z() == 0 && this->w() == 0;
		}

		Quaternion operator * (const Quaternion& t) const {
			assert(!this->IsFlip() && !t.IsFlip());
			return Quaternion(this->w() * t.w() - this->x() * t.x() - this->y() * t.y() - this->z() * t.z(),
				this->w() * t.x() + this->x() * t.w() + this->y() * t.z() - this->z() * t.y(),
				this->w() * t.y() + this->y() * t.w() + this->z() * t.x() - this->x() * t.z(),
				this->w() * t.z() + this->z() * t.w() + this->x() * t.y() - this->y() * t.x());
		}

		Quaternion& Conjugate() {
			this->x() = -this->x();
			this->y() = -this->y();
			this->z() = -this->z();

			return *this;
		}

		void Transform(TType3<T>& v) const {
			v = (*this)(v);
		}

		TType3<T> operator () (const TType3<T>& v) const {
			if (IsFlip()) {
				return TType3<T>(-v.x(), -v.y(), -v.z());
			} else {
				Quaternion q2(0, v.x(), v.y(), v.z()), q = *this, qinv = *this;
				q.Conjugate();

				q = q * q2 * qinv;
				return TType3<T>(q.x(), q.y(), q.z());
			}
		}

		static void Interpolate(Quaternion& out, const Quaternion& start, const Quaternion& end, T factor) {
			assert(!start.IsFlip() && !end.IsFlip());
			T cosom = start.x() * end.x() + start.y() * end.y() + start.z() * end.z() + start.w() * end.w();

			Quaternion qend = end;
			if (cosom < 0) {
				cosom = -cosom;
				qend.x() = -qend.x();
				qend.y() = -qend.y();
				qend.z() = -qend.z();
				qend.w() = -qend.w();
			}

			T sclp, sclq;
			if ((T)1.0 - cosom > 1e-6) {
				T omega, sinom;
				omega = acos(cosom);
				sinom = sin(omega);
				sclp = sin((1.0f - factor) * omega) / sinom;
				sclq = sin(factor * omega) / sinom;
			} else {
				sclp = (T)1.0 - factor;
				sclq = factor;
			}

			out.x() = sclp * start.x() + sclq * qend.x();
			out.y() = sclp * start.y() + sclq * qend.y();
			out.z() = sclp * start.z() + sclq * qend.z();
			out.w() = sclp * start.w() + sclq * qend.w();
		}

		static void InterpolateSquad(Quaternion& out, const Quaternion& left, const Quaternion& outTan,  const Quaternion& right, const Quaternion& inTan, float factor) {
			T t = (T)(2.0 * factor * (1.0 - factor));
			Quaternion p, q;
			Interpolate(p, left, right, t);
			Interpolate(q, outTan, inTan, t);

			Interpolate(out, p, q, t);
		}

		void WriteMatrix(TMatrix<T, 4U, 4U>& m) const {
			if (!IsFlip()) {
				T mat[16];
				mat[0] = 1 - 2 * (this->y() * this->y() + this->z() * this->z());
				mat[1] = 2 * (this->x() * this->y() - this->z() * this->w());
				mat[2] = 2 * (this->x() * this->z() + this->y() * this->w());
				mat[3] = 0;
				mat[4] = 2 * (this->x() * this->y() + this->z() * this->w());
				mat[5] = 1 - 2 * (this->x() * this->x() + this->z() * this->z());
				mat[6] = 2 * (this->y() * this->z() - this->x() * this->w());
				mat[7] = 0;
				mat[8] = 2 * (this->x() * this->z() - this->y() * this->w());
				mat[9] = 2 * (this->y() * this->z() + this->x() * this->w());
				mat[10] = 1 - 2 * (this->x() * this->x() + this->y() * this->y());
				mat[11] = mat[12] = mat[13] = mat[14] = 0;
				mat[15] = 1;

				m = TMatrix<T, 4U, 4U>(mat);
			} else {
				T mat[16] = { -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1 };
				m = TMatrix<T, 4U, 4U>(mat);
			}
		}
	};

	// From: https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
	inline int Log2(uint32_t value) {
		static const int tab32[32] = {
			0,  9,  1, 10, 13, 21,  2, 29,
			11, 14, 16, 18, 22, 25,  3, 30,
			8, 12, 20, 28, 15, 17, 24,  7,
			19, 27, 23,  6, 26,  5,  4, 31 };
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
	}

	inline int Log2(uint64_t value) {
		const int tab64[64] = {
			63,  0, 58,  1, 59, 47, 53,  2,
			60, 39, 48, 27, 54, 33, 42,  3,
			61, 51, 37, 40, 49, 18, 28, 20,
			55, 30, 34, 11, 43, 14, 22,  4,
			62, 57, 46, 52, 38, 26, 32, 41,
			50, 36, 17, 19, 29, 10, 13, 21,
			56, 45, 25, 31, 35, 16,  9, 12,
			44, 24, 15,  8, 23,  7,  6,  5 };

		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value |= value >> 32;
		return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
	}

	std::vector<String> Split(const String& str, char sep = ' ');
}

#endif // __ZTYPE_H__
