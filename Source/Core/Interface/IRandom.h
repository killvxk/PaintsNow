// INetwork.h
// By PaintDream (paintdream@paintdream.com)
// 2016-5-16
//

#ifndef __IRANDOM_H__
#define __IRANDOM_H__

#include "../PaintsNow.h"
#include "../Template/TProxy.h"
#include "../Common/ZType.h"
#include "IDevice.h"

namespace PaintsNow {
	class IRandom : public IDevice {
	public:
		virtual ~IRandom();
		virtual void Seed(long seed) = 0;
		virtual void SetConfig(const String& parameter, double value) = 0;
		virtual double GetConfig(const String& parameter) const = 0;
		virtual double GetValue(const double* coords, size_t dimension) = 0;

		template <class T>
		typename T::type GetValue(const T& input) {
			double value[T::size];
			for (size_t i = 0; i < T::size; i++) {
				value[i] = (double)input[i];
			}

			return (typename T::type)GetValue(value, T::size);
		}

	};
}

#endif // __IRANDOM_H__
