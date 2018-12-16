#include "SkeletonResource.h"
#include "../../../Core/Template/TAlgorithm.h"
#include <cassert>

using namespace PaintsNow;

const double EPSILON = 1e-7;
typedef Quaternion<float> Quater;

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

SkeletonResource::SkeletonResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : MeshResource(manager, uniqueID), indexModified(true), transformModified(true) {}

uint64_t SkeletonResource::GetMemoryUsage() const {
	return 0;
}

bool SkeletonResource::operator << (IStreamBase& stream) {
	return false;
}

bool SkeletonResource::operator >> (IStreamBase& stream) {
	return false;
}

void SkeletonResource::Attach(IRender& render) {
	MeshResource::Attach(render);
}

void SkeletonResource::Detach(IRender& render) {
	MeshResource::Detach(render);
}

void SkeletonResource::Upload(IRender& render) {
	Flag() &= ~RESOURCE_UPLOADED;

	if (indexModified) {
		UpdateBuffer(render, boneIndexBuffer, boneAnimation.boneIndices);
		UpdateBuffer(render, boneWeightBuffer, boneAnimation.boneWeights);
		indexModified = false;
	}

	if (transformModified) {
		UpdateBuffer(render, boneMatrixBuffer, transforms);
		transformModified = false;
	}
}

void SkeletonResource::Download(IRender& render) {

}

float SkeletonResource::GetDuration() const {
	if (boneAnimation.clips.size() == 0) {
		return 0.0f;
	}

	return boneAnimation.clips[currentClip].duration;
}

void SkeletonResource::PrepareTransform(size_t k) {
	const Joint& joint = boneAnimation.joints[k];
	int parent = joint.parent;
	if (parent != -1) {
		transforms[k] = transforms[k] * transforms[parent];
	}
}


void SkeletonResource::PrepareLocal(size_t k) {
	const Joint& joint = boneAnimation.joints[k];
	int parent = joint.parent;

	ZMatrixFloat4x4 mat = joint.offsetMatrix;
	locals[k] = orglocals[k] = mat;

	// mult all parents
	if (parent != -1) {
		invs[k] = locals[k] = locals[k] * locals[parent];
	} else {
		invs[k] = locals[k];
	}

	invs[k] = Inverse(invs[k]); // shouldn't it be invs[k] = locals[k].Inverse() ?
}


