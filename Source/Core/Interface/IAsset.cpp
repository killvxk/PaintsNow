#include "IAsset.h"

using namespace PaintsNow;
using namespace PaintsNow::IAsset;

TObject<IReflect>& MeshCollection::operator () (IReflect& reflect) {
	ReflectClass(MeshCollection);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(vertices);
		ReflectProperty(normals);
		ReflectProperty(colors);
		ReflectProperty(texCoords);
		ReflectProperty(indices);
		ReflectProperty(meshes);
	}

	return *this;
}

void MeshCollection::CalulateNormals(ZFloat3* outputNormals, const ZFloat3* vertices, const ZInt3* faces, size_t vertexCount, size_t faceCount) {
	memset(outputNormals, 0, sizeof(ZFloat3) * vertexCount);
	for (size_t i = 0; i < faceCount; i++) {
		const ZInt3& index = faces[i];
		ZFloat3 normal = CrossProduct(vertices[index.y()] - vertices[index.x()], vertices[index.z()] - vertices[index.x()]);
		float length = normal.Length();
		if (length > 1e-5) {
			normal /= length;
			for (size_t j = 0; j < 3; j++) {
				outputNormals[index[j]] += normal;
			}
		}
	}

	for (size_t k = 0; k < vertexCount; k++) {
		ZFloat3& normal = outputNormals[k];
		float length = normal.Length();
		if (length > 1e-5) {
			normal /= length;
		}
	}
}

TObject<IReflect>& MeshSlice::operator () (IReflect& reflect) {
	ReflectClass(MeshSlice);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
		ReflectProperty(material);
		ReflectProperty(fromIndex);
		ReflectProperty(toIndex);
	}

	return *this;
}

TObject<IReflect>& MaterialCollection::operator () (IReflect& reflect) {
	ReflectClass(Material);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(materials);
	}

	return *this;
}

Material::Variable::Variable() {}
bool Material::Variable::WriteShaderVariable(IShaderVariableBase& var) const {
	static Unique strType = UniqueType<String>::Get();
	if (type != strType && type == var.GetSubUnique()) {
		assert(false); // TODO
		// memcpy(var.GetValue(), value.data(), value.size());
		return true;
	} else {
		return false;
	}
}

TObject<IReflect>& Material::Variable::operator () (IReflect& reflect) {
	ReflectClass(Material::Variable);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(key);
		ReflectProperty(value);
		ReflectProperty(type);
	}

	return *this;
}

TObject<IReflect>& Material::operator () (IReflect& reflect) {
	ReflectClass(Material);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(materialName);
		ReflectProperty(shaderName);
		ReflectProperty(variables);
		ReflectProperty(materialFlag);
	}

	return *this;
}

TObject<IReflect>& BoneAnimation::operator () (IReflect & reflect) {
	ReflectClass(BoneAnimation);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(joints);
		ReflectProperty(boneIndices);
		ReflectProperty(boneWeights);
	}

	return *this;
}

TObject<IReflect>& BoneAnimation::Clip::operator () (IReflect& reflect) {
	ReflectClass(BoneAnimation::Clip);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
		ReflectProperty(fps);
		ReflectProperty(duration);
		ReflectProperty(rarity);
		ReflectProperty(speed);
		ReflectProperty(loop);
		ReflectProperty(channels);
	}

	return *this;
}


TObject<IReflect>& BoneAnimation::Joint::operator () (IReflect& reflect) {
	ReflectClass(BoneAnimation::Joint);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
		ReflectProperty(offsetMatrix);
		ReflectProperty(id);
		ReflectProperty(parent);
	}

	return *this;
}

TObject<IReflect>& BoneAnimation::Channel::operator () (IReflect& reflect) {
	ReflectClass(BoneAnimation::Channel);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(jointIndex);
		ReflectProperty(rotSequence);
		ReflectProperty(transSequence);
		ReflectProperty(scalingSequence);
	}

	return *this;
}


TObject<IReflect>& TextureAnimation::Fragment::operator () (IReflect & reflect) {
	ReflectClass(TextureAnimation::Fragment);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(rotSequence);
		ReflectProperty(transSequence);
		ReflectProperty(scalingSequence);
	}

	return *this;
}

TObject<IReflect>& TextureAnimation::operator () (IReflect & reflect) {
	ReflectClass(TextureAnimation);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(fragments);
	}

	return *this;
}

TObject<IReflect>& TextureCollection::operator () (IReflect & reflect) {
	ReflectClass(TextureCollection);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(textures);
	}

	return *this;
}

