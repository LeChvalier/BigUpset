#include "Camera.h"


void Camera::Update(float width, float height)
{
	// Projection matrix
	projMat = Mat4::Perspective(foy, width / height, near, far);

	viewMat = Mat4::RotateXYZ({ -pitch, -yaw, -roll }) * Mat4::Translate(-position);
}