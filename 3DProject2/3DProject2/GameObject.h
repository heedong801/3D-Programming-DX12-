#pragma once
#include "Camera.h"
#include "Mesh.h"

class CShader;

class CGameObject
{
public:
	CGameObject();   
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public: 
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;
	CShader *m_pShader = NULL;

public: 
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh *pMesh); 
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Move(XMFLOAT3& vDirection, float fSpeed);

public: 
	//따라하기 11
	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4& xmf4x4matix );
	virtual void ReleaseShaderVariables();

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook(); 
	XMFLOAT3 GetUp(); 
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z); 
	void SetPosition(XMFLOAT3 xmf3Position);
	
	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f); 
	void MoveUp(float fDistance = 1.0f); 
	void MoveForward(float fDistance = 1.0f);
	
	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed;
	float						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

	bool						m_bRenderFlag = false;
	bool						m_bRed = false;

	CGameObject					*m_pObjectCollided = NULL;
	int							m_nCheckCollideNum = 0;
	int							m_nCheckPrevCollidIndex = -1;

	XMFLOAT3					m_xmf3RememberPlayerPosition;

	XMFLOAT4					m_pxmf4Planes[6];

	float						m_nPickMeshDistance = 10000;

	BoundingOrientedBox			m_xmOOBB;
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }

	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
};


class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

	XMFLOAT4					m_pxmf4WallPlanes[6];
};

class CBossObject : public CGameObject
{
public:
	CBossObject();
	virtual ~CBossObject();

	bool						m_bDieFlag = false;
	int							m_nBossLife = 5;
	int							m_nBossSecondPat = 0;
	int							m_nBossAttackTimer = 3;
};
class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;
	bool						m_bDeathFlag = false;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 1.0f;
	float						m_fExplosionSpeed = 15.0f;
	float						m_fExplosionRotation = 720.0f;

	int							m_nPickingPlaneColor = -1;

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

public:
	static CMesh				**m_ppExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
};