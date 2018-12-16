#include "TextControl.h"
#include "../SnowyStream/Resource/Pipelines/WidgetPipeline.h"

using namespace PaintsNow;
using namespace PaintsNow::NsLunaWidget;
using namespace PaintsNow::NsSnowyStream;

static int GetUtf8Size(char c) {
	int t = 1 << 7;
	int r = c;
	int count = 0;
	while (r & t) {
		r = r << 1;
		count++;
	}
	return count == 0 ? 1 : count;
}

class TextControl::TagParser {
public:
	enum TYPE { TEXT = 0, RETURN, COLOR, ALIGN };
	struct Node {
		Node(TYPE t, size_t off) : type(t), offset(off) {}
		virtual ~Node() {}
		virtual bool IsClose() const { return false; }
		TYPE type;
		size_t offset;
		virtual void Print() const = 0;
	};

	struct NodeText : public Node {
	public:
		NodeText(size_t offset, const char* s, size_t len) : Node(TEXT, offset), length((int)len) {
			text = new char[len + 1];
			bool slash = false;
			char* p = text;
			for (size_t i = 0; i < len; i++) {
				// if (!(slash = (!slash && s[i] == '\\'))) {
				*p++ = s[i];
				// }
			}
			*p = '\0';
			length = (int)(p - text);
		}

		virtual ~NodeText() {
			delete[] text;
		}

		virtual void Print() const {
			printf("NodeText: %s\n", text);
		}

		char* text;
		int length;
	};

	struct NodeReturn : public Node {
		NodeReturn(size_t offset) : Node(RETURN, offset) {}
		virtual void Print() const {
			printf("NodeReturn\n");
		}
	};

	struct NodeAlign : public Node {
		enum ALIGN_TYPE { LEFT, RIGHT, CENTER };
		NodeAlign(size_t offset, ALIGN_TYPE type) : Node(ALIGN, offset), align(type) {}
		virtual void Print() const {
			printf("ALIGN TYPE: %d\n", align);
		}

		ALIGN_TYPE align;
	};

	struct NodeColor : public Node {
		NodeColor(size_t offset, int v, bool c = false) : Node(COLOR, offset), value(v), close(c) {}

		virtual void Print() const {
			printf("NodeColor: %d\n", value);
		}

		virtual bool IsClose() const {
			return close;
		}

		int value;
		bool close;
	};

	void Parse(const char* str, bool append = false) {
		if (!append)
			Clear();

		bool less = false;

		const char* q = nullptr;
		const char* p = nullptr;
		bool slash = false;
		for (p = str, q = str; *p != '\0'; ++p) {
			int i = GetUtf8Size(*p);
			if (i != 1) {
				p += i - 1;
				continue;
			}

			if (*p == '\\') {
				slash = true;
			} else {
				if (*p == '<' && !slash) {
					PushText(q - str, q, p);
					less = true;
					q = p + 1;
				} else if (*p == '>' && less && !slash) {
					assert(q != nullptr);
					PushFormat(q - str, q, p);
					less = false;
					q = p + 1;
				} else if (*p == '\n' && !less) {
					PushText(q - str, q, p);
					PushReturn(q - str);
					q = p + 1;
				}

				slash = false;
			}
		}

		PushText(q - str, q, p);
	}

	void Push(Node* node) {
		nodes.push_back(node);
	}

	void PushReturn(size_t offset) {
		NodeReturn* ret = new NodeReturn(offset);
		Push(ret);
	}

	bool ParseAttrib(const char*& valueString, bool& isClose, const char* start, const char* end, const char* attrib) {
		assert(attrib != nullptr);
		bool ret = false;
		valueString = nullptr;
		isClose = false;
		const size_t len = strlen(attrib);
		char* format = new char[end - start + 1];
		memcpy(format, start, sizeof(char)* (end - start));
		format[end - start] = '\0';

		if (strncmp(format, attrib, len) == 0) {
			// find "="
			const char* t;
			for (t = format + len; *t != '\0'; ++t) {
				if (*t == '=')
					break;
			}

			while (*t != '\0' && !isalnum(*t)) t++;

			if (*t != '\0')
				valueString = start + (t - format);
			ret = true;
		} else if (format[0] == '/' && strncmp(format + 1, attrib, len) == 0) {
			isClose = true;
			ret = true;
		}

		delete[] format;

		return ret;
	}

