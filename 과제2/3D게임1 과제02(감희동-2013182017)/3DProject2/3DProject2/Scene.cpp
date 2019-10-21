#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
	m_pd3dPipelineState = NULL;
	m_pd3dGraphicsRootSignature = NULL;
}


CScene::~CScene()
{

}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CExplosiveObject::PrepareExplosion(pd3dDevice, pd3dCommandList);

	m_nShaders = 1;
	m_pShaders = new CObjectsShader[m_nShaders]; 
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature); 
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return(false); 
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	return(false); 
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (int i = 0; i < m_nShaders; i++) 
	{
		m_pShaders[i].ReleaseShaderVariables(); 
		m_pShaders[i].ReleaseObjects(); 
	} 
	if (m_pShaders) 
		delete[] m_pShaders;
}
void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	if (m_pShaders[0].m_pBossObject->m_bRenderFlag == false)
	{
		if (m_bMobJen)
		{
			BuildEnemyMeshXred();	//오브젝트 생성함수로 ( 빨강 오브젝트 생성함수는 이 함수 내부에 같이 존재
			m_bMobJen = false;
		}
	}
	else
	{
		if (m_bMobJen && m_nBossAttackStart < m_pShaders[0].m_pBossObject->m_nBossAttackTimer)
		{
			m_nBossAttackStart++;
			m_bMobJen = false;
		}
		if (m_nBossAttackStart >= m_pShaders[0].m_pBossObject->m_nBossAttackTimer)
		{
			m_nBossAttackStart = 0;
			m_pShaders[0].m_pBossObject->m_nBossAttackTimer = 1;
			m_pShaders[0].m_pBossObject->m_nBossSecondPat++;
			m_pShaders[0].m_pBossObject->SetRotationSpeed(0.0f);
			if (m_pShaders[0].m_pBossObject->m_nBossSecondPat == 2)
			{
				for (int i = 0; i < 2; i++)
					BuildEnemyMeshXred();
				m_pShaders[0].m_pBossObject->SetRotationSpeed(85.0f);
				m_pShaders[0].m_pBossObject->m_nBossSecondPat = 0;
			}
			m_pShaders[0].m_pBossBullets[m_pShaders[0].m_nBossBullets].m_xmf3RememberPlayerPosition = m_pPlayer->m_xmf3Position;
			m_pShaders[0].m_pBossBullets[m_pShaders[0].m_nBossBullets].SetPosition(m_pShaders[0].m_pBossObject->GetPosition().x, m_pShaders[0].m_pBossObject->GetPosition().y, m_pShaders[0].m_pBossObject->GetPosition().z);
			m_pShaders[0].m_pBossBullets[m_pShaders[0].m_nBossBullets].SetMovingDirection(Vector3::Subtract(m_pPlayer->m_xmf3Position, m_pShaders[0].m_pBossObject->GetPosition()));
			m_pShaders[0].m_pBossBullets[m_pShaders[0].m_nBossBullets].m_bRenderFlag = true;
			m_pShaders[0].m_nBossBullets++;
			if (m_pShaders[0].m_nBossBullets == 5)
				m_pShaders[0].m_nBossBullets = 0;
		}
	}
	
	if (m_pPlayer->m_xmf3Position.z > 350 && m_pShaders[0].m_pBossObject->m_bRenderFlag == false)
	{
		m_pShaders[0].m_pBossObject->m_bRenderFlag = true;
		m_pShaders[0].m_pBossObject->SetPosition(0, 0, 400);
	}

	if (m_nPresentPlayerColor != m_nPrevPlayerColor)
	{
		if(m_nPresentPlayerColor == 0)
			m_pPlayer->SetMesh(m_pPlayer->m_ppMeshs[0]);
		else if (m_nPresentPlayerColor == 1)
			m_pPlayer->SetMesh(m_pPlayer->m_ppMeshs[1]);
		else if (m_nPresentPlayerColor == 2)
			m_pPlayer->SetMesh(m_pPlayer->m_ppMeshs[2]);
		else
			m_pPlayer->SetMesh(m_pPlayer->m_ppMeshs[3]);
	

		m_nPrevPlayerColor = m_nPresentPlayerColor;
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed); 
	}

	CheckObjectByWallCollisions();
	CheckPlayerByObjectCollisions();
	CheckObjectByObjectCollisions();
	CheckBulletByWallCollisions();
	CheckObjectByBulletCollisions();

	if (m_pShaders[0].m_pBossObject->m_bRenderFlag)
	{
		CheckBossByWallCollisions();
		CheckPlayerByBulletCollisions();
		CheckPlayerByBossCollisions();
	}
	m_nLiveObject = m_pShaders->m_nObjects - m_nAttackedObject;
}


ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{ 
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) 
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS; 
	
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc; 
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters; 
	d3dRootSignatureDesc.NumStaticSamplers = 0; 
	d3dRootSignatureDesc.pStaticSamplers = NULL; 
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL; 
	
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob); 
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature); 
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) 
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}

}

