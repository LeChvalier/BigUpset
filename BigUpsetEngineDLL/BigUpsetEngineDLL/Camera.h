#pragma once

#include "Maths.h"
#include "Define.h"

class Camera
{
public:
	// Position
	float3 position = { 0.f, 0.3f, 1.5f };

	// Rotation
	float pitch = 0.f, yaw = 0.f, roll = 0.f;

	// Near and Far
	float near = NEAR, far = FAR, foy = FOY;

	// Matrix
	mat4 projMat, viewMat = Mat4::Identity();

	float normalSpeed = 2.f;
	float fastSpeed = 8.f;

	// Update ratio
	void Update(float width = 1200.f, float height = 800.f);
};