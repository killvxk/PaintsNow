// IAsset.h -- Asset data exchangement
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __IASSET_H__
#define __IASSET_H__


#include "../PaintsNow.h"
#include "IFilterBase.h"
#include "../Template/TShaderVariable.h"
#include "../Common/ZType.h"
#include "../Interface/IRender.h"
#include <list>
#include <vector>

namespace PaintsNow {
	class IShaderVariableBase;
	namespace IAsset {
		class MeshSlice : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			String name;
			uint32_t material;
			uint32_t fromIndex;
			uint32_t toIndex;
		};

		class MeshCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			std::vector<ZFloat3> vertices;
			std::vector<ZFloat3> normals;
			std::vector<ZFloat4> colors;
			std::vector<std::vector<ZFloat4> > texCoords;
			std::vector<ZInt3> indices;
			std::vector<MeshSlice> meshes;

			static void CalulateNormals(ZFloat3* outputNormals, const ZFloat3* vertices, const ZInt3* faces, size_t vertexCount, size_t faceCount);
		};


		enum INTERPOLATE { INTERPOLATE_NONE, INTERPOLATE_LINEAR, INTERPOLATE_HERMITE, INTERPOLATE_BEZIER };
		enum FILTER { FILTER_NONE, FILTER_TRANSPARENT, FILTER_BLEND, FILTER_ADDITIVE, FILTER_ADD_ALPHA, FILTER_MODULATE };

		template <class T>
		class InterpolateValue {
		public:
			T value;
			T inTan;
			T outTan;
			float time;
		};

		template <class T>
		class Sequence : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect) {
				ReflectClass(Sequence<T>);
				if (reflect.IsReflectProperty()) {
					ReflectProperty(frames);
					ReflectProperty(interpolate);
				}

				return *this;
			}

#ifdef _MSC_VER
			typedef typename InterpolateValue<T> Frame;
#else
			typedef InterpolateValue<T> Frame;
#endif
			std::vector<Frame> frames;
			Frame staticFrame;
			INTERPOLATE interpolate;
		};

		class Material : public IReflectObjectComplex {
		public:
			class Variable : public IReflectObjectComplex {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				Variable();

				bool WriteShaderVariable(IShaderVariableBase& base) const;

				template <class T>
				Variable(const String& k, const T& value) {
					key = k;
					*this = value;
				}
				
				template <class T>
				Variable& operator = (const T& object) {
					type = UniqueType<T>::Get();
					value.resize(sizeof(T));
					memcpy(const_cast<char*>(value.data()), &object, sizeof(T));

					return *this;
				}

#ifdef _MSC_VER
				template <>
#endif
				Variable& operator = (const String& object) {
					type = UniqueType<String>::Get();
					value = object;
					return *this;
				}

				String key;
				String value;
				Unique type;
			};

			typedef uint32_t MATERIAL_FLAG;
			enum {
				MATERIAL_TRANSPARENT = 1 << 0,
				MATERIAL_CLIPPING = 1 << 1,
				MATERIAL_ZWRITE = 1 << 2,
				MATERIAL_REFLECTNESS = 1 << 3,
				MATERIAL_REFRACTNESS = 1 << 4,
				MATERIAL_SCREEN = 1 << 5,
				MATERIAL_MAX = 1 << 31
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			String materialName;
			String shaderName;
			std::vector<Variable> variables;
			MATERIAL_FLAG materialFlag;
		};

		class MaterialCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			std::vector<Material> materials;
		};

		typedef Sequence<ZFloat4> RotSequence;
		typedef Sequence<ZFloat3> TransSequence;
		typedef Sequence<ZFloat3> ScalingSequence;

		class TextureAnimation : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			class Fragment : public IReflectObjectComplex {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				typedef Sequence<ZFloat4> RotSequence;
				typedef Sequence<ZFloat3> TransSequence;
				typedef Sequence<ZFloat3> ScalingSequence;
				RotSequence rotSequence;
				TransSequence transSequence;
				ScalingSequence scalingSequence;
			};

			std::vector<Fragment> fragments;
		};


		class BoneAnimation : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			class Joint : public IReflectObjectComplex {
			public:

				virtual TObject<IReflect>& operator () (IReflect& reflect);

				String name;
				ZMatrixFloat4x4 offsetMatrix;

				int id;
				int parent;
			};

			class Channel : public IReflectObjectComplex {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				int jointIndex;
				RotSequence rotSequence;
				TransSequence transSequence;
				ScalingSequence scalingSequence;
			};

			class Clip : public IReflectObjectComplex {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				String name;
				float fps;
				float duration;
				float rarity;
				float speed;
				bool loop;

				std::vector<Channel> channels;
			};

			std::vector<Joint> joints;
			std::vector<Clip> clips;
			std::vector<ZFloat4> boneIndices;
			std::vector<ZFloat4> boneWeights;
		};

		class TextureCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			class Texture : public IReflectObjectComplex {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);

				String name;
				size_t replaceableId;
				bool wrapWidth;
				bool wrapHeight;
			};

			std::vector<Texture> textures;
		};
		/*
		class ParticleEmitterBasicCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			class ParticleEmitterBasic : public Node {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				typedef Sequence<float> ValueType;

				String particleFileName;
				float emissionRate;
				float gravity;
				float longitude;
				float latitude;
				ValueType visibility;
				float particleLifeSpan;
				float particleInitialVelocity;

				bool emitterUsesModel;
				bool emitterUsesTexture;
			};

			std::vector<ParticleEmitterBasic> emitters;
		};

		class ParticleEmitterAdvancedCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			class ParticleEmitterAdvanced : public Node {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);
				typedef Sequence<float> ValueType;
				FILTER filterMode;

				float variation;
				float gravity;
				ValueType speed;
				ValueType latitude;
				ValueType visibility;
				ValueType emissionRate;
				ValueType width;
				ValueType length;

				std::vector<ZFloat3> segmentColor;
				ZFloat3 alpha;
				ZFloat3 particleScaling;
				ZFloat3 headLifeSpan;
				ZFloat3 headDecay;
				ZFloat3 tailLifeSpan;
				ZFloat3 tailDecay;

				int rows;
				int columns;
				int textureId;
				int priorityPlane;
				int replaceableId;
				float time;
				float lifeSpan;
				float tailLength;

				bool sortPrimitivesFarZ;
				bool lineEmitter;
				bool modelSpace;
				bool alphaKey;
				bool unshaded;
				bool unfogged;
				bool xyQuad;
				bool squirt;
				bool head;
				bool tail;

				float currentEmission;
			};

			std::vector<ParticleEmitterAdvanced> emitters;
		};

		class ParticleEmitterRibbonCollection : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			class ParticleEmitterRibbon : public Node {
			public:
				virtual TObject<IReflect>& operator () (IReflect& reflect);

				typedef Sequence<float> ValueType;

				ValueType heightAbove;
				ValueType heightBelow;
				ValueType visibility;

				float alpha;
				ZFloat3 color;

				float emissionRate;
				float lifeSpan;
				float gravity;
				int rows;
				int columns;
				int materialId;
			};

			std::vector<ParticleEmitterRibbon> emitters;
		};
		*/

		// Collection?
		class Resource : public IReflectObjectComplex {};
		class Model : public Resource {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			MeshCollection meshes;
			MaterialCollection materials;
			TextureCollection textures;
			BoneAnimation animations;
		};

		class Texture : public Resource {
		public:
			Texture();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			uint32_t width;
			uint32_t height;
			IRender::LAYOUT_TYPE layout;
			IRender::DATA_TYPE dataType;
			String pixelData;
		};
	}
}


#endif // __IASSET_H__
