//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}


XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
			return(XMVector3Normalize(v));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial)
		m_pMaterial = new CMaterial();
	m_pMaterial->m_nReflection = nReflection;
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);

	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	//if (m_pMaterial)
	//{
	//	if (m_pMaterial->m_pShader)
	//	{
	//		m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	//		m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	//	}
	//}

	if (m_pMesh) 
		m_pMesh->Render(pd3dCommandList);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4& xmf4x4matix, int index)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4matix)));
	CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)(m_pcbMappedBombObjects + (index * ncbGameObjectBytes));
	::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////

CWallsObject::CWallsObject()
{
}
CWallsObject::~CWallsObject()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////

XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh **CExplosiveObject::m_ppExplosionMesh = NULL;
int CExplosiveObject::m_nBoonindex = 0;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
	static int count = 0;

	if (count==0)
	{
		for (int j = 0; j < 7; j++) 
			m_ppExplosionMesh[j]->ReleaseUploadBuffers();
		count = 1;

		if (m_ppExplosionMesh)
		{
			for (int j = 0; j < 7; j++)
			{
				if (m_ppExplosionMesh[j]) delete m_ppExplosionMesh[j];
			}
			delete[] m_ppExplosionMesh;
		}
	}
}

void CExplosiveObject::PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_ppExplosionMesh = new CMesh*[7];

	m_ppExplosionMesh[0] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1, 0, 0, 1));
	m_ppExplosionMesh[1] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 1, 0, 1));
	m_ppExplosionMesh[2] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1, 0, 1, 1));
	m_ppExplosionMesh[3] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 1, 1, 1));
	m_ppExplosionMesh[4] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1, 1, 0, 1));
	m_ppExplosionMesh[5] = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 0, 1, 1));
	m_ppExplosionMesh[6] = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_bDeathFlag = true;
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * 5 * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * 5 * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * 5 * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
			m_bRenderFlag = false;
			if (m_bZBoomFlag)
				m_bZBoomFlag = false;
		}
	}
	else
	{
		if (m_bDeathFlag == false)
			CGameObject::Animate(fElapsedTime);
	}

}

void CExplosiveObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_bBlowingUp)
	{
		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_ppExplosionMesh[0])
			{
				OnPrepareRender();

				//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
				UpdateShaderVariables(pd3dCommandList, m_pxmf4x4Transforms[i], m_nBoonindex + i);

				pd3dCommandList->SetGraphicsRootConstantBufferView(2, m_d3dcbBombGpuVirtualAddress + (ncbGameObjectBytes * (m_nBoonindex + i )));

				if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
				if (m_nPickingPlaneColor == -1)
					m_ppExplosionMesh[6]->Render(pd3dCommandList);
				else
					m_ppExplosionMesh[m_nPickingPlaneColor]->Render(pd3dCommandList);
			}
		}
		if (m_bZBoomFlag == true)
		{
			m_nBoonindex += 99;
		}
	}
	else
	{
		if ( m_bDeathFlag == false)
		{
			OnPrepareRender();

			if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
			if (m_pMesh) m_pMesh->Render(pd3dCommandList);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////

CBossObject::CBossObject()
{
}

CBossObject::~CBossObject()
{
}