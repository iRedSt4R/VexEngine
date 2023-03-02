#pragma once
#include "CameraBase.h"

class FPSCamera : public CameraBase
{
public:
	FPSCamera(uint32_t width, uint32_t height);
	~FPSCamera();

	void Create() override;
	void Update(uint32_t width, uint32_t height, int* mouseDeltaX, int* mouseDeltaY) override;

private:
	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;
	XMVECTOR camForward;
	XMVECTOR camRight;

	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;

	float moveLeftRight;
	float moveBackForward;

	float camYaw;
	float camPitch;

	// Mouse
	POINT cursorOldPos;
	POINT cursorCurPos;

};