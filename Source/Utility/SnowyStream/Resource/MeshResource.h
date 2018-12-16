// MeshResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __MESH_RESOURCE_H__
#define __MESH_RESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IAsset.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class MeshResource : public GraphicResourceBase {
		public:
			MeshResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual ~MeshResource();
			virtual uint64_t GetMemoryUsage() const;
			virtual bool operator << (IStreamBase& stream);
			virtual bool operator >> (IStreamBase& stream);
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			template <class T>
			void UpdateBuffer(IRender& render, IRender::Buffer*& buffer, T& bufferVec, IRender::BUFFER_TARGET bufferTarget = IRender::VERTEX) {
				if (buffer == nullptr && bufferVec.size() != 0) {
					buffer = render.CreateBuffer(bufferTarget);
					// assert(statusNotifier != nullptr);
					render.BindNotifierBuffer(statusNotifier, buffer);
					render.InitBufferDataEx(buffer, bufferVec);
				} else if (buffer != nullptr) {
					if (bufferVec.size() != 0) {
						render.UpdateBufferData(buffer, bufferVec);
					} else {
						render.UnBindNotifierBuffer(statusNotifier, buffer);
						render.DeleteBuffer(buffer);
						buffer = nullptr;
					}
				}
			}

			template <class T>
			void UpdateBuffers(IRender& render, std::vector<IRender::Buffer*>& buffers, T& bufferVecs, IRender::BUFFER_TARGET bufferTarget = IRender::VERTEX) {
				if (buffers.size() < bufferVecs.size()) {
					buffers.resize(bufferVecs.size(), nullptr);
				} else if (buffers.size() > bufferVecs.size()) {
					for (size_t j = bufferVecs.size(); j < buffers.size(); j++) {
						render.UnBindNotifierBuffer(statusNotifier, buffers[j]);
						render.DeleteBuffer(buffers[j]);
					}

					buffers.resize(bufferVecs.size());
				}

				assert(buffers.size() == bufferVecs.size());
				for (size_t i = 0; i < buffers.size(); i++) {
					UpdateBuffer(render, buffers[i], bufferVecs[i], bufferTarget);
				}
			}

		public:
			IAsset::MeshCollection meshCollection;
			struct BufferCollection {
				BufferCollection();
				IRender::Buffer* indexBuffer;
				IRender::Buffer* positionBuffer;
				IRender::Buffer* normalBuffer;
				IRender::Buffer* vertexColorBuffer;
				std::vector<IRender::Buffer*> texCoordBuffers;
			} bufferCollection;
		};
	}
}

#endif // __MESH_RESOURCE_H__