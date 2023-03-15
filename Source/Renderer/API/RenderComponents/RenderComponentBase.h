#pragma once

#include "../../Common/VexRenderCommon.h"

template<typename RenderComponentImpl>
class RenderComponentBase
{
public:
	RenderComponentBase() {};
	virtual ~RenderComponentBase() {};

	__forceinline void TranslateX(float dx) { m_position.x += dx; }
	__forceinline void TranslateY(float dy) { m_position.x += dy; }
	__forceinline void TranslateZ(float dz) { m_position.x += dz; }

	__forceinline void SetX(float x) { m_position.x = x; }
	__forceinline void SetY(float y) { m_position.x = y; }
	__forceinline void SetZ(float z) { m_position.x = z; }

	__forceinline XMFLOAT3 GetPosition() { return m_position; }
	__forceinline XMFLOAT3 GetRotation() { return m_rotation; }
	__forceinline XMFLOAT3 GetScale() { return m_scale; }

	__forceinline const XMFLOAT4X4& GetPositionMatrix() { return m_positionMatrix; };
	__forceinline const XMFLOAT4X4& GetRotationMatrix() { return m_rotationMatrix; };
	__forceinline const XMFLOAT4X4& GetScaleMatrix() { return m_scaleMatrix; };
	__forceinline const XMFLOAT4X4& GetModelMatrix() { return m_modelMatrix; };

	void BuildMatrices();

	// Interface functions
	void Render();


protected:
	XMFLOAT3 m_position = {};
	XMFLOAT3 m_rotation = {};
	XMFLOAT3 m_scale = {};

	XMFLOAT4X4 m_positionMatrix = {};
	XMFLOAT4X4 m_rotationMatrix = {};
	XMFLOAT4X4 m_scaleMatrix = {};
	XMFLOAT4X4 m_modelMatrix = {};
};