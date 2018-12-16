#ifndef __ISTREAMBASE_H__
#define __ISTREAMBASE_H__

#include "IReflect.h"
#include "../PaintsNow.h"
#include <map>
#include <list>
#include <stack>

namespace PaintsNow {
	class IStreamBase : public IReflectObjectComplex {
	protected:
		virtual ~IStreamBase();
	public:
		virtual void Flush() = 0;
		virtual bool Read(void* p, size_t& len) = 0;
		virtual bool Write(const void* p, size_t& len) = 0;
		virtual bool Transfer(IStreamBase& stream, size_t& len) = 0;
		virtual bool WriteDummy(size_t& len) = 0;
		enum SEEK_OPTION { BEGIN, CUR, END };
		virtual bool Seek(SEEK_OPTION option, long offset) = 0;

		bool ReadBlock(void* p, size_t& len) {
			size_t org = len;
			if (Read(p, len)) {
				if (len == org) { // done
					return true;
				} else {
					Seek(IStreamBase::CUR, -(long)len);
					return false;
				}
			} else {
				return false;
			}
		}

		bool WriteBlock(const void* p, size_t& len) {
			size_t org = len;
			if (Write(p, len)) {
				if (len == org) {
					return true;
				} else {
					Seek(IStreamBase::CUR, -(long)len);
					return false;
				}
			} else {
				return false;
			}
		}

		class StreamState {
		public:
			StreamState(IStreamBase& s, bool su) : stream(s), success(su) {}
			template <class T>
			inline StreamState operator << (const T& t) {
				assert(success);
				if (success) {
					return stream << t;
				} else {
					return *this;
				}
			}

			template <class T>
			inline StreamState operator >> (const T& t) {
				assert(success);
				if (success) {
					return stream >> t;
				} else {
					return *this;
				}
			}

			inline operator bool() const {
				return success;
			}

			inline bool operator ! () const {
				return !success;
			}

		private:
			IStreamBase& stream;
			bool success;
		};

		template <class T>
		inline StreamState operator << (const T& t) {
			static Unique u = UniqueType<T>::Get();
			bool result = WriteForward(t, u, (void*)&t, sizeof(T));
			return StreamState(*this, result);
		}

		template <class T>
		inline StreamState operator >> (T& t) {
			static Unique u = UniqueType<T>::Get();
			bool result = ReadForward(t, u, (void*)&t, sizeof(T));
			return StreamState(*this, result);
		}

		virtual bool Write(IReflectObject& a, Unique type, void* ptr, size_t length);
		virtual bool Read(IReflectObject& a, Unique type, void* ptr, size_t length);

		template <class T>
		T Parse(UniqueType<T>) {
			T object;
			*this >> object;
			return object;
		}

	protected:
		bool WriteForward(const IReflectObject& a, Unique type, void* ptr, size_t length);
		bool ReadForward(const IReflectObject& a, Unique type, void* ptr, size_t length);
	};
}


#endif // __ISTREAMBASE_H__
