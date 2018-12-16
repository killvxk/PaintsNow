#include "TransformComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

void TransformComponent::LookAt(const ZFloat3& position, const ZFloat3& dir, const ZFloat3& u) {
	editor = nullptr;

	ZFloat3 direction = dir;
	ZFloat3 up = u;

	direction.Normalize();
	up.Normalize();

	/* Side = forward x up */
	ZFloat3 side = CrossProduct(direction, up);
	side.Normalize();
	up = CrossProduct(side, direction);

	ZMatrixFloat4x4 m;

	m(0, 0) = side[0];
	m(1, 0) = side[1];
	m(2, 0) = side[2];

	m(0, 1) = up[0];
	m(1, 1) = up[1];
	m(2, 1) = up[2];

	m(0, 2) = -direction[0];
	m(1, 2) = -direction[1];
	m(2, 2) = -direction[2];

	m(3, 0) = -position[0];
	m(3, 1) = -position[1];
	m(3, 2) = -position[2];

	localTransform = (ZMatrixFloat3x4)m;
	Flag() &= ~Tiny::TINY_MODIFIED;
}

TShared<TransformComponent::Editor>& TransformComponent::MakeEditor() {
	if (!editor) {
		editor = TShared<Editor>::Make();
		editor->rotation = Quaternion<float>((ZMatrixFloat4x4)localTransform);
		editor->translation = ZFloat3(localTransform(3, 0), localTransform(3, 1), localTransform(3, 2));
		editor->scale = ZFloat3(
			ZFloat3(localTransform(0, 0), localTransform(1, 0), localTransform(2, 0)).Length(),
			ZFloat3(localTransform(0, 1), localTransform(1, 1), localTransform(2, 1)).Length(),
			ZFloat3(localTransform(0, 2), localTransform(1, 2), localTransform(2, 2)).Length()
		);
	}

	return editor;
}

void TransformComponent::SetRotation(const ZFloat3& r) {
	MakeEditor()->rotation = Quaternion<float>(r);
	Flag() |= TINY_MODIFIED;
}

ZFloat3 TransformComponent::GetRotation() {
	return MakeEditor()->rotation.ToEulerAngle();
}

void TransformComponent::SetTranslation(const ZFloat3& t) {
	MakeEditor()->translation = t;
	Flag() |= TINY_MODIFIED;
}

const ZFloat3& TransformComponent::GetTranslation() {
	return MakeEditor()->translation;
}

ZFloat3 TransformComponent::GetQuickTranslation() const {
	return ZFloat3(localTransform(3, 0), localTransform(3, 1), localTransform(3, 2));
}

void TransformComponent::SetScale(const ZFloat3& s) {
	MakeEditor()->scale = s;
	Flag() |= TINY_MODIFIED;
}

const ZFloat3& TransformComponent::GetScale() {
	return MakeEditor()->scale;
}

ZMatrixFloat4x4 TransformComponent::GetLocalTransform() {
	UpdateTransform();
	return ZMatrixFloat4x4(localTransform);
}

void TransformComponent::UpdateTransform() {
	if (Flag() & Tiny::TINY_MODIFIED) {
		ZMatrixFloat4x4 rotMatrix;
		TShared<Editor>& editor = MakeEditor();
		editor->rotation.WriteMatrix(rotMatrix);
		const ZFloat3& scale = editor->scale;
		const ZFloat3& translation = editor->translation;

		float s[16] = {
			scale.x(), 0, 0, 0,
			0, scale.y(), 0, 0,
			0, 0, scale.z(), 0,
			translation.x(), translation.y(), translation.z(), 1
		};

		localTransform = ZMatrixFloat3x4(rotMatrix * ZMatrixFloat4x4(s));

		editor = nullptr;
		Flag() &= ~Tiny::TINY_MODIFIED;
	}
}