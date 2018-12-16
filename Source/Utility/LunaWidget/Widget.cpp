#include "Widget.h"
#include "../../Core/Template/TAlgorithm.h"
#include "../SnowyStream/Resource/Pipelines/WidgetPipeline.h"
#include "../SnowyStream/Resource/TextureResource.h"
#include "../SnowyStream/Resource/FontResource.h"
#include <queue>

using namespace PaintsNow;
using namespace PaintsNow::NsLunaWidget;
using namespace PaintsNow::NsSnowyStream;


Widget::TextCursorOption::TextCursorOption() : reverseColor(false) {}
Widget::TextRangeOption::TextRangeOption() : reverseColor(false) {}

TObject<IReflect>& Widget::TextCursorOption::operator () (IReflect& reflect) {
	ReflectClass(TextCursorOption)[Constructable];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(reverseColor)[ScriptVariable = "ReverseColor"];
		ReflectProperty(color)[ScriptVariable = "Color"];
		ReflectProperty(ch)[ScriptVariable = "Char"];
	}

	return *this;
}

TObject<IReflect>& Widget::TextRangeOption::operator () (IReflect& reflect) {
	ReflectClass(TextCursorOption)[Constructable];;
	if (reflect.IsReflectProperty()) {
		ReflectProperty(reverseColor)[ScriptVariable = "ReverseColor"];
		ReflectProperty(color)[ScriptVariable = "Color"];
	}

	return *this;
}

Widget::Widget(Widget* parentWidget) : SharedTiny(Tiny::TINY_MODIFIED | Tiny::TINY_PINNED | Widget::WIDGET_DISPLAY), parent(parentWidget), firstChild(nullptr), nextSibling(nullptr) {}

Widget::~Widget() {
	ClearChildren();
}

TShared<TextControl> Widget::GetTextControl(bool createOnEmpty) {
	if (createOnEmpty && !textControl)
		textControl = TShared<TextControl>::Make();

	return textControl;
}

void Widget::AppendChild(Widget* w, bool head) {
	// check if already exists
	assert(w != nullptr);
	Widget** tail;
	for (tail = &firstChild; *tail != nullptr; tail = &(*tail)->nextSibling) {
		if (*tail == w) return;
	}

	if (head) tail = &firstChild;

	w->ReferenceObject();
	w->parent = this;
	w->nextSibling = *tail;

	*tail = w;
}

void Widget::ClearChildren() {
	Widget* c = firstChild;
	firstChild = nullptr;

	while (c != nullptr) {
		Widget* w = c;
		c = c->nextSibling;
		w->parent = nullptr;
		w->ReleaseObject();
	}
}

void CheckRect(const ZInt2Pair& rect) {
	assert(rect.first.x() >= 0);
	assert(rect.first.y() >= 0);
	assert(rect.second.x() >= 0);
	assert(rect.second.y() >= 0);
}

void Widget::RemoveChild(Widget* w) {
	for (Widget** c = &firstChild; *c != nullptr; c = &(*c)->nextSibling) {
		if (*c == w) {
			*c = w->nextSibling;
			w->parent = nullptr;
			w->ReleaseObject();
			break;
		}
	}
}

void Widget::SetUpdateMark() {
	Flag() |= Tiny::TINY_MODIFIED;
}

void Widget::SetTexture(TShared<NsSnowyStream::TextureResource> texture, const ZFloat4& texCoord, const ZFloat4& percent) {
	mainTexture = texture;
	layout.texCoordRect = texCoord;
	layout.percentage = percent;
}

// Layouts

template <class T, class A>
inline ZInt2 CalcDimension(Widget* win, int& lastMargin, int& sum, int& range, int& count, A& margin, A& padding, A& size, T get) {
	ZInt2 offset;
	int value = get(size.first);
	int maxvalue = get(size.second);
	int m = get(margin.first);
	assert(m >= 0 && value >= 0);
	if (win->Flag() & Widget::WIDGET_ADAPTABLE) {
		CheckRect(win->layout.rect);
		win->layout.rect.second = win->layout.rect.first;
		win->UpdateLayout(true);
		CheckRect(win->layout.rect);
		maxvalue = value = get(win->layout.scrollSize) + get(win->layout.rect.second) - get(win->layout.rect.first);
		assert(value >= 0);
	}

	if (maxvalue - value >= 0) {
		m = (lastMargin == -1 ? 0 : Max(m, lastMargin)) + get(padding.first);
		assert(m >= 0);
		assert(value >= 0);
		sum += maxvalue - value;
		offset.x() = m;
		offset.y() = m + value;
		range -= (value + get(padding.first) + get(padding.second) + m);
		count++;
		lastMargin = get(margin.second);
		assert(lastMargin >= 0);
	} else {
		// hide element?
		lastMargin = Max(lastMargin, get(margin.second));
		assert(lastMargin >= 0);
	}

	return offset;
}

