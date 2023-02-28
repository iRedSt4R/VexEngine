#pragma once

#include "../../Common/VexRenderCommon.h"
#include "DX12Resource.h"

class DX12Texture2D : public DX12ResourceBase
{
public:
	DX12Texture2D();
	~DX12Texture2D();

	void CreateFromFilepath(LPCWSTR filePath, bool bCreateSRV, bool bCreateUAV, bool bVCreateRTV, bool bCreateDSV);
	void CreateFromBinary(void* pDataBlob);

	__forceinline uint16_t GetWidth();
	__forceinline uint16_t GetHeight();

private:
	uint16_t m_Height;
	uint16_t m_Width;
	DXGI_FORMAT m_ImageFormat;
	bool m_bIsLinear;

};