CObjectsShader* CScene::GetShaders()
{
	return m_pShaders;
}

void CScene::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			ContainmentType containType = m_pShaders[0].m_ppWallObjects[6]->m_xmOOBB.Contains(m_pShaders[0].m_ppObjects[i]->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].x, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].y, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].x, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].y, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
	}
}

void CScene::CheckPlayerByObjectCollisions()
{
	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			if (m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
			{
				GameReset();
				break;
			}
		}
	}
}

void CScene::GameReset()
{
	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		pExplosiveObject->m_bBlowingUp = false;
		pExplosiveObject->m_bDeathFlag = false;
		pExplosiveObject->m_nPickingPlaneColor = -1;
		pExplosiveObject->m_pObjectCollided = NULL;
		m_pShaders[0].m_ppObjects[i]->m_nPickMeshDistance = 10000;
		m_pShaders[0].m_ppObjects[i]->m_bRenderFlag = false;
		m_pShaders[0].m_ppObjects[i]->m_nCheckPrevCollidIndex = -1;
		m_pShaders[0].m_ppObjects[i]->m_nCheckCollideNum = 0;
		m_pShaders[0].m_ppObjects[i]->SetRotationAxis(XMFLOAT3(1,0, rand() % 2));
	}
	m_pShaders[0].m_nObjects = 0;				//적 0
	m_nScore = 0;								//점수
	m_pPlayer->m_nBoom = 0;						//아이템 0
	RedCount = 0;								//레드 생성 카운트
	m_pShaders[0].m_nPlayerBullets = 0;			//플레이어 총알 인덱스
	m_pShaders[0].m_nBossBullets = 0;			//보스 총알 인덱스
	m_pPlayer->m_nPickingIndex = -1;			//피킹오브젝트인덱스
	m_pPlayer->m_bDoPick = false;				//피킹할것인지
	m_nLiveObject = 0;							//생존오브젝트 수
	m_nAttackedObject = 0;						//잡은 오브젝트 수
	m_bMobJen = false;							//몹젠 플래그 초기화
	m_bGameReset = true;						//게임리셋했다는 것을 프레임워크에 알리기 위함

	m_pShaders[0].m_pBossObject->m_bRenderFlag = false;
	m_pShaders[0].m_pBossObject->m_bDieFlag = false;
	m_pShaders[0].m_pBossObject->m_nBossLife = 5;
	m_pShaders[0].m_pBossObject->m_nBossAttackTimer = 3;
	m_pShaders[0].m_pBossObject->m_nBossSecondPat = 0;

	m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
	m_pPlayer->m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
}

