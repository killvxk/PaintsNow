// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#if _MSC_VER <= 1200
#include <atlimpl.cpp>
#endif

#ifdef _MSC_VER
#if _MSC_VER > 1200
#ifdef _DEBUG
#pragma comment(lib, "../../Build/Windows/VS/Debug/PaintsNow.lib")
#else
#pragma comment(lib, "../../Build/Windows/VS/Release/PaintsNow.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../../Build/Windows/VC/PaintsNow/Debug/PaintsNow.lib")
#else
#pragma comment(lib, "../../Build/Windows/VC/PaintsNow/Release/PaintsNow.lib")
#endif
#endif
#endif // _MSC_VER

#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "iconv32D.lib")
#else
#pragma comment(lib, "iconv32.lib")
#endif

#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "pthreadD.lib")
#else
#pragma comment(lib, "pthread.lib")
#endif


#if 0 // _MSC_VER > 1200

extern "C" {

	/***
	*qsort.c - quicksort algorithm; qsort() library function for sorting arrays
	*       Copyright (c) Microsoft Corporation. All rights reserved.
	*
	*Purpose:
	*       To implement the qsort() routine for sorting arrays.
	*
	*****************************************************************************

	**/

#include <stdlib.h>
#include <search.h>

#pragma optimize("t", on)

	static void __cdecl shortsort(char *lo, char *hi, size_t width,
		int(__cdecl *comp)(const void *, const void *));
	static void __cdecl swap(char *p, char *q, size_t width);
#define CUTOFF 8 

	static void __cdecl shortsort(
		char *lo,
		char *hi,
		size_t width,
		int(__cdecl *comp)(const void *, const void *)
		) {
		char *p, *max;
		while (hi > lo) {
			max = lo;
			for (p = lo + width; p <= hi; p += width) {
				if (comp(p, max) > 0) {
					max = p;
				}
			}
			swap(max, hi, width);
			hi -= width;
		}
	}

	static void __cdecl swap(
		char *a,
		char *b,
		size_t width
		) {
		char tmp;
		if (a != b)
		while (width--) {
			tmp = *a;
			*a++ = *b;
			*b++ = tmp;
		}
	}


#define STKSIZ (8*sizeof(void*) - 2)

	void __cdecl qsort(
		void *base,
		size_t num,
		size_t width,
		int(__cdecl *comp)(const void *, const void *)
		) {
		char *lo, *hi;
		char *mid;
		char *loguy, *higuy;
		size_t size;
		char *lostk[STKSIZ], *histk[STKSIZ];
		int stkptr;
		if (num < 2 || width == 0)
			return;

		stkptr = 0;

		lo = (char*)base;
		hi = (char *)base + width * (num - 1);

	recurse:

		size = (hi - lo) / width + 1;
		if (size <= CUTOFF) {
			shortsort(lo, hi, width, comp);
		} else {
			mid = lo + (size / 2) * width;
			if (comp(lo, mid) > 0) {
				swap(lo, mid, width);
			}
			if (comp(lo, hi) > 0) {
				swap(lo, hi, width);
			}
			if (comp(mid, hi) > 0) {
				swap(mid, hi, width);
			}

			loguy = lo;
			higuy = hi;

			for (;;) {
				if (mid > loguy) {
					do {
						loguy += width;
					} while (loguy < mid && comp(loguy, mid) <= 0);
				}

				if (mid <= loguy) {
					do {
						loguy += width;
					} while (loguy <= hi && comp(loguy, mid) <= 0);
				}

				do {
					higuy -= width;
				} while (higuy > mid && comp(higuy, mid) > 0);


				if (higuy < loguy)
					break;

				swap(loguy, higuy, width);
				if (mid == higuy)
					mid = loguy;
			}


			higuy += width;
			if (mid < higuy) {
				do {
					higuy -= width;
				} while (higuy > mid && comp(higuy, mid) == 0);
			}
			if (mid >= higuy) {
				do {
					higuy -= width;
				} while (higuy > lo && comp(higuy, mid) == 0);
			}


			if (higuy - lo >= hi - loguy) {
				if (lo < higuy) {
					lostk[stkptr] = lo;
					histk[stkptr] = higuy;
					++stkptr;
				}                           /* save big recursion for later */

				if (loguy < hi) {
					lo = loguy;
					goto recurse;           /* do small recursion */
				}
			} else {
				if (loguy < hi) {
					lostk[stkptr] = loguy;
					histk[stkptr] = hi;
					++stkptr;               /* save big recursion for later */
				}

				if (lo < higuy) {
					hi = higuy;
					goto recurse;           /* do small recursion */
				}
			}
		}

		/* We have sorted the array, except for any pending sorts on the stack.
		Check if there are any, and do them. */
		--stkptr;
		if (stkptr >= 0) {
			lo = lostk[stkptr];
			hi = histk[stkptr];
			goto recurse;           /* pop subarray from stack */
		} else
			return;                 /* all subarrays done */
	}

}

#endif