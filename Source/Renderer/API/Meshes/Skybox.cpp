#include "Skybox.h"

void Skybox::CreateSkyboxSphere(ID3D12GraphicsCommandList* cmdList, std::wstring pathToDDSCubemap, int latLines, int longLines)
{
	m_numSphereVertices = ((latLines - 2) * longLines) + 2;
	m_numSphereFaces = ((latLines - 3) * (longLines) * 2) + (longLines * 2);


	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	// generate sphere vertices
	std::vector<Vertex> vertices(m_numSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Position.x = 0.0f;
	vertices[0].Position.y = 0.0f;
	vertices[0].Position.z = 1.0f;

	for (DWORD i = 0; i < latLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (latLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < longLines; ++j)
		{
			sphereYaw = j * (6.28 / (longLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * longLines + j + 1].Position.x = XMVectorGetX(currVertPos);
			vertices[i * longLines + j + 1].Position.y = XMVectorGetY(currVertPos);
			vertices[i * longLines + j + 1].Position.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[m_numSphereVertices - 1].Position.x = 0.0f;
	vertices[m_numSphereVertices - 1].Position.y = 0.0f;
	vertices[m_numSphereVertices - 1].Position.z = -1.0f;

	// generate sphere indices
	std::vector<DWORD> indices(m_numSphereFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < longLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = longLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < latLines - 3; ++i)
	{
		for (DWORD j = 0; j < longLines - 1; ++j)
		{
			indices[k] = i * longLines + j + 1;
			indices[k + 1] = i * longLines + j + 2;
			indices[k + 2] = (i + 1) * longLines + j + 1;

			indices[k + 3] = (i + 1) * longLines + j + 1;
			indices[k + 4] = i * longLines + j + 2;
			indices[k + 5] = (i + 1) * longLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * longLines) + longLines;
		indices[k + 1] = (i * longLines) + 1;
		indices[k + 2] = ((i + 1) * longLines) + longLines;

		indices[k + 3] = ((i + 1) * longLines) + longLines;
		indices[k + 4] = (i * longLines) + 1;
		indices[k + 5] = ((i + 1) * longLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < longLines - 1; ++l)
	{
		indices[k] = m_numSphereVertices - 1;
		indices[k + 1] = (m_numSphereVertices - 1) - (l + 1);
		indices[k + 2] = (m_numSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = m_numSphereVertices - 1;
	indices[k + 1] = (m_numSphereVertices - 1) - longLines;
	indices[k + 2] = m_numSphereVertices - 2;

	// create vertex and index buffer:
	m_indxedVertexBuffer->Create(cmdList, sizeof(Vertex) * m_numSphereVertices, sizeof(XMFLOAT3), indices.size(), vertices.data(), indices.data());
	m_cubemapSRV->CreateFromFile(cmdList, pathToDDSCubemap);
	m_cubeMapCB->CPUData().textureCubeIndex = m_cubemapSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
}

void Skybox::Update(FPSCamera* const skyboxCenterCamera)
{
	
	sphereWorld = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	Scale = XMMatrixScaling(10.f, 10.0f, 10.0f);
	//Make sure the sphere is always centered around camera
	Translation = XMMatrixTranslation(skyboxCenterCamera->GetCameraPosition().x, skyboxCenterCamera->GetCameraPosition().y, skyboxCenterCamera->GetCameraPosition().z);
	//Translation = XMMatrixTranslation(0.f, 0.f, 0.f);
	XMVECTOR rotationDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX mRotation = XMMatrixRotationAxis(rotationDir, 0);

	sphereWorld = Scale * Translation;

	XMFLOAT4X4 cameraRefView = skyboxCenterCamera->GetViewMatrix();
	XMFLOAT4X4 cameraRefProjection = skyboxCenterCamera->GetProjectionMatrix();
	XMMATRIX cameraView = XMLoadFloat4x4(&cameraRefView);
	XMMATRIX cameraProjection = XMLoadFloat4x4(&cameraRefProjection);
	cameraView = XMMatrixTranspose(cameraView);
	cameraProjection = XMMatrixTranspose(cameraProjection);

	WVP = sphereWorld * cameraView * cameraProjection;
	WVP = XMMatrixTranspose(WVP);
	XMStoreFloat4x4(&m_cubeMapCB->CPUData().WorldViewProjection, WVP);

	m_cubeMapCB->SendConstantDataToGPU();
}