template <class Getter>
struct CompareRatio {
	CompareRatio() {}
	bool operator () (Widget* lhs, Widget* rhs) const {
		Getter get;
		assert(lhs->layout.weight != 0 && rhs->layout.weight != 0);
		int left = (get(lhs->layout.size.second) - get(lhs->layout.size.first)) * rhs->layout.weight, right = (get(rhs->layout.size.second) - get(rhs->layout.size.first)) * lhs->layout.weight;
		if (get(lhs->layout.size.second) == 0) {
			if (get(rhs->layout.size.second) == 0) {
				return false;
			} else {
				return true;
			}
		} else if (get(rhs->layout.size.second) == 0) {
			return false;
		} else {
			return left > right;
		}
	}
};

template <class Getter, class Other>
struct DoLayoutImpl {
public:
	typedef Widget Target;
	void DoLayout(Widget* widget) {
		// No children, no layout
		if (widget->firstChild == nullptr) return;


		// two getters, one gets main direction data, and the other one gets the remaining
		Getter get;
		Other other;

		// valid size
		const ZInt2Pair& range = widget->layout.rect;
		CheckRect(range);

		uint32_t winCount = widget->layout.count;
		const uint32_t start = widget->layout.start;
		Tiny::FLAG flag = widget->Flag().load();

		ZInt2 t = range.second - range.first;
		ZInt2 scrollSize = t;

		int count = 0;
		int margin = -1;
		int size = get(t);
		int sum = 0;
		int weight = 0;
		size_t num = 0;
		if (winCount == -1) {
			// calculate window count
			winCount = 0;
			for (Widget* w = widget->firstChild; w != nullptr; w = w->nextSibling) {
				winCount++;
			}
		}

		ZInt2 current = range.first;
		static CompareRatio<Getter> cmp;
		int valid = 0;
		std::priority_queue<Widget*, std::vector<Widget*>, CompareRatio<Getter> > sorted(cmp);
		const Tiny::FLAG checkFlag = Widget::WIDGET_DISPLAY | Tiny::TINY_PINNED;

		Widget* win;
		for (win = widget->firstChild; win != nullptr; win = win->nextSibling, ++num) {
			if ((int)num < start) {
				continue;
			} else if ((int)num >= start + winCount) {
				break;
			} else {
				ZInt2Pair& rect = win->layout.rect;
				CheckRect(rect);

				if ((win->Flag() & checkFlag) == checkFlag) {
					ZInt2 pair = CalcDimension(win, margin, sum, size, count, win->layout.margin, win->layout.padding, win->layout.size, Getter());
					assert(pair.x() >= 0 && pair.y() >= 0);
					if (get(win->layout.size.second) - get(win->layout.size.first) >= 0) {
						if (!(win->Flag() & Widget::WIDGET_ADAPTABLE)) {
							sorted.push(win);
							weight += win->layout.weight;
							valid++;
						}

						assert(get(current) >= 0);
						get(current) += get(win->layout.padding.first);
						assert(get(current) >= 0);
						rect.first = rect.second = current;

						CheckRect(rect);

						get(rect.first) += pair.x();
						get(rect.second) += pair.y();
						get(current) += pair.y() + get(win->layout.padding.second);
						win->layout.remained = valid;

						ZInt2Pair saveRange = range;
						other(rect.first) = other(saveRange.first) + other(win->layout.padding.first);
						other(rect.second) = other(saveRange.second) - other(win->layout.padding.second);
						if (other(win->layout.size.second) - other(win->layout.size.first) >= 0 && other(win->layout.size.second) > 0) {
							other(rect.second) = Min(Max(other(rect.second), other(rect.first) + other(win->layout.size.first)), other(rect.first) + other(win->layout.size.second));

							// Update scroll size, important!
							other(scrollSize) = Max(other(scrollSize), other(rect.second) - other(range.first));
						}

						CheckRect(rect);
					} else {
						get(rect.first) = get(rect.second) = 0;
					}
				} else {
					// move
					rect.first = range.first;

					ZInt2 s = win->layout.size.second;
					for (int i = 0; i < 2; i++) {
						if (s[i] == 0) {
							rect.second[i] = range.second[i];
						} else {
							rect.second[i] = range.first[i] + s[i];
						}
					}
					rect.first += win->layout.padding.first;
					rect.second -= win->layout.padding.second;

					CheckRect(rect);
				}
			}
		}

		while (size > 0 && !sorted.empty()) {
			Widget* top = sorted.top();
			// check!
			assert(get(top->layout.size.second) - get(top->layout.size.first) >= 0);
			if (get(top->layout.size.second) != 0 && size * top->layout.weight > ((get(top->layout.size.second) - get(top->layout.size.first)) * weight)) {
				int diff = get(top->layout.size.second) - get(top->layout.size.first);

				assert(diff >= 0);
				assert(size >= diff);
				size -= diff;
				get(top->layout.rect.second) += diff;
				top->layout.remained = diff;
				sorted.pop();
				weight -= top->layout.weight;

				CheckRect(top->layout.rect);
			} else {
				break;
			}
		}

		get(scrollSize) = Max(0, get(scrollSize) - size);
		other(scrollSize) = Max(0, other(scrollSize) - (other(range.second) - other(range.first)));
		// int totalHeight = range.first.y() + range.second.y();// +scrollSize.y();

		// allocate remaining
		if (size < 0) {
			size = 0;
		}

		while (!sorted.empty()) {
			Widget* top = sorted.top();
			int diff = size * top->layout.weight / weight;
			if (get(top->layout.size.second) - get(top->layout.size.first) >= 0 && get(top->layout.size.second) > 0) {
				diff = Min(diff, get(top->layout.size.second) - get(top->layout.rect.second) + get(top->layout.rect.first));
			}
			assert(diff >= 0);
			get(top->layout.rect.second) += diff;
			top->layout.remained = diff;
			sorted.pop();
			CheckRect(top->layout.rect);
		}

		int remained = 0;
		for (win = widget->firstChild; win != nullptr; win = win->nextSibling) {
			if ((win->Flag() & checkFlag) == checkFlag) {
				assert(win->layout.remained >= 0);
				ZInt2Pair& rect = win->layout.rect;
				get(rect.first) += remained;
				get(rect.second) += remained;
				remained += win->layout.remained;
				CheckRect(rect);
				// last = get(rect.second);
				/*
				int diff = totalHeight - rect.second.y() - rect.first.y();
				rect.first.y() += diff;
				rect.second.y() += diff;
				rect.first.y() = Max(range.first.y(), rect.first.y());
				rect.second.y() = Max(range.first.y(), rect.second.y());*/

				// printf("Rect (%d, %d, %d, %d)\n", rect.first.x(), rect.first.y(), rect.second.x(), rect.second.y());
				// add padding ...
				CheckRect(rect);

				win->Flag() |= Tiny::TINY_MODIFIED;
			}

			CheckRect(win->layout.rect);
		}


		widget->layout.scrollSize = scrollSize;

	}
};