void CScene::CheckObjectByObjectCollisions()
{
	static int nTimeGap = 0;

	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		m_pShaders[0].m_ppObjects[i]->m_pObjectCollided = NULL;
	}

	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			for (int j = (i + 1); j < m_pShaders[0].m_nObjects; j++)
			{
				CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[j];
				if (pExplosiveObject->m_bDeathFlag == false)
				{
					if (m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Intersects(m_pShaders[0].m_ppObjects[j]->m_xmOOBB))
					{
						m_pShaders[0].m_ppObjects[i]->m_pObjectCollided = m_pShaders[0].m_ppObjects[j];
						m_pShaders[0].m_ppObjects[j]->m_pObjectCollided = m_pShaders[0].m_ppObjects[i];
						m_pShaders[0].m_ppObjects[i]->m_nCheckCollideNum++;
						m_pShaders[0].m_ppObjects[j]->m_nCheckCollideNum++;
						m_pShaders[0].m_ppObjects[i]->m_nCheckPrevCollidIndex = j;
						m_pShaders[0].m_ppObjects[j]->m_nCheckPrevCollidIndex = i;
					}
				}
			}
		}
	}

	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			if (m_pShaders[0].m_ppObjects[i]->m_pObjectCollided)
			{
				XMFLOAT3 xmf3MovingDirection = m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection;
				float fMovingSpeed = m_pShaders[0].m_ppObjects[i]->m_fMovingSpeed;

				m_pShaders[0].m_ppObjects[i]->m_xmf3MovingDirection = m_pShaders[0].m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
				m_pShaders[0].m_ppObjects[i]->m_fMovingSpeed = m_pShaders[0].m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
				m_pShaders[0].m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
				m_pShaders[0].m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
				m_pShaders[0].m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
				m_pShaders[0].m_ppObjects[i]->m_pObjectCollided = NULL;

			}
		}
	}

	if (nTimeGap % 350 == 0)
	{
		for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
		{
			CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
			if (m_pShaders[0].m_ppObjects[i]->m_nCheckCollideNum > 15)
			{
				if (m_pShaders[0].m_ppObjects[m_pShaders[0].m_ppObjects[m_pShaders[0].m_ppObjects[i]->m_nCheckPrevCollidIndex]->m_nCheckPrevCollidIndex] == m_pShaders[0].m_ppObjects[i])
				{
					pExplosiveObject->m_bBlowingUp = true;
					m_pShaders[0].m_ppObjects[i]->m_nCheckCollideNum = 0;
					m_pShaders[0].m_ppObjects[m_pShaders[0].m_ppObjects[i]->m_nCheckPrevCollidIndex]->m_nCheckCollideNum = 0;
				}
			}
		}
	}
	nTimeGap++;
}

void CScene::CheckBulletByWallCollisions()
{
	for (int i = 0; i < m_pShaders[0].m_nPlayerBullets; i++)
	{
		if (m_pShaders[0].m_pPlayerBullets->m_bRenderFlag)
		{
			if (m_pShaders[0].m_ppWallObjects[6]->m_xmOOBB.Contains(m_pShaders[0].m_pPlayerBullets[i].m_xmOOBB) != CONTAINS)
				m_pShaders[0].m_pPlayerBullets[i].m_bRenderFlag = false;
		}
	}

	if (m_pShaders[0].m_pBossObject->m_bRenderFlag == true)
	{
		for (int i = 0; i < m_pShaders[0].m_nBossBullets; i++)
		{
			if (m_pShaders[0].m_pBossBullets->m_bRenderFlag)
			{
				if (m_pShaders[0].m_ppWallObjects[6]->m_xmOOBB.Contains(m_pShaders[0].m_pBossBullets[i].m_xmOOBB) != CONTAINS)
					m_pShaders[0].m_pBossBullets[i].m_bRenderFlag = false;
			}

		}
	}
}

void CScene::CheckObjectByBulletCollisions()
{
	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pShaders[0].m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			for (int j = 0; j < m_pShaders[0].m_nPlayerBullets; j++)
			{
				if (m_pShaders[0].m_pPlayerBullets[j].m_bRenderFlag == true)
				{
					if (m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Intersects(m_pShaders[0].m_pPlayerBullets[j].m_xmOOBB))
					{
						pExplosiveObject->m_bBlowingUp = true;
						m_pShaders[0].m_pPlayerBullets[j].m_bRenderFlag = false;
						m_nAttackedObject++;
						if (m_pShaders[0].m_ppObjects[i]->m_bRed == true)
						{
							m_pPlayer->m_nBoom++;
							m_nScore += 50;
						}
						else
							m_nScore += 10;
						if (i == m_pPlayer->m_nPickingIndex)
							m_pPlayer->m_nPickingIndex = -1;
						break;
					}
				}
			}
		}
	}

	for (int j = 0; j < m_pShaders[0].m_nPlayerBullets; j++)
	{
		if(m_pShaders[0].m_pPlayerBullets[j].m_bRenderFlag==true)
			if(Vector3::Length(Vector3::Subtract(m_pShaders[0].m_pPlayerBullets[j].GetPosition(), m_pShaders[0].m_pPlayerBullets[j].m_xmf3RememberPlayerPosition)) > 200)
				m_pShaders[0].m_pPlayerBullets[j].m_bRenderFlag = false;
	}

	if (m_pShaders[0].m_pBossObject->m_bRenderFlag == true)
	{
		for (int i = 0; i < m_pShaders[0].m_nPlayerBullets; i++)
		{
			if (m_pShaders[0].m_pPlayerBullets[i].m_bRenderFlag == true)
			{
				if (m_pShaders[0].m_pBossObject->m_xmOOBB.Intersects(m_pShaders[0].m_pPlayerBullets[i].m_xmOOBB))
				{
					if (m_pShaders[0].m_pBossObject->m_nBossLife != 0)
					{
						m_pShaders[0].m_pBossObject->m_nBossLife--;
						m_pShaders[0].m_pPlayerBullets[i].m_bRenderFlag = false;
					}
					else
					{
						m_nScore += 100;
						m_bGameEnd = true;
						break;
					}
				}
			}
		}
	}
}

