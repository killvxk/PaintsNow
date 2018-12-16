// TShaderVariable.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-5
//

#ifndef __TSHADERVARIABLE_H__
#define __TSHADERVARIABLE_H__


#include "../PaintsNow.h"
#include "../Interface/Interfaces.h"
#include "../Interface/IReflect.h"
#include <string>

namespace PaintsNow {
	// typedef Final<VRender>::Class IRender;
	class IShaderVariableBase : public IReflectObjectComplex {
	public:
		enum VAR_TYPE {
			IMPORT, // DATA: none
			LOCAL,
			INPUT_ATTRIB_BUFFER, // uniform var
			INPUT_INSTANCED_BUFFER,
			INPUT_UNIFORM,
			INPUT_UNIFORM_TEXTURE,
			INPUT_UNIFORM_BUFFER, // UBO
			INPUT_UNIFORM_BUFFER_FIELD, // UBO field
			INPUT_VARYING,
			OUTPUT_POSITION,
			OUTPUT_VARYING,
			OUTPUT_TARGET
		};

		virtual ~IShaderVariableBase() {};
		typedef void (*Binder)(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data);
		typedef void (*Exporter)(IShaderVariableBase* var, void* data);
		typedef void (*Importer)(IShaderVariableBase* var, const void* data);
		virtual size_t GetDataLength() const = 0;
		virtual void Bind(IRender& render, IRender::Program* program) = 0;
		virtual Binder GetBinder() const = 0;
		virtual Exporter GetExporter() const = 0;
		virtual Importer GetImporter() const = 0;
		virtual Unique GetSubUnique() const = 0;
		virtual void Initialize(IRender& render, IRender::Program* program, const char* name) = 0;
		virtual VAR_TYPE GetType() const = 0;
		virtual void SetType(VAR_TYPE varType) = 0;
		virtual IRender::Buffer*& GetBuffer() = 0;

		virtual TObject<IReflect>& operator () (IReflect& reflect) {
			ReflectClass(IShaderVariableBase);
			return *this;
		}

		static bool IsTexture(VAR_TYPE varType) {
			switch (varType) {
				case INPUT_UNIFORM_TEXTURE:
				case OUTPUT_TARGET:
					return true;
			}

			return false;
		}
		
		static bool IsBuffer(VAR_TYPE varType) {
			switch (varType) {
				case INPUT_ATTRIB_BUFFER:
				case INPUT_UNIFORM_BUFFER:
				case INPUT_INSTANCED_BUFFER:
					return true;
			}

			return false;
		}
	};


	template <class T, IShaderVariableBase::VAR_TYPE defType = IShaderVariableBase::INPUT_UNIFORM>
	class TShaderVariable : public IShaderVariableBase {
	public:
		TShaderVariable(const T& v = T()) : varType(defType), value(v), location((IRender::Location)-1){
			buffer = nullptr;
			texture = nullptr;
#ifdef _DEBUG
			valueAssigned = varType == OUTPUT_TARGET; // target is default to screen (nullptr)
#endif
		}

		virtual IRender::Buffer*& GetBuffer() {
			assert(varType == INPUT_UNIFORM_BUFFER || varType == INPUT_ATTRIB_BUFFER);
			return buffer;
		}

		virtual VAR_TYPE GetType() const {
			return varType;
		}

		virtual void SetType(VAR_TYPE t) {
			varType = t;
		}

