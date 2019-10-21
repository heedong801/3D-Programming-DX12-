#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//

CGameObject::CGameObject() 
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity()); 
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release(); 
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables(); 
		m_pShader->Release();
	} 
}

void CGameObject::SetShader(CShader *pShader)
{ 
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader; 
	if (m_pShader) m_pShader->AddRef(); 
}

void CGameObject::SetMesh(CMesh *pMesh)
{ 
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh; 
	if (m_pMesh) m_pMesh->AddRef(); 
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
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

void CGameObject::OnPrepareRender() 
{
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) 
{
	OnPrepareRender();

	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
	UpdateShaderVariables(pd3dCommandList, m_xmf4x4World);
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle) 
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle)); 
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) 
{
}

void CGameObject::ReleaseShaderVariables() 
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4& xmf4x4matix)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4matix)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 
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
//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook() 
{
	XMFLOAT3 tmp = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };

	return(Vector3::Normalize(tmp));
}
//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 tmp = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };

	return(Vector3::Normalize(tmp));
}
//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight() 
{
	XMFLOAT3 tmp = { m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };

	return(Vector3::Normalize(tmp));
}
//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance) 
{
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Right = GetRight(); 
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance); 
	CGameObject::SetPosition(xmf3Position); 
}
//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance) 
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp(); 
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance); 
	CGameObject::SetPosition(xmf3Position); 
}
//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) 
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World); 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////

XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh **CExplosiveObject::m_ppExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
	
}

void CExplosiveObject::PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) 
		XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_ppExplosionMesh = new CMesh*[7];

	m_ppExplosionMesh[0] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1,0,0,1));
	m_ppExplosionMesh[1] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 1, 0, 1));
	m_ppExplosionMesh[2] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1, 0, 1, 1));
	m_ppExplosionMesh[3] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 1, 1, 1));
	m_ppExplosionMesh[4] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(1, 1, 0, 1));
	m_ppExplosionMesh[5] = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f, XMFLOAT4(0, 0, 1, 1));
	m_ppExplosionMesh[6] = new CCubeMeshColorful(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 3.0f);
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
		}
	}
	else
	{
		if (m_bBlowingUp == false && m_bDeathFlag == false)
			CGameObject::Animate(fElapsedTime);
		else
		{
			CGameObject::Animate(fElapsedTime);
		}
	}

}

void CExplosiveObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	static int count = 0;

	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_ppExplosionMesh[0])
			{
				if (count == 2)
				{
					OnPrepareRender();

					//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
					UpdateShaderVariables(pd3dCommandList, m_pxmf4x4Transforms[i]);
					if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
					if( m_nPickingPlaneColor == -1)
						m_ppExplosionMesh[6]->Render(pd3dCommandList);
					else
						m_ppExplosionMesh[m_nPickingPlaneColor]->Render(pd3dCommandList);
					count = 0;
				}
				else
					count++;
			}
		}
	}
	else
	{
		if (m_bBlowingUp == false && m_bDeathFlag == false)
		{
			OnPrepareRender();

			//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
			UpdateShaderVariables(pd3dCommandList, m_xmf4x4World);
			if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
			if (m_pMesh) m_pMesh->Render(pd3dCommandList);
		}
		else
		{
			OnPrepareRender();

			//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
			UpdateShaderVariables(pd3dCommandList, m_xmf4x4World);
			if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
			if (m_pMesh) m_pMesh->Render(pd3dCommandList);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////

CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////

CBossObject::CBossObject()
{
}

CBossObject::~CBossObject()
{
}