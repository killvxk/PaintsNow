// TranformComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-4
//

#ifndef __TRANSFORMCOMPONENT_H__
#define __TRANSFORMCOMPOENNT_H__

#include "../../Entity.h"
#include "../../Component.h"
#include "../../../../Core/Common/ZType.h"

namespace PaintsNow {
	namespace NsMythForest {
		class TransformComponent : public Component {
		public:
			ZFloat3 GetQuickTranslation() const;

			void SetRotation(const ZFloat3& rotation);
			ZFloat3 GetRotation();
			void SetTranslation(const ZFloat3& translation);
			const ZFloat3& GetTranslation();
			void SetScale(const ZFloat3& scale);
			const ZFloat3& GetScale();
			void UpdateTransform();
			void LookAt(const ZFloat3& position, const ZFloat3& direction, const ZFloat3& up);

			ZMatrixFloat4x4 GetLocalTransform();

		protected:
			class Editor : public SharedTiny {
			public:
				Quaternion<float> rotation;
				ZFloat3 scale;
				ZFloat3 translation;
			};

			TShared<Editor>& MakeEditor();

		protected:
			ZMatrixFloat3x4 localTransform;
			TShared<Editor> editor; // 8 + 12 * 4 + 4 + 4 = 64 bytes on 32-bit platform
		};
	}
}


#endif // __TRANSFORMCOMPONENT_H__