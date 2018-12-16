// SkeletonResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __SKELETON_RESOURCE_H__
#define __SKELETON_RESOURCE_H__

#include "MeshResource.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class SkeletonResource : public MeshResource {
		public:
			SkeletonResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual bool operator << (IStreamBase& stream);
			virtual bool operator >> (IStreamBase& stream);
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
			void Seek(double time);
			float GetDuration() const;

		protected:
			void PrepareTransform(size_t i);
			void PrepareLocal(size_t i);

		protected:
			typedef IAsset::MeshCollection MeshCollection;
			typedef IAsset::MaterialCollection MaterialCollection;
			typedef IAsset::Material Material;
			typedef IAsset::BoneAnimation BoneAnimation;
			typedef IAsset::BoneAnimation::Joint Joint;
			BoneAnimation boneAnimation;
			std::vector<size_t> lastTrans;
			std::vector<size_t> lastRots;
			std::vector<size_t> lastScalings;
			std::vector<ZMatrixFloat4x4> transforms;
			std::vector<ZMatrixFloat4x4> orglocals;
			std::vector<ZMatrixFloat4x4> locals;
			std::vector<ZMatrixFloat4x4> invs;

			IRender::Buffer* boneIndexBuffer;
			IRender::Buffer* boneWeightBuffer;
			IRender::Buffer* boneMatrixBuffer;
			uint32_t currentClip;
			float currentTime;
			bool indexModified;
			bool transformModified;
		};
	}
}

#endif // __SKELETON_RESOURCE_H__