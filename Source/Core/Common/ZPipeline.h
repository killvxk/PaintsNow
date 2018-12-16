// ZPipeline -- Render pipeline
// By PaintDream (paintdream@paintdream.com)
// 2014-12-3
//

#ifndef __ZPIPELINE_H__
#define __ZPIPELINE_H__

#include "../Interface/IShader.h"
#include "../Interface/IReflect.h"
#include "../Template/TEvent.h"

namespace PaintsNow {
	// New version of ZPipeline !
	class ZPipeline : public IReflectObjectComplex {
	public:
		TEvent<IRender&, ZPipeline&> eventFireRender;

	public:
		ZPipeline();
		virtual TObject<IReflect>& operator () (IReflect& reflect);
		virtual ~ZPipeline();

		bool Compile(IRender& render);
		bool IsCompiled() const;
		void Attach(IRender& render, IRender::Target* renderTarget);
		void Detach(IRender& render, IRender::Target* renderTarget);
		void FireRender(IRender& render);
		void FireRenderCompacted(IRender& render, const String& dataBuffer);
		void BindIndexBuffer(IRender& render, IRender::Buffer* indexBuffer, uint32_t from, uint32_t to = 0, uint32_t instanceCount = 0);
		IRender::Buffer* GetIndexBuffer() const;
		void GetIndexRange(uint32_t& from, uint32_t& to) const;
		IRender::Program* GetProgram() const;
		void Cleanup(IRender& render);
		bool UpdateArgsHandlers(bool skipOutputBindings);

		struct ArgumentsHandler {
			IShaderVariableBase* var;
			uint32_t offset; // get from reflection
			uint32_t dataLength; // cache from var
			IShaderVariableBase::Binder binder; // cache from var
			IShaderVariableBase::Importer importer; // cache from var string
			IShaderVariableBase::Exporter exporter; // cache from var string
		};

		std::vector<ArgumentsHandler> argsHandler;
		std::map<String, size_t> argsMap;

	protected:
		void UpdateParams(IRender& render);

	protected:
		IRender::Program* program;
		IRender::Buffer* indexBuffer;
		uint32_t fromIndex, toIndex, instanceCount;
	};
}

#endif // __ZPIPELINE_H__