		virtual Unique GetSubUnique() const {
			switch (varType) {
			case IMPORT:
				return UniqueType<TShaderVariable<T, IMPORT> >::Get();
			case LOCAL:
				return UniqueType<TShaderVariable<T, LOCAL> >::Get();
			case INPUT_ATTRIB_BUFFER:
				return UniqueType<TShaderVariable<T, INPUT_ATTRIB_BUFFER> >::Get();
			case INPUT_UNIFORM:
				return UniqueType<TShaderVariable<T, INPUT_UNIFORM> >::Get();
			case INPUT_UNIFORM_TEXTURE:
				return UniqueType<TShaderVariable<T, INPUT_UNIFORM_TEXTURE> >::Get();
			case INPUT_UNIFORM_BUFFER:
				return UniqueType<TShaderVariable<T, INPUT_UNIFORM_BUFFER> >::Get();
			case INPUT_INSTANCED_BUFFER:
				return UniqueType<TShaderVariable<T, INPUT_INSTANCED_BUFFER> >::Get();
			case INPUT_VARYING:
				return UniqueType<TShaderVariable<T, INPUT_VARYING> >::Get();
			case OUTPUT_POSITION:
				return UniqueType<TShaderVariable<T, OUTPUT_POSITION> >::Get();
			case OUTPUT_VARYING:
				return UniqueType<TShaderVariable<T, OUTPUT_VARYING> >::Get();
			case OUTPUT_TARGET:
				return UniqueType<TShaderVariable<T, OUTPUT_TARGET> >::Get();
			}

			return UniqueType<TShaderVariable>::Get();
		}

#ifdef __TSHADERGENERATOR_H__
#if defined(_MSC_VER) && _MSC_VER <= 1200
		TShaderVariable(float a, float b = 0, float c = 0, float d = 0) {}
#else
		template <typename... Args>
		TShaderVariable(float first, Args... args) {
			static_assert(sizeof...(args) == T::size - 1, "Invalid parameter count");
		}
#endif
		static ShaderVarCheckType1<T> vecType1() {
			return ShaderVarCheckType1<T>();
		}
		static ShaderVarCheckType2<T> vecType2() {
			return ShaderVarCheckType2<T>();
		}
		static ShaderVarCheckType3<T> vecType3() {
			return ShaderVarCheckType3<T>();
		}
		static ShaderVarCheckType4<T> vecType4() {
			return ShaderVarCheckType4<T>();
		}

		operator T () {
			return Value();
		}

		TShaderVariable<T, defType>& operator = (const TShaderVariable<T, IMPORT>& rhs) {
			return *this;
		}

		TShaderVariable<T, defType>& operator = (const TShaderVariable<T, LOCAL>& rhs) {
			return *this;
		}

		TShaderVariable<T, defType>& operator = (const TShaderVariable<T, INPUT_VARYING>& rhs) {
			return *this;
		}