	void PushFormat(size_t offset, const char* start, const char* end) {
		const char* valueString;
		bool isClose;
		if (ParseAttrib(valueString, isClose, start, end, "color")) {
			if (isClose) {
				NodeColor* p = new NodeColor(offset, 0, true);
				Push(p);
			} else {
				if (valueString != nullptr) {
					int value;
					sscanf(valueString, "%x", &value);
					NodeColor* p = new NodeColor(offset, value);
					Push(p);
				}
			}
		} else if (ParseAttrib(valueString, isClose, start, end, "align")) {
			NodeAlign::ALIGN_TYPE t = NodeAlign::LEFT;
			static const char* right = "right";
			static const char* center = "center";
			if (memcmp(valueString, right, strlen(right)) == 0) {
				t = NodeAlign::RIGHT;
			} else if (memcmp(valueString, center, strlen(center)) == 0) {
				t = NodeAlign::CENTER;
			}

			NodeAlign* p = new NodeAlign(offset, t);
			Push(p);
		}
	}

	void PushText(size_t offset, const char* start, const char* end) {
		if (start != end) {
			NodeText* text = new NodeText(offset, start, end - start);
			Push(text);
		}
	}

	void Clear() {
		for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			delete *it;
		}

		nodes.clear();
	}

	~TagParser() {
		Clear();
	}

	void Print() {
		for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			(*it)->Print();
		}
	}

	std::list<Node*> nodes;
};


TextControl::TextControl() : textLength(0), passwordChar(0), cursorChar(0), cursorPos(0), fontSize(12), cursorRevColor(true), selectRevColor(true) {
	parser = new TagParser();
}

TextControl::~TextControl() {
	delete parser;
}

const ZInt2& TextControl::GetFullSize() const {
	return fullSize;
}

void TextControl::Scroll(const ZInt2& pt) {
	scroll = pt;
}

void TextControl::TextChange() {
	widthInfo = std::vector<Descriptor>();
	/*
	if (mainFont) {
		widthInfo = std::vector<Descriptor>();
	//	fullSize = PerformRender(ZInt2Pair(ZInt2(0, 0), size), widthInfo, ZInt2(1, 1), nullptr);
	}*/
}

void TextControl::SetText(const String& t) {
	// String withspace = text + " ";
	text = t;
	parser->Parse(text.c_str());
	textLength = text.length();
	TextChange();
}

static int Utf8ToUnicode(const unsigned char* s, int size) {
	char uc[] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03 };
	unsigned long data = 0;
	unsigned long tmp = 0;
	int count = 0;

	const unsigned char* p = s;
	while (count < size) {
		unsigned char c = (*p);
		tmp = c;

		if (count == 0) {
			tmp &= uc[size - 1];
		} else {
			tmp &= 0x3f;
		}

		tmp = tmp << (6 * (size - count - 1));
		data |= tmp;

		p++;
		count++;
	}

	return data;
}

void TextControl::FireRender(IRender& render, WidgetPipeline& pipeline, const ZInt2Pair& rect, const ZInt2& totalSize, const ZInt2& padding) {
	std::vector<Descriptor> info;
	PerformRender(render, rect, info, totalSize, padding, &pipeline);
	std::swap(info, widthInfo);
}

TextControl::Descriptor::Descriptor(int h, int s) : totalWidth(0), firstOffset(h) {}
TextControl::Descriptor::Char::Char(int c, int off) : xCoord(c), offset(off) {}

