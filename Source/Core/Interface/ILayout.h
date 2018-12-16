// ILayout.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-21
//

#ifndef __ILAYOUT_H__
#define __ILAYOUT_H__

#include "../Common/ZType.h"
#include <list>

namespace PaintsNow {
	class ILayout {
	public:
		class Target {
		public:
			Target();
			void UpdateLayout(bool forceUpdate = false);

			std::list<Target*> subWindows;
			ILayout* layout;
			ZInt2Pair rect; // output
			ZInt2Pair margin;
			ZInt2Pair padding;
			ZInt2Pair size;
			ZInt2 scrollSize;
			ZInt2 scrollOffset;
			unsigned short weight;
			int start;
			int count;
			int remained;
			bool floated;
			bool hide;
			bool fitContent;
			bool needUpdate;
		};

		virtual ~ILayout();
		virtual ZInt2 DoLayout(const ZInt2Pair& rect, std::list<Target*>& targets, int start, int count, bool forceUpdate) const = 0;
	};
}

#endif // __ILAYOUT_H__