void CScene::CheckBossByWallCollisions()
{
	ContainmentType containType = m_pShaders[0].m_ppWallObjects[6]->m_xmOOBB.Contains(m_pShaders[0].m_pBossObject->m_xmOOBB);
	switch (containType)
	{
	case DISJOINT:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pShaders[0].m_pBossObject->m_xmOOBB.Intersects(XMLoadFloat4(&m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].x, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].y, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pShaders[0].m_pBossObject->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pShaders[0].m_pBossObject->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case INTERSECTS:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pShaders[0].m_pBossObject->m_xmOOBB.Intersects(XMLoadFloat4(&m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].x, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].y, m_pShaders[0].m_ppWallObjects[6]->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pShaders[0].m_pBossObject->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pShaders[0].m_pBossObject->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case CONTAINS:
		break;
	}
}

void CScene::CheckPlayerByBossCollisions()
{
	if (m_pShaders[0].m_pBossObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) != DISJOINT)
	{
		GameReset();
	}
}

void CScene::CheckPlayerByBulletCollisions()
{
	for (int i = 0; i < m_pShaders[0].m_nBossBullets; i++)
	{
		if (m_pShaders[0].m_pBossBullets[i].m_bRenderFlag)
		{
			if (m_pShaders[0].m_pBossBullets[i].m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
			{
				GameReset();
				break;
			}
		}
	}

	for (int j = 0; j < m_pShaders[0].m_nBossBullets; j++)
	{
		if (Vector3::Length(Vector3::Subtract(m_pShaders[0].m_pBossObject->GetPosition(), m_pShaders[0].m_pBossBullets[j].m_xmf3RememberPlayerPosition)) > 200)
			m_pShaders[0].m_pBossBullets[j].m_bRenderFlag = false;
	}
}
void CScene::BuildEnemyMeshRed()
{
	m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->m_bRed = true;
	m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->m_bRenderFlag = true;
	m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->SetMesh(m_pShaders[0].m_pRedCubeMesh);

	if (m_pPlayer->m_xmf3Position.z < 350)
		m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->SetPosition((float)(rand() % 250 - 125), (float)(rand() % 250 - 125), m_pPlayer->GetPosition().z + 100.0f);
	else
		m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->SetPosition((float)(rand() % 250 - 125), (float)(rand() % 250 - 125), 400);
	
	m_pShaders[0].m_nObjects++;
}

void CScene::BuildEnemyMeshXred()
{
	for (int i = 0; i < m_pShaders[0].m_nObjects; i++)
	{
		XMFLOAT3 FollowPlayer = Vector3::Subtract(m_pPlayer->m_xmf3Position, m_pShaders[0].m_ppObjects[i]->GetPosition());
		m_pShaders[0].m_ppObjects[i]->SetMovingDirection(FollowPlayer);
	}

	if (m_nLiveObject > 50)
		GameReset();

	if (m_pShaders[0].m_pBossObject->m_bRenderFlag == false)
	{
		RedCount++;
		if (RedCount % 10 == 0)
			BuildEnemyMeshRed();
	}

	m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->m_bRed = false;
	m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->m_bRenderFlag = true;
	if (m_pPlayer->m_xmf3Position.z < 350)
		m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->SetPosition((float)(rand() % 250 - 125), (float)(rand() % 250 - 125), m_pPlayer->GetPosition().z + 100.0f);
	else
		m_pShaders[0].m_ppObjects[m_pShaders[0].m_nObjects]->SetPosition((float)(rand() % 250 - 125), (float)(rand() % 250 - 125), 400);

	m_pShaders[0].m_nObjects++;
}