void SkeletonResource::Seek(double time) {
	if (boneAnimation.clips.size() == 0) {
		return;
	}

	const IAsset::BoneAnimation::Clip& clip = boneAnimation.clips[currentClip];
	time = fmod(time, clip.duration);

	for (size_t c = 0; c < clip.channels.size(); c++) {
		const IAsset::BoneAnimation::Channel& channel = clip.channels[c];
		int a = channel.jointIndex;
		const Joint* joint = &boneAnimation.joints[a];

		// process positions
		ZFloat3 presentPosition;

		if (channel.transSequence.frames.size() != 0) {
			size_t frame = (time >= currentTime) ? lastTrans[a] : 0;
			while (frame < channel.transSequence.frames.size() - 1) {
				if (time < channel.transSequence.frames[frame + 1].time)
					break;
				frame++;
			}

			// interpolate between this frame and the next one.

			int nextFrame = (frame + 1) % (int)channel.transSequence.frames.size();
			const IAsset::TransSequence::Frame& key = channel.transSequence.frames[frame];
			const IAsset::TransSequence::Frame& nextKey = channel.transSequence.frames[nextFrame];
			float diffTime = nextKey.time - key.time;

			if (diffTime < 0)
				diffTime += clip.duration;

			if (diffTime > 0) {
				float factor = ((float)time - key.time) / diffTime;
				// linear interpolation
				float factor1;
				float factor2;
				float factor3;
				float factor4;
				float factorTimesTwo;
				float inverseFactorTimesTwo;
				float inverseFactor = 1.0f - factor;

				switch (channel.transSequence.interpolate) {
					case IAsset::INTERPOLATE_NONE:
						presentPosition = key.value;
						break;
					case IAsset::INTERPOLATE_LINEAR:
						presentPosition = key.value + (nextKey.value - key.value) * factor;
						break;
					case IAsset::INTERPOLATE_HERMITE:
						factorTimesTwo = factor * factor;

						factor1 = factorTimesTwo * (2.0f * factor - 3.0f) + 1;
						factor2 = factorTimesTwo * (factor - 2.0f) + factor;
						factor3 = factorTimesTwo * (factor - 1.0f);
						factor4 = factorTimesTwo * (3.0f - 2.0f * factor);
						presentPosition = key.value * factor1 + key.outTan * factor2 + nextKey.inTan * factor3 + nextKey.outTan * factor4;
						break;
					case IAsset::INTERPOLATE_BEZIER:
						factorTimesTwo = factor * factor;
						inverseFactorTimesTwo = inverseFactor * inverseFactor;

						factor1 = inverseFactorTimesTwo * inverseFactor;
						factor2 = 3.0f * factor * inverseFactorTimesTwo;
						factor3 = 3.0f * factorTimesTwo * inverseFactor;
						factor4 = factorTimesTwo * factor;

						presentPosition = key.value * factor1 + key.outTan * factor2 + nextKey.inTan * factor3 + nextKey.outTan * factor4;
						break;
				}
			} else {
				presentPosition = key.value;
			}

			lastTrans[a] = frame;
		}

		// process rotation
		Quater presentRotation;

		if (channel.rotSequence.frames.size() != 0) {
			size_t frame = (time >= currentTime) ? lastRots[a] : 0;
			while (frame < channel.rotSequence.frames.size() - 1) {
				if (time < channel.rotSequence.frames[frame + 1].time)
					break;
				frame++;
			}

			// interpolate between this frame and the next one.

			int nextFrame = (frame + 1) % (int)channel.rotSequence.frames.size();
			const IAsset::RotSequence::Frame& key = channel.rotSequence.frames[frame];
			const IAsset::RotSequence::Frame& nextKey = channel.rotSequence.frames[nextFrame];
			float diffTime = nextKey.time - key.time;

			if (diffTime < 0)
				diffTime += clip.duration;
			if (diffTime > EPSILON) {
				float factor = ((float)time - key.time) / diffTime;
				// linear interpolation
				switch (channel.rotSequence.interpolate) {
					case IAsset::INTERPOLATE_NONE:
						presentRotation = key.value;
						break;
					case IAsset::INTERPOLATE_LINEAR:
						Quater::Interpolate(presentRotation, Quater(key.value), Quater(nextKey.value), factor);
						break;
					case IAsset::INTERPOLATE_HERMITE:
					case IAsset::INTERPOLATE_BEZIER:
						Quater::InterpolateSquad(presentRotation, Quater(key.value), Quater(key.outTan), Quater(nextKey.value), Quater(nextKey.inTan), factor);
						break;
				}
			} else {
				presentRotation = key.value;
			}


			lastRots[a] = frame;
		}

		// process scaling
		ZFloat3 presentScaling(1, 1, 1);

		if (channel.scalingSequence.frames.size() != 0) {
			size_t frame = (time >= currentTime) ? lastScalings[a] : 0;
			while (frame < channel.scalingSequence.frames.size() - 1) {
				if (time < channel.scalingSequence.frames[frame + 1].time)
					break;
				frame++;
			}

			presentScaling = channel.scalingSequence.frames[frame].value;
			lastScalings[a] = frame;
		}

		ZMatrixFloat4x4 mat;
		presentRotation.WriteMatrix(mat);
		mat = mat.Transpose();

		for (int i = 0; i < 3; i++)
			mat(0, i) *= presentScaling.x();
		for (int j = 0; j < 3; j++)
			mat(1, j) *= presentScaling.y();
		for (int k = 0; k < 3; k++)
			mat(2, k) *= presentScaling.z();

		mat(3, 0) = presentPosition.x();
		mat(3, 1) = presentPosition.y();
		mat(3, 2) = presentPosition.z();
		transforms[a] = mat * orglocals[a];
	}

	// make transforms from top to end

	for (size_t k = 0; k < boneAnimation.joints.size(); k++) {
		PrepareTransform(k);
	}

	for (size_t d = 0; d < boneAnimation.joints.size(); d++) {
		transforms[d] = invs[d] * transforms[d];
	}

	currentTime = (float)time;

//	render.UpdateBufferData(boneMatrixBuffer, transforms);
	transformModified = true;
}