TObject<IReflect>& TextureCollection::Texture::operator () (IReflect & reflect) {
	ReflectClass(TextureCollection::Texture);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
		ReflectProperty(replaceableId);
		ReflectProperty(wrapWidth);
		ReflectProperty(wrapHeight);
	}

	return *this;
}


/*
TObject<IReflect>& ParticleEmitterBasicCollection::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterBasicCollection);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(emitters);
	}

	return *this;
}

TObject<IReflect>& ParticleEmitterBasicCollection::ParticleEmitterBasic::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterBasicCollection::ParticleEmitterBasic);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(particleFileName);
		ReflectProperty(emissionRate);
		ReflectProperty(gravity);
		ReflectProperty(longitude);
		ReflectProperty(latitude);
		ReflectProperty(visibility);
		ReflectProperty(particleLifeSpan);
		ReflectProperty(particleInitialVelocity);

		ReflectProperty(emitterUsesModel);
		ReflectProperty(emitterUsesTexture);
	}

	return *this;
}

TObject<IReflect>& ParticleEmitterAdvancedCollection::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterAdvancedCollection);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(emitters);
	}

	return *this;
}

TObject<IReflect>& ParticleEmitterAdvancedCollection::ParticleEmitterAdvanced::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterAdvancedCollection::ParticleEmitterAdvanced);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(filterMode);
		ReflectProperty(speed);
		ReflectProperty(variation);
		ReflectProperty(latitude);
		ReflectProperty(gravity);
		ReflectProperty(visibility);
		ReflectProperty(emissionRate);
		ReflectProperty(width);
		ReflectProperty(length);

		ReflectProperty(segmentColor);
		ReflectProperty(alpha);
		ReflectProperty(particleScaling);
		ReflectProperty(headLifeSpan);
		ReflectProperty(headDecay);
		ReflectProperty(tailLifeSpan);
		ReflectProperty(tailDecay);

		ReflectProperty(rows);
		ReflectProperty(columns);
		ReflectProperty(textureId);
		ReflectProperty(priorityPlane);
		ReflectProperty(replaceableId);
		ReflectProperty(time);
		ReflectProperty(lifeSpan);
		ReflectProperty(tailLength);

		ReflectProperty(sortPrimitivesFarZ);
		ReflectProperty(lineEmitter);
		ReflectProperty(modelSpace);
		ReflectProperty(alphaKey);
		ReflectProperty(unshaded);
		ReflectProperty(unfogged);
		ReflectProperty(xyQuad);
		ReflectProperty(squirt);
		ReflectProperty(head);
		ReflectProperty(tail);

		ReflectProperty(currentEmission);
	}

	return *this;
}

TObject<IReflect>& ParticleEmitterRibbonCollection::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterRibbonCollection);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(emitters);
	}

	return *this;
}

TObject<IReflect>& ParticleEmitterRibbonCollection::ParticleEmitterRibbon::operator () (IReflect & reflect) {
	ReflectClass(ParticleEmitterRibbonCollection::ParticleEmitterRibbon);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(heightAbove);
		ReflectProperty(heightBelow);
		ReflectProperty(alpha);
		ReflectProperty(color);
		ReflectProperty(visibility);

		ReflectProperty(emissionRate);
		ReflectProperty(lifeSpan);
		ReflectProperty(gravity);
		ReflectProperty(rows);
		ReflectProperty(columns);
		ReflectProperty(materialId);
	}

	return *this;
}

TObject<IReflect>& Node::operator () (IReflect & reflect) {
	ReflectClass(Node);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
		ReflectProperty(rotSequence);
		ReflectProperty(transSequence);
		ReflectProperty(scalingSequence);
		ReflectProperty(pivot);
		ReflectProperty(id);
		ReflectProperty(parent);
		ReflectProperty(dontInheritTranslation);
		ReflectProperty(dontInheritRotation);
		ReflectProperty(dontInheritScaling);
		ReflectProperty(billboarded);
		ReflectProperty(billboardedLockX);
		ReflectProperty(billboardedLockY);
		ReflectProperty(billboardedLockZ);
		ReflectProperty(cameraAnchored);
	}

	return *this;
}*/

Texture::Texture() : width(0), height(0), layout(IRender::LAYOUT_RGBA), dataType(IRender::UNSIGNED_BYTE) {}

TObject<IReflect>& Texture::operator () (IReflect& reflect) {
	ReflectClass(Texture);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(width);
		ReflectProperty(height);
		ReflectProperty(layout);
		ReflectProperty(dataType);
		ReflectProperty(pixelData);
	}

	return *this;
}

TObject<IReflect>& Model::operator () (IReflect& reflect) {
	ReflectClass(Model);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(meshes);
		ReflectProperty(materials);
		ReflectProperty(textures);
		ReflectProperty(animations);
	}

	return *this;
}