template <size_t k>
struct GetK {
	int& operator () (ZInt2& val) {
		return val[k];
	}
	const int operator () (const ZInt2& val) const {
		return val[k];
	}
};

typedef GetK<0> GetX;
typedef GetK<1> GetY;

void Widget::UpdateLayout(bool forceUpdate) {
	if (forceUpdate || (Flag() & TINY_MODIFIED)) {
		if (Flag() & WIDGET_VERTICAL) {
			static DoLayoutImpl<GetY, GetX> impl;
			impl.DoLayout(this);
		} else {
			static DoLayoutImpl<GetX, GetY> impl;
			impl.DoLayout(this);
		}

		Flag() &= ~TINY_MODIFIED;
	}
}

TObject<IReflect>& Widget::operator () (IReflect& reflect) {
	ReflectClass(Widget)[Interface(Tiny)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(parent)[Runtime];
		ReflectProperty(firstChild)[Runtime];
		ReflectProperty(nextSibling)[Runtime];

		ReflectProperty(layout)[Runtime];
		ReflectProperty(textControl)[Runtime];
	}

	return *this;
}

Widget::Layout::Layout() : count(-1), start(0), weight(1), remained(0), rect(ZInt2(0, 0), ZInt2(0, 0)), percentage(0, 0, 1, 1), texCoordRect(0, 0, 1, 1), color(1, 1, 1, 1), updateTimeStamp(0) {}