ZInt2 TextControl::PerformRender(IRender& render, const ZInt2Pair& range, std::vector<Descriptor>& widthRecords, const ZInt2& totalSize, const ZInt2& padding, WidgetPipeline* pipeline) const {
	ZInt2 size;
	if (!mainFont) {
		return ZInt2(0, 0);
	}

	ZInt2 fullSize;
	// const double EPSILON = 1e-6;
	assert(parser != nullptr);

	int ws = range.second.x() - range.first.x(); //, wh = range.second.y() - range.first.y();
	int h = fontSize + padding.y();
	assert(h != 0);

	ZInt2 start = scroll;
	int count = 0;
	int currentWidth = -start.x();
	int maxWidth = 0;
	int currentHeight = -start.y();

	// if (currentWidth < 0) currentWidth = 0;
	// if (currentHeight < 0) currentHeight = 0;
	bool full = false;
	const TagParser* s = parser;

	int align = TagParser::NodeAlign::LEFT;

	ZInt2 texSize;
	mainFont->GetFontTexture(fontSize, texSize);

	const FontResource::Char& cursor = mainFont->Get(cursorChar, fontSize);
	ZInt2 current;
	const FontResource::Char& pwd = mainFont->Get(passwordChar, fontSize);
	ZFloat4 color(1, 1, 1, 1);
	widthRecords.push_back(Descriptor(currentHeight, 0));
	FontResource::Char info;
	bool showCursor = false;

	for (std::list<TagParser::Node*>::const_iterator it = s->nodes.begin(); it != s->nodes.end(); ++it) {
		const TagParser::Node* node = *it;
		if (node->type == TagParser::TEXT) {
			const TagParser::NodeText* text = static_cast<const TagParser::NodeText*>(node);
			int size = 1;
			for (const char* p = text->text; p < text->text + text->length; p += size) {
				// use utf-8 as default encoding
				size = GetUtf8Size(*p);
				info = passwordChar != 0 ? pwd : mainFont->Get(Utf8ToUnicode((const unsigned char*)p, size), fontSize);

				int temp = info.info.adv.x() + padding.x();
				currentWidth += temp;
				maxWidth = maxWidth > currentWidth ? maxWidth : currentWidth;

				if (currentHeight > range.second.y() - range.first.y() - h) {
					full = true;
					//	break;
				}

				if (temp >= ws) {
					break;
				}

				if (currentWidth + start.x() >= ws) {
					widthRecords.push_back(Descriptor(currentHeight, (int)text->offset));
					currentWidth = -start.x();
					currentHeight += h;
					p -= size;

					count++;
					continue;
				}

				int offset = (int)(text->offset + (p - text->text));
				widthRecords.back().allOffsets.push_back(Descriptor::Char(currentWidth - temp / 2, offset));
				widthRecords.back().totalWidth = currentWidth + start.x();

				if (pipeline != nullptr && !full && currentHeight >= 0) {
					int wt = info.info.width;
					int ht = info.info.height;
					int centerOffset = (temp - info.info.width) / 2;
					int alignOffset = (size_t)count >= this->widthInfo.size() ? 0 : align == TagParser::NodeAlign::LEFT ? 0 : align == TagParser::NodeAlign::CENTER ? (ws - this->widthInfo[count].totalWidth) / 2 : (ws - this->widthInfo[count].totalWidth);
					current = ZInt2(currentWidth - temp + range.first.x() + alignOffset + info.info.bearing.x(), currentHeight + range.first.y() + (h - ht) - info.info.delta.y());

					ZFloat4 c = color;
					if (selectRange.x() <= offset && selectRange.y() > offset) {
						if (selectRevColor) {
							c = ZFloat4(1.0, 1.0, 1.0, 1.0) - color;
							c.a() = color.a();
						} else {
							c = selectColor;
						}
					}

					ZInt2 end(current.x() + wt, current.y() + ht);
					RenderCharacter(render, *pipeline, range, ZInt2Pair(current, end), info.rect, c, totalSize, fontSize);

					// if cursor ?
					if (!showCursor && cursorPos <= offset && cursorChar != 0) {
						current.y() += info.info.delta.y() - cursor.info.delta.y() + ht - cursor.info.height;
						// current.x() -= cursor.info.width;
						if (cursorRevColor) {
							c = ZFloat4(1.0, 1.0, 1.0, 1.0) - color;
						} else {
							c = cursorColor;
						}

						ZInt2 m(current.x() + cursor.info.width, current.y() + cursor.info.height);
						RenderCharacter(render, *pipeline, range, ZInt2Pair(current, m), cursor.rect, c, totalSize, fontSize);
						showCursor = true;
					}
				}
			} // end for

			if (full) break;
		} else if (node->type == TagParser::ALIGN) {
			TagParser::NodeAlign* p = (TagParser::NodeAlign*)node;
			align = p->align;
		} else if (node->type == TagParser::RETURN) {
			widthRecords.push_back(Descriptor(start.x(), (int)node->offset));
			currentWidth = -start.x();
			currentHeight += h;
			count++;
		} else if (node->type == TagParser::COLOR) {
			// TODO: render!
			if (pipeline != nullptr) {
				const TagParser::NodeColor* c = static_cast<const TagParser::NodeColor*>(node);
				if (!c->IsClose()) {
					color = ZFloat4((float)((c->value >> 16) & 0xff) / 255, (float)((c->value >> 8) & 0xff) / 255, (float)(c->value & 0xff) / 255, 1);
				}
			}
		}
	}

	// if cursor ?
	if (pipeline != nullptr && !full && currentHeight >= 0 && !showCursor && cursorChar != 0) {
		ZFloat4 c;
		current.x() += info.info.width;
		current.y() += info.info.delta.y() - cursor.info.delta.y() + info.info.height - cursor.info.height;
		// current.x() -= cursor.info.width;
		if (cursorRevColor) {
			c = ZFloat4(1.0, 1.0, 1.0, 1.0) - color;
		} else {
			c = cursorColor;
		}

		ZInt2 m(current.x() + cursor.info.width, current.y() + cursor.info.height);
		RenderCharacter(render, *pipeline, range, ZInt2Pair(current, m), cursor.rect, c, totalSize, fontSize);
		showCursor = true;
	}

	count++;
	currentHeight += h;

	fullSize.x() = maxWidth + start.x();
	fullSize.y() = currentHeight + start.y();

	return fullSize;
}

