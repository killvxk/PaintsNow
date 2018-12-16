// ZBatchRanger.h
// PaintDream (paintdream@paintdream.com)
// 2018-8-13
//

#ifndef __ZBATCHRANGER_H__
#define __ZBATCHRANGER_H__

#include "ZPipeline.h"
#include <list>

namespace PaintsNow {
	// TODO: use UBO for unmerged data
	class ZBatchRanger {
	public:
		typedef IShaderVariableBase::VAR_TYPE VAR_TYPE;
		typedef ZPipeline::ArgumentsHandler ArgumentsHandler;
		struct DrawCall {
			DrawCall();
			IRender::Buffer* indexBuffer;
			uint32_t instanceCount;
			uint32_t from, to;
			String dataBuffer;
		};

		struct CommandCapture : public DrawCall {
			CommandCapture();
			bool operator < (const CommandCapture& lhs) const;
			bool operator == (const CommandCapture& rhs) const;

			uint32_t hashValue;
			String unmergableData;
		};

		struct CommandQueue {
			// render data
			CommandQueue() : batchedTotalDataLength(0) {}
			~CommandQueue() { assert(managedBuffers.empty()); }
			uint32_t batchedTotalDataLength;
			std::vector<DrawCall> drawCalls;
			std::vector<IRender::Buffer*> managedBuffers;
			std::vector<uint32_t> mergedIndices;
			std::vector<std::pair<ArgumentsHandler, uint32_t> > mergableDataHandlers;
			std::vector<std::pair<ArgumentsHandler, uint32_t> > unmergableDataHandlers;
		};

		struct SortCapturePointers {
			bool operator () (const CommandCapture* lhs, const CommandCapture* rhs) {
				return *lhs < *rhs;
			}
		};

		ZBatchRanger(ZPipeline& pipeline);
		virtual ~ZBatchRanger();
		void DoCapture(IRender& render, CommandCapture& commandCapture) const;
		bool CompilePipeline(IRender& render, std::vector<CommandCapture*> sortedCommandCaptures, ZPipeline& batchedPipeline) const;
		void CompileQueue(IRender& render, CommandQueue& commandQueue, ZPipeline& batchedPipeline) const;
		void CommitQueueCaptures(IRender& render, CommandQueue& commandQueue, std::vector<CommandCapture*> sortedCommandCaptures, ZPipeline& batchedPipeline) const;
		void InvokeRenderQueue(IRender& render, CommandQueue& commandQueue, ZPipeline& batchedPipeline) const;
		void CleanupQueue(IRender& render, CommandQueue& commandQueue) const;

	protected:
		static void CleanupBuffers(IRender& render, std::vector<IRender::Buffer*>& buffers);
		static void WriteMergableDataBuffer(String& target, IRender& render, std::vector<IRender::Buffer*>& buffers, const std::vector<String>& mergedData, const String& dataBuffer, const std::vector<std::pair<ArgumentsHandler, uint32_t> >& mergableDataHandlers);
		static void WriteUnmergableDataBuffer(String& target, const String& unmergedData, const std::vector<std::pair<ArgumentsHandler, uint32_t> >& unmergableDataHandlers);

	protected:
		// eternal
		ZPipeline& basePipeline;
		std::vector<std::pair<ArgumentsHandler, uint32_t> > mergableDataHandlers;
		std::vector<std::pair<ArgumentsHandler, uint32_t> > unmergableDataHandlers;
	};
}


#endif // __ZBATCHRANGER_H__