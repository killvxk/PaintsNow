// RandomQuery.h
// By PaintDream (paintdream@paintdream.com)
// 2018-2-10
//

#ifndef __RANDOM_QUERY_H__
#define __RANDOM_QUERY_H__

namespace PaintsNow {
	namespace NsLostDream {
		class RandomQuery : public LostDream::Qualifier {
		public:
			virtual bool Initialize();
			virtual bool Run(int randomSeed, int length);
			virtual void Summary();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}

#endif // __RANDOM_QUERY_H__