void TextControl::RenderCharacter(IRender& render, WidgetPipeline& pipeline, const ZInt2Pair& range, const ZInt2Pair& rect, const ZInt2Pair& info, const ZFloat4& color, const ZInt2& totalSize, uint32_t fontSize) const {
	ZInt2 fontTexSize;
	IRender::Texture* texture = mainFont->GetFontTexture(fontSize, fontTexSize);
	if (texture != nullptr) {
		ZInt2Pair inv = rect;
		inv.first.y() = range.second.y() - inv.first.y() + range.first.y();
		inv.second.y() = range.second.y() - inv.second.y() + range.first.y();
		std::swap(inv.first.y(), inv.second.y());
		ZFloat4 position = ZFloat4(
			(float)rect.first.x() / totalSize.x(),
			1.0f - (float)rect.second.y() / totalSize.y(),
			(float)rect.second.x() / totalSize.x(),
			1.0f - (float)rect.first.y() / totalSize.y()
		);

		ZFloat4 texCoord = ZFloat4(
			(float)info.first.x() / fontTexSize.x(),
			(float)info.first.y() / fontTexSize.y(),
			(float)info.second.x() / fontTexSize.x(),
			(float)info.second.y() / fontTexSize.y()
		);

		ZFloat4 texCoordMark(0.5f, 0.5f, 0.5f, 0.5f);
		ZFloat4 texCoordScale(1, 1, 1, 1);

		pipeline.widgetTransform.inputPositionRect = position * 2.0f - 1.0f;
		pipeline.widgetShading.inputTexCoordRect = texCoord;
		pipeline.widgetShading.inputTexCoordMark = texCoordMark;
		pipeline.widgetShading.inputTexCoordScale = texCoordScale;
		pipeline.widgetShading.mainTexture = texture;
		pipeline.FireRender(render);
	}
}

int TextControl::GetLineCount() const {
	return (int)widthInfo.size();
}

void TextControl::AppendText(const String& text) {
	if (parser != nullptr) {
		parser->Parse(text.c_str(), true);
		TextChange();
	}
}

void TextControl::SetPasswordChar(int ch) {
	passwordChar = ch;
}

void TextControl::SetSize(const ZInt2& s) {
	size = s;
	TextChange();
}

const ZInt2& TextControl::GetSize() const {
	return size;
}

struct LocateLineOffset {
	bool operator () (const TextControl::Descriptor& desc, int offset) {
		return desc.firstOffset < offset;
	}
};

struct LocatePosOffset {
	bool operator () (const TextControl::Descriptor::Char& desc, int offset) {
		return desc.offset < offset;
	}
};


struct LocateLine {
	bool operator () (const TextControl::Descriptor& desc, const ZInt2& pt) {
		return desc.yCoord < pt.y();
	}
};

struct LocatePos {
	bool operator () (const TextControl::Descriptor::Char& desc, const ZInt2& pt) {
		return desc.xCoord < pt.x();
	}
};

int TextControl::Locate(ZInt2& rowCol, const ZInt2& pt, bool isPtRowCol) const {
	if (widthInfo.empty()) {
		rowCol = ZInt2(0, 0);
		return 0;
	}

	if (isPtRowCol) {
		rowCol.x() = Max(0, Min((int)widthInfo.size() - 1, pt.x()));
		const Descriptor& desc = widthInfo[rowCol.y()];

		rowCol.y() = Max(0, Min((int)desc.allOffsets.size(), pt.y()));
		if (rowCol.y() == (int)desc.allOffsets.size()) {
			return (int)textLength;
		} else {
			return desc.allOffsets[rowCol.y()].offset;
		}
	} else {
		std::vector<Descriptor>::const_iterator p = std::lower_bound(widthInfo.begin(), widthInfo.end(), pt, LocateLine());
		if (p == widthInfo.end()) {
			--p;
		}
		rowCol.x() = (int)(p - widthInfo.begin());

		std::vector<TextControl::Descriptor::Char>::const_iterator t = std::lower_bound(p->allOffsets.begin(), p->allOffsets.end(), pt, LocatePos());

		rowCol.y() = (int)(t - p->allOffsets.begin());
		if (t == p->allOffsets.end()) {
			std::vector<Descriptor>::const_iterator q = p;
			q++;
			if (q != widthInfo.end() && !(*q).allOffsets.empty()) {
				return q->firstOffset;
			} else {
				return (int)textLength;
			}
		} else {
			return t->offset;
		}
	}
}

bool TextControl::IsEmpty() const {
	return parser->nodes.empty();
}