		TShaderVariable<T, defType> operator + (const TShaderVariable<T, IMPORT>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator + (const TShaderVariable<T, LOCAL>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator + (const TShaderVariable<T, INPUT_VARYING>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator + (const T& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator + (double v) const {
			return *this;
		}

		TShaderVariable<T, defType> operator - (const TShaderVariable<T, IMPORT>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator - (const TShaderVariable<T, LOCAL>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator - (const TShaderVariable<T, INPUT_VARYING>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator - (const T& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator - (double v) const {
			return *this;
		}

		TShaderVariable<T, defType> operator * (const TShaderVariable<T, IMPORT>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator * (const TShaderVariable<T, LOCAL>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator * (const TShaderVariable<T, INPUT_VARYING>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator * (const T& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator * (double v) const {
			return *this;
		}

		TShaderVariable<T, defType> operator / (const TShaderVariable<T, IMPORT>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator / (const TShaderVariable<T, LOCAL>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator / (const TShaderVariable<T, INPUT_VARYING>& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator / (const T& rhs) const {
			return *this;
		}

		TShaderVariable<T, defType> operator / (double v) const {
			return *this;
		}
#endif // __TSHADERGENERATOR_H__


		virtual size_t GetDataLength() const {
			return IsBuffer(varType) ? sizeof(IRender::Buffer*) : IsTexture(varType) ? sizeof(IRender::Texture*) : sizeof(T);
		}

		static void BindAttribBuffer(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data) {
			render.BindAttrib(program, static_cast<TShaderVariable*>(var)->location, *reinterpret_cast< IRender::Buffer* const*>(data), sizeof(T) / sizeof(float));
		}

		static void BindUniformBuffer(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data) {
			render.BindUniform(program, static_cast<TShaderVariable*>(var)->location, *reinterpret_cast< IRender::Buffer* const*>(data));
		}

		static void BindUniform(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data) {
			render.BindUniform(program, static_cast<TShaderVariable*>(var)->location, reinterpret_cast<const float*>(data), sizeof(T) / sizeof(float));
		}

		static void BindUniformTexture(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data) {
			render.BindUniform(program, static_cast<TShaderVariable*>(var)->location, *reinterpret_cast< IRender::Texture* const*>(data));
		}

		static void BindOutputTexture(IRender& render, IRender::Program* program, IShaderVariableBase* var, const void* data) {
			render.BindOutput(program, static_cast<TShaderVariable*>(var)->location, *reinterpret_cast< IRender::Texture* const*>(data));
		}

		virtual Binder GetBinder() const {
			switch (varType) {
				case INPUT_ATTRIB_BUFFER:
					return BindAttribBuffer;
				case INPUT_UNIFORM:
					return BindUniform;
				case INPUT_UNIFORM_TEXTURE:
					return BindUniformTexture;
				case INPUT_UNIFORM_BUFFER:
					return BindUniformBuffer;
				case INPUT_INSTANCED_BUFFER:
					return BindAttribBuffer;
				case OUTPUT_TARGET:
					return BindOutputTexture;
			}

			return nullptr;
		}

		static void ExportAttribBuffer(IShaderVariableBase* var, void* data) {
			*reinterpret_cast<IRender::Buffer**>(data) = static_cast<TShaderVariable*>(var)->buffer;
		}

		static void ImportAttribBuffer(IShaderVariableBase* var, const void* data) {
			static_cast<TShaderVariable*>(var)->buffer = *reinterpret_cast<IRender::Buffer* const*>(data);
		}

		static void ExportUniformBuffer(IShaderVariableBase* var, void* data) {
			*reinterpret_cast<IRender::Buffer**>(data) = static_cast<TShaderVariable*>(var)->buffer;
		}

		static void ImportUniformBuffer(IShaderVariableBase* var, const void* data) {
			static_cast<TShaderVariable*>(var)->buffer = *reinterpret_cast<IRender::Buffer* const*>(data);
		}

		static void ExportUniform(IShaderVariableBase* var, void* data) {
			*reinterpret_cast<T*>(data) = static_cast<TShaderVariable*>(var)->value;
		}

		static void ImportUniform(IShaderVariableBase* var, const void* data) {
			static_cast<TShaderVariable*>(var)->value = *reinterpret_cast<T const*>(data);
		}

		static void ExportUniformTexture(IShaderVariableBase* var, void* data) {
			*reinterpret_cast<IRender::Texture**>(data) = static_cast<TShaderVariable*>(var)->texture;
		}

		static void ImportUniformTexture(IShaderVariableBase* var, const void* data) {
			static_cast<TShaderVariable*>(var)->texture = *reinterpret_cast<IRender::Texture* const*>(data);
		}

		static void ExportOutputTexture(IShaderVariableBase* var, void* data) {
			*reinterpret_cast<IRender::Texture**>(data) = static_cast<TShaderVariable*>(var)->texture;
		}

		static void ImportOutputTexture(IShaderVariableBase* var, const void* data) {
			static_cast<TShaderVariable*>(var)->texture = *reinterpret_cast<IRender::Texture* const*>(data);
		}

		virtual Importer GetImporter() const {
			switch (varType) {
				case INPUT_ATTRIB_BUFFER:
					return ImportAttribBuffer;
				case INPUT_UNIFORM:
					return ImportUniform;
				case INPUT_UNIFORM_TEXTURE:
					return ImportUniformTexture;
				case INPUT_UNIFORM_BUFFER:
					return ImportUniformBuffer;
				case INPUT_INSTANCED_BUFFER:
					return ImportAttribBuffer;
				case OUTPUT_TARGET:
					return ImportOutputTexture;
			}

			return nullptr;
		}

		virtual Exporter GetExporter() const {
			switch (varType) {
				case INPUT_ATTRIB_BUFFER:
					return ExportAttribBuffer;
				case INPUT_UNIFORM:
					return ExportUniform;
				case INPUT_UNIFORM_TEXTURE:
					return ExportUniformTexture;
				case INPUT_UNIFORM_BUFFER:
					return ExportUniformBuffer;
				case INPUT_INSTANCED_BUFFER:
					return ExportAttribBuffer;
				case OUTPUT_TARGET:
					return ExportOutputTexture;
			}

			return nullptr;
		}

		virtual void Initialize(IRender& render, IRender::Program* program, const char* name) {
			if ((size_t)location == (size_t)-1) {
				switch (varType) {
				case INPUT_UNIFORM_BUFFER:
					location = render.GetUniformBufferLocation(program, name);
					assert((size_t)location != (size_t)-1); // must be set before calling me
					break;
				case INPUT_UNIFORM:
				case INPUT_UNIFORM_TEXTURE:
					location = render.GetUniformLocation(program, name);
					assert((size_t)location != (size_t)-1); // must be set before calling me
					break;
				case INPUT_ATTRIB_BUFFER:
					location = render.GetAttribLocation(program, name);
					assert((size_t)location != (size_t)-1); // must be set before calling me
					break;
				case OUTPUT_TARGET:
					location = render.GetOutputLocation(program, name);
					assert((size_t)location != (size_t)-1); // must be set before calling me
					break;
				}
			}
		}

		inline TShaderVariable& operator = (const T& v) {
			assert(varType == INPUT_UNIFORM || varType == INPUT_UNIFORM_BUFFER || varType == OUTPUT_TARGET);
			value = v;
#ifdef _DEBUG
			valueAssigned = true;
#endif
			return *this;
		}

		inline TShaderVariable& operator = (IRender::Buffer* b) {
			assert(varType == INPUT_ATTRIB_BUFFER || varType == INPUT_UNIFORM_BUFFER);
			buffer = b;
#ifdef _DEBUG
			valueAssigned = true;
#endif
			return *this;
		}

		inline TShaderVariable& operator = (IRender::Texture* t) {
			assert(varType == INPUT_UNIFORM_TEXTURE || varType == OUTPUT_TARGET);
#ifdef _DEBUG
			valueAssigned = true;
#endif
			texture = t;
			return *this;
		}

		inline const T& Value() const {
			return value;
		}

		inline T& Value() {
			return value;
		}

		static void BindValue(IReflectObject* s, IRender& render, IRender::Program* program, IRender::Location location) {
			assert(false); // must use ubo
		}

		static void BindValue(float d, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, (const float*)&d, sizeof(d) / sizeof(float));
		}

		static void BindValue(const ZFloat2& d, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, (const float*)&d, sizeof(d) / sizeof(float));
		}

		static void BindValue(const ZFloat3& d, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, (const float*)&d, sizeof(d) / sizeof(float));
		}

		static void BindValue(const ZFloat4& d, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, (const float*)&d, sizeof(d) / sizeof(float));
		}

		static void BindValue(const ZMatrixFloat4x4& mat, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, mat);
		}

		static void BindValue(const ZMatrixFloat3x3& mat, IRender& render, IRender::Program* program, IRender::Location location) {
			render.BindUniform(program, location, mat);
		}

		virtual void Bind(IRender& render, IRender::Program* program) {
			if (GetBinder() == nullptr) return;
#ifdef _DEBUG
			assert(valueAssigned);
#endif
			switch (varType) {
				case INPUT_UNIFORM_BUFFER:
					assert((size_t)location != (size_t)-1); // must be set before calling me
					render.BindUniform(program, location, buffer);
					break;
				case INPUT_UNIFORM:
					assert((size_t)location != (size_t)-1); // must be set before calling me
					BindValue(value, render, program, location);
					break;
				case INPUT_UNIFORM_TEXTURE:
					assert((size_t)location != (size_t)-1); // must be set before calling me
					render.BindUniform(program, location, texture);
					break;
				case INPUT_ATTRIB_BUFFER:
					assert((size_t)location != (size_t)-1); // must be set before calling me
					render.BindAttrib(program, location, buffer, sizeof(T) / sizeof(float));
					break;
				case OUTPUT_TARGET:
					assert((size_t)location != (size_t)-1);
					render.BindOutput(program, location, texture);
					break;
				default:
					break;
			}
		}

	protected:
		T value;
		VAR_TYPE varType;
		union {
			IRender::Buffer* buffer;
			IRender::Texture* texture;
		};
		IRender::Location location;

#ifdef _DEBUG
		bool valueAssigned;
#endif
	};
}

#endif // __TSHADERVARIABLE_H__
