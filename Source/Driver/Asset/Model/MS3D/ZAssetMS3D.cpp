#include "ZAssetMS3D.h"
#include "../../../../Core/Template/TAlgorithm.h"
#include <map>
#include <string>

using namespace PaintsNow;

#pragma pack(push, 1)

struct MS3D_FILE_HEADER
{
	char tag[10];
	uint32_t version;
};

#pragma pack(pop)

bool ZAssetMS3D::operator >> (IStreamBase& stream) const {
	// Not implemented.
	assert(false);
	return false;
}

bool ZAssetMS3D::operator << (IStreamBase& stream) {
	// printf("IN\n");
	// int totalLen = stream.GetRemaining();
	// load
	MS3D_FILE_HEADER header;
	stream >> header;

	typedef IAsset::MeshSlice Mesh;
	typedef IAsset::BoneAnimation::Joint Joint;
	typedef IAsset::Material Material;

	std::vector<ZInt3>& indexBuffer = meshes.indices;
	std::vector<ZFloat3>& vertexBuffer = meshes.vertices;
	std::vector<ZFloat3>& normalBuffer = meshes.normals;
	meshes.texCoords.push_back(std::vector<ZFloat4>());
	std::vector<ZFloat4>& texCoordBuffer = meshes.texCoords.back();

	std::vector<ZFloat4>& boneBuffer = animations.boneIndices;
	std::vector<ZFloat4>& weightBuffer = animations.boneWeights;
	std::vector<Joint>& jointBuffer = animations.joints;

	std::vector<Mesh>& meshBuffer = meshes.meshes;
	std::vector<Material>& materialBuffer = materials.materials;

	if (memcmp(header.tag, "MS3D000000", 10) != 0 || header.version != 4) {
		return false;
	}

	uint16_t i;
	uint16_t vertexCount;
	// Load vertices
	stream >> vertexCount;

	if ((signed)vertexCount <= 0)
		return false;

	char reserved;
	char b;
	ZFloat3 initNormal(0, 0, 1);
	ZFloat2 initTexCoord(0, 0);

	vertexBuffer.resize(vertexCount);
	normalBuffer.resize(vertexCount);
	texCoordBuffer.resize(vertexCount);
	boneBuffer.resize(vertexCount);
	weightBuffer.resize(vertexCount);

	for (i = 0; i < vertexCount; i++) {
		ZFloat3 vertex;
		stream >> reserved;
		stream >> vertex;
		vertexBuffer[i] = vertex;
		stream >> b;
		stream >> reserved;
		ZFloat4 bone(b, 0, 0, 0);
		ZFloat4 weight(1, 0, 0, 0);
		boneBuffer[i] = bone;
		weightBuffer[i] = weight;
	}

	std::vector<bool> exist(vertexBuffer.size(), false);

	// Load faces
	uint16_t faceCount;
	stream >> faceCount;

	if ((signed)faceCount <= 0)
		return false;

	unsigned short res;
	uint16_t indices[3];
	ZFloat3 normals[3];
	ZFloat2 texCoords[3];
	std::vector<ZInt3> facePrepare;
	facePrepare.resize(faceCount);

	for (i = 0; i < faceCount; i++) {
		stream >> res;
		for (int j = 0; j < 3; j++)
			stream >> indices[j];

		for (int k = 0; k < 3; k++)
			stream >> normals[k];

		for (int m = 0; m < 3; m++)
			stream >> texCoords[m].x();

		for (int n = 0; n < 3; n++)
			stream >> texCoords[n].y();

		// check if we need a newer vertex
		for (int w = 0; w < 3; w++) {
			if (indices[w] > exist.size())
				indices[w] = 0;

			ZFloat4 tex(texCoords[w].x(), 1 - texCoords[w].y(), 0, 0);
			if (exist[indices[w]]) {
				// already exists, check if it's the same
				if (!(tex == texCoordBuffer[indices[w]]) || !(normals[w] == normalBuffer[w])) {
					vertexBuffer.push_back(vertexBuffer[indices[w]]);
					normalBuffer.push_back(normals[w]);
					texCoordBuffer.push_back(tex);
					indices[w] = vertexCount++;
				}
			} else {
				normalBuffer[indices[w]] = normals[w];
				texCoordBuffer[indices[w]] = tex;
			}
		}

		facePrepare[i] = ZInt3(indices[0], indices[1], indices[2]);

		stream >> reserved;
		stream >> reserved;
	}

	// Load meshes
	uint16_t meshCount;
	stream >> meshCount;

	if ((signed)meshCount <= 0)
		return false;
	
	enum { MESH_NAME_LEN = 32, MAT_NAME_LEN = 32, TEX_NAME_LEN = 128, ALPHA_TEX_NAME_LEN = 128, JOINT_NAME_LEN = 32 };
	char name[256];
	meshBuffer.resize(meshCount);
	for (i = 0; i < meshCount; i++) {
		Mesh& mesh = meshBuffer[i];
		stream >> reserved;
		size_t len = MESH_NAME_LEN;
		stream.Read(name, len);
		name[len] = 0;
		mesh.name = name;
		uint16_t indexCount;
		stream >> indexCount;

		unsigned short id;
		mesh.fromIndex = (uint32_t)indexBuffer.size();
		for (size_t j = 0; j < indexCount; j++) {
			stream >> id;
			indexBuffer.push_back(facePrepare[id]);
		}

		mesh.toIndex = (uint32_t)indexBuffer.size();
		uint8_t mat;
		stream >> mat;
		mesh.material = mat;
	}

	// Load materials
	uint16_t materialCount;
	stream >> materialCount;

	if ((signed)materialCount <= 0)
		return false;

	std::map<String, int> mapPathToID;
	materialBuffer.resize(materialCount);
	for (i = 0; i < materialCount; i++) {
		Material& m = materialBuffer[i];
		
		size_t len = MAT_NAME_LEN;
		stream.Read(name, len);

		if (name[0] != '\0')
			m.materialName = name;
		else
			m.materialName = "StandardShaderResource";

		ZFloat4 ambient, albedo, specular, emissive;
		float transparent, shininess;
		stream >> ambient;
		stream >> albedo;
		stream >> specular;
		stream >> emissive;
		stream >> shininess;
		stream >> transparent;

		m.variables.push_back(Material::Variable("AmbientColor", ambient));
		m.variables.push_back(Material::Variable("SpecularColor", specular));
		m.variables.push_back(Material::Variable("EmissiveColor", emissive));
		m.variables.push_back(Material::Variable("Shininess", shininess));

		if (transparent != 0) {
			m.materialFlag |= Material::MATERIAL_TRANSPARENT;
		}

		stream >> reserved;

		len = TEX_NAME_LEN;
		stream.Read(name, len);
		// layer.texture = name;

		if (name[0] != '\0' && mapPathToID.count(name) == 0) {
			mapPathToID[name] = (int)mapPathToID.size();
			textures.textures.push_back(IAsset::TextureCollection::Texture());
			IAsset::TextureCollection::Texture& texture = textures.textures.back();
			texture.name = name;
			texture.wrapWidth = false;
			texture.wrapHeight = false;
		}

		m.variables.push_back(Material::Variable("AlbedoTexture", name));
		
		len = ALPHA_TEX_NAME_LEN;
		stream.Read(name, len);
		m.variables.push_back(Material::Variable("AlphaTexture", name));
		materialBuffer.push_back(m);
	}

	uint32_t total;
	float a, c;
	stream >> a;
	stream >> c;
	stream >> total;

	uint16_t jointCount;
	stream >> jointCount;
	std::map<String, int> mapJoint;
	std::vector<String> parentNames(jointCount);

	animations.clips.push_back(IAsset::BoneAnimation::Clip());
	IAsset::BoneAnimation::Clip& clip = animations.clips.back();
	clip.name = "default";
	clip.loop = true;
	clip.fps = a;
	// currentTime = c;
	clip.duration = 0;

	for (i = 0; i < jointCount; i++) {
		Joint j;
		IAsset::BoneAnimation::Channel channel;
		channel.jointIndex = (int)i;
		channel.transSequence.interpolate = channel.rotSequence.interpolate = channel.scalingSequence.interpolate = IAsset::INTERPOLATE_LINEAR;
		stream >> reserved;
		
		size_t len = JOINT_NAME_LEN;
		stream.Read(name, len);
		j.name = name;
		stream.Read(name, len);
		parentNames[i] = name;
		ZFloat3 rotation, pivot;
		stream >> rotation >> pivot;

		typedef Quaternion<float> Quater;
		Quater q = rotation;
		ZFloat3 t = pivot;
		ZMatrixFloat4x4 mat;
		q.WriteMatrix(mat);
		mat = mat.Transpose();
		mat(3, 0) = t.x();
		mat(3, 1) = t.y();
		mat(3, 2) = t.z();

		j.parent = -1;
		j.offsetMatrix = mat;

		uint16_t rotFrameCount;
		uint16_t transFrameCount;
		stream >> rotFrameCount;
		stream >> transFrameCount;

		for (size_t k = 0; k < rotFrameCount; k++) {
			IAsset::RotSequence::Frame f;
			ZFloat3 value;
			stream >> f.time;
			stream >> value;
			f.value = Quaternion<float>(value);
			channel.rotSequence.frames.push_back(f);
			clip.duration = clip.duration > f.time ? clip.duration : f.time;
		}

		for (size_t m = 0; m < transFrameCount; m++) {
			IAsset::TransSequence::Frame f;
			ZFloat3 value;
			stream >> f.time;
			stream >> value;
			f.value = ZFloat3(value.x(), value.y(), value.z());
			channel.transSequence.frames.push_back(f);
			
			clip.duration = clip.duration > f.time ? clip.duration : f.time;
		}

		mapJoint[j.name] = i;
		
		jointBuffer.push_back(j);
		clip.channels.push_back(channel);
	}

	for (i = 0; i < jointCount; i++) {
		std::map<String, int>::const_iterator p = mapJoint.find(parentNames[i]);
		if (p != mapJoint.end())
			jointBuffer[i].parent = p->second;
	}

	clip.name = "DefaultClip";

	return true;
}

TObject<IReflect>& ZAssetMS3D::operator () (IReflect & reflect) {
	ReflectClass(ZAssetMS3D);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(meshes);
		ReflectProperty(textures);
		ReflectProperty(animations);
		ReflectProperty(materials);
	}

	return *this;
}
