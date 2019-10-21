#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'T':
					for (int i = 0; i < m_nObjects; i++)
					{
						CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
						pExplosiveObject->m_bBlowingUp = true;
						pExplosiveObject->m_bZboom = true;
					}
					m_nObjects = 0;
					m_nAttackedObject = 0;
					break;
				case VK_SPACE:
					if (m_pPlayer->m_bDoPick)
						m_pPlayer->m_bDoPick = false;
					else
						m_pPlayer->m_bDoPick = true;
					break;
				case 'R':
					m_nObjects = 0;				//적 0
					m_nScore = 0;				//점수
					NotRedPeriodStart = -1;		//타이머 초기화
					m_pPlayer->m_nBoom = 0;		//아이템 0
					RedCount = 0;				//레드 생성 카운트
					m_nPlayerBullets = 0;		//플레이어 총알 인덱스
					m_nBossBullets = 0;			//플레이어 총알 인덱스
					m_pPlayer->m_nPickingIndex = -1;	//피킹오브젝트인덱스
					m_pPlayer->m_bDoPick = false;		//피킹할것인지
					m_nLiveObject = 0;					//생존오브젝트 수
					m_nAttackedObject = 0;				//잡은 오브젝트 수

					m_pBossObject->m_bRenderFlag = false;
					m_pBossObject->m_bDieFlag = false;
					m_pBossObject->m_nBossLife = 50;
					m_pBossObject->m_nBossAttackTimer = 3;
					m_pBossObject->m_nBossSecondPat = 0;

					m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
					m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
					m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);


					m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
					m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
					m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

					m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
					m_pPlayer->SetColor(RGB(0, 0, 255));
					m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));//카메라
					break;
				case 'G':
					m_pPlayer->m_nPickingIndex = -1;
					break;
				case 'Z':
					if (m_pPlayer->m_nBoom > 0)
					{
						for (int i = 0; i < m_nObjects; i++)
						{
							CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];

							if (pExplosiveObject->m_bDeathFlag == false)
							{
								if (Vector3::Length(Vector3::Subtract(m_pPlayer->m_xmf3Position, m_ppObjects[i]->GetPosition())) <= 30)
								{
									m_nAttackedObject++;
									pExplosiveObject->m_bBlowingUp = true;
									pExplosiveObject->m_bZboom = true;
								}
							}
						}
						m_pPlayer->m_nBoom--;
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 150.0f, fHalfHeight = 150.0f, fHalfDepth = 500.0f;
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 10);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);

	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CCubeMesh *pPlayerBulletCubeMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	pPlayerBulletCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


	m_pPlayerBulletObject = new CBulletObject[50];

	for (int i = 0; i < 50; i++)
	{
		m_pPlayerBulletObject[i].SetMesh(pPlayerBulletCubeMesh);
		m_pPlayerBulletObject[i].SetColor(RGB(255, 255, 0));
		m_pPlayerBulletObject[i].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_pPlayerBulletObject[i].SetRotationSpeed(100.0f);
		m_pPlayerBulletObject[i].SetMovingSpeed(100.0f);
	}

	m_pBossBulletObject = new CBulletObject[5];

	CCubeMesh *pBossBulletCubeMesh = new CCubeMesh(2.0f, 2.0f, 2.0f);
	pBossBulletCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	for (int i = 0; i < 5; i++)
	{
		m_pBossBulletObject[i].SetMesh(pBossBulletCubeMesh);
		m_pBossBulletObject[i].SetColor(RGB(0, 255, 255));
		m_pBossBulletObject[i].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_pBossBulletObject[i].SetRotationSpeed(200.0f);
		m_pBossBulletObject[i].SetMovingSpeed(50.0f);
		m_pBossBulletObject[i].SetPosition(-2000, 0, 0);
		m_pBossBulletObject[i].m_bRenderFlag = true;
		m_pBossBulletObject[i].Animate(0);
	}
	m_nObjects = 0;
	m_ppObjects = new CGameObject*[1500];

	BossMesh *pBossMesh = new BossMesh();
	pBossMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(4.0f, 4.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pBossObject = new CBossObject();
	m_pBossObject->SetMesh(pBossMesh);
	m_pBossObject->SetColor(RGB(255, 0, 255));
	m_pBossObject->SetPosition(0.0f, 0.0f, 400.0f);
	m_pBossObject->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pBossObject->SetRotationSpeed(0.0f);
	m_pBossObject->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_pBossObject->SetMovingSpeed(20.0f);
}
void CScene::BuildEnemyMeshRed()
{
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_nObjects += 1;

	m_ppObjects[m_nObjects - 1] = new CExplosiveObject();
	m_ppObjects[m_nObjects - 1]->SetMesh(pObjectCubeMesh);
	m_ppObjects[m_nObjects - 1]->SetColor(RGB(255, 0, 0));
	m_ppObjects[m_nObjects - 1]->SetRotationAxis(XMFLOAT3(rand() % 2, 1, rand() % 2));
	m_ppObjects[m_nObjects - 1]->SetRotationSpeed(90.0f);
	m_ppObjects[m_nObjects - 1]->SetMovingSpeed(15.0f);
	if( m_pPlayer->m_xmf3Position.z < 350)
		m_ppObjects[m_nObjects - 1]->SetPosition((float)(rand() % 90 - 45), (float)(rand() % 90 - 45), m_pPlayer->GetPosition().z + 100.0f);
	else
		m_ppObjects[m_nObjects - 1]->SetPosition((float)(rand() % 90 - 45), (float)(rand() % 90 - 45), 400);
	m_ppObjects[m_nObjects - 1]->SetMovingDirection(XMFLOAT3((float)(rand() % 2 - 1), (float)(rand() % 2 - 1), (float)(rand() % 2 - 1)));

}
void CScene::BuildEnemyMeshXred()
{
	if (m_nLiveObject == 99)
	{
		m_nObjects = 0;				//적 0
		m_nScore = 0;				//점수
		NotRedPeriodStart = -1;		//타이머 초기화
		m_pPlayer->m_nBoom = 0;		//아이템 0
		RedCount = 0;				//레드 생성 카운트
		m_nPlayerBullets = 0;		//플레이어 총알 인덱스
		m_nBossBullets = 0;			//플레이어 총알 인덱스
		m_pPlayer->m_nPickingIndex = -1;	//피킹오브젝트인덱스
		m_pPlayer->m_bDoPick = false;		//피킹할것인지
		m_nLiveObject = 0;					//생존오브젝트 수
		m_nAttackedObject = 0;				//잡은 오브젝트 수

		m_pBossObject->m_bRenderFlag = false;
		m_pBossObject->m_bDieFlag = false;
		m_pBossObject->m_nBossLife = 50;
		m_pBossObject->m_nBossAttackTimer = 3;
		m_pBossObject->m_nBossSecondPat = 0;

		m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);


		m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

		m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
		m_pPlayer->SetColor(RGB(0, 0, 255));
		m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));//카메라
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			XMFLOAT3 FollowPlayer = Vector3::Subtract(m_pPlayer->m_xmf3Position, m_ppObjects[i]->GetPosition());
			m_ppObjects[i]->SetMovingDirection(FollowPlayer);
		}
	}

	if (m_pBossObject->m_bRenderFlag == false)
	{
		RedCount++;
		if (RedCount % 10 == 0)
			BuildEnemyMeshRed();
	}

	int objectColor = 0;
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	objectColor = rand() % 3;
	m_nObjects += 1;

	m_ppObjects[m_nObjects - 1] = new CExplosiveObject();
	m_ppObjects[m_nObjects - 1]->SetMesh(pObjectCubeMesh);

	if (objectColor == 0)
	{
		m_ppObjects[m_nObjects - 1]->SetColor(RGB(0, 255, 0));
		m_ppObjects[m_nObjects - 1]->SetRotationSpeed(90.0f);
	}
	else if (objectColor == 1)
	{
		m_ppObjects[m_nObjects - 1]->SetColor(RGB(0, 0, 255));
		m_ppObjects[m_nObjects - 1]->SetRotationSpeed(90.0f);
	}
	else if (objectColor == 2)
	{
		m_ppObjects[m_nObjects - 1]->SetColor(RGB(255, 128, 255));
		m_ppObjects[m_nObjects - 1]->SetRotationSpeed(90.0f);
	}
	if (m_pPlayer->m_xmf3Position.z < 350)
		m_ppObjects[m_nObjects - 1]->SetPosition((float)(rand() % 90 - 45), (float)(rand() % 90 - 45), m_pPlayer->GetPosition().z + 100.0f);
	else
		m_ppObjects[m_nObjects - 1]->SetPosition((float)(rand() % 90 - 45), (float)(rand() % 90 - 45), 400);
	m_ppObjects[m_nObjects - 1]->SetRotationAxis(XMFLOAT3(1, rand() % 2, rand() % 2));
	m_ppObjects[m_nObjects - 1]->SetMovingDirection(XMFLOAT3((float)(rand() % 2 - 1), (float)(rand() % 2 - 1), (float)(rand() % 2 - 1)));
	m_ppObjects[m_nObjects - 1]->SetMovingSpeed(15.0f);
}
void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;

	if (m_pPlayerBulletObject) delete[] m_pPlayerBulletObject;

	if (m_pWallsObject) delete m_pWallsObject;
}

void CScene::CheckPlayerByBulletCollisions()
{
	for (int i = 0; i < 5; i++)
	{
		if (m_pBossBulletObject[i].m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) != DISJOINT)
		{
			m_nObjects = 0;				//적 0
			m_nScore = 0;				//점수
			NotRedPeriodStart = -1;		//타이머 초기화
			m_pPlayer->m_nBoom = 0;		//아이템 0
			RedCount = 0;				//레드 생성 카운트
			m_nPlayerBullets = 0;		//플레이어 총알 인덱스
			m_nBossBullets = 0;			//플레이어 총알 인덱스
			m_pPlayer->m_nPickingIndex = -1;	//피킹오브젝트인덱스
			m_pPlayer->m_bDoPick = false;		//피킹할것인지
			m_nLiveObject = 0;					//생존오브젝트 수
			m_nAttackedObject = 0;				//잡은 오브젝트 수

			m_pBossObject->m_bRenderFlag = false;
			m_pBossObject->m_bDieFlag = false;
			m_pBossObject->m_nBossLife = 50;
			m_pBossObject->m_nBossAttackTimer = 3;
			m_pBossObject->m_nBossSecondPat = 0;

			m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
			m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);


			m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
			m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

			m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
			m_pPlayer->SetColor(RGB(0, 0, 255));
			m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));//카메라

			break;
		}
	}
}
void CScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (!pExplosiveObject->m_bDeathFlag)
		{
			for (int j = (i + 1); j < m_nObjects; j++)
			{
				CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[j];
				if (!pExplosiveObject->m_bDeathFlag)
				{
					if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
					{
						m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
						m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
					}
				}
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			if (m_ppObjects[i]->m_pObjectCollided)
			{
				XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
				float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
				m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
				m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
				m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
				m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
				m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
				m_ppObjects[i]->m_pObjectCollided = NULL;
			}
		}
	}
}

void CScene::CheckObjectByBulletCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			for (int j = 0; j < m_nPlayerBullets; j++)
			{
				if (m_pPlayerBulletObject[j].m_bRenderFlag == false)
				{
					if (m_ppObjects[i]->m_xmOOBB.Intersects(m_pPlayerBulletObject[j].m_xmOOBB))
					{
						pExplosiveObject->m_bBlowingUp = true;
						m_nAttackedObject++;
						if (m_ppObjects[i]->m_dwColor == 255)
						{
							m_pPlayer->m_nBoom++;
							m_nScore += 50;
							
						}
						else
							m_nScore += 10;
						break;
					}
				}
			}
		}
	}

	if (m_pBossObject->m_bRenderFlag == true)
	{
		for (int i = 0; i < m_nPlayerBullets; i++)
		{
			if (m_pPlayerBulletObject[i].m_bRenderFlag == false)
			{
				if (m_pBossObject->m_xmOOBB.Intersects(m_pPlayerBulletObject[i].m_xmOOBB))
				{
					if (m_pBossObject->m_nBossLife != 0)
						m_pBossObject->m_nBossLife--;
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

void CScene::CheckBulletByWallCollisions()
{
	for (int i = 0; i < m_nPlayerBullets; i++)
	{
		if (m_pWallsObject->m_xmOOBB.Contains(m_pPlayerBulletObject[i].m_xmOOBB) != CONTAINS)
			m_pPlayerBulletObject[i].m_bRenderFlag = true;
	}

	if (m_pBossObject->m_bRenderFlag == true)
	{
		for (int i = 0; i < 5; i++)
		{
			if (m_pWallsObject->m_xmOOBB.Contains(m_pBossBulletObject[i].m_xmOOBB) != CONTAINS)
				m_pBossBulletObject[i].m_bRenderFlag = true;
		}
	}
}

void CScene::CheckPlayerByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			if (m_ppObjects[i]->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) != DISJOINT)
			{
				m_nObjects = 0;				//적 0
				m_nScore = 0;				//점수
				NotRedPeriodStart = -1;		//타이머 초기화
				m_pPlayer->m_nBoom = 0;		//아이템 0
				RedCount = 0;				//레드 생성 카운트
				m_nPlayerBullets = 0;		//플레이어 총알 인덱스
				m_nBossBullets = 0;			//플레이어 총알 인덱스
				m_pPlayer->m_nPickingIndex = -1;	//피킹오브젝트인덱스
				m_pPlayer->m_bDoPick = false;		//피킹할것인지
				m_nLiveObject = 0;					//생존오브젝트 수
				m_nAttackedObject = 0;				//잡은 오브젝트 수

				m_pBossObject->m_bRenderFlag = false;
				m_pBossObject->m_bDieFlag = false;
				m_pBossObject->m_nBossLife = 50;
				m_pBossObject->m_nBossAttackTimer = 3;
				m_pBossObject->m_nBossSecondPat = 0;

				m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
				m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
				m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);


				m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
				m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
				m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

				m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
				m_pPlayer->SetColor(RGB(0, 0, 255));
				m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));//카메라
				break;
			}
		}
	}

}

void CScene::CheckPlayerByBossCollisions()
{
	if (m_pBossObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) != DISJOINT)
	{
		m_nObjects = 0;				//적 0
		m_nScore = 0;				//점수
		NotRedPeriodStart = -1;		//타이머 초기화
		m_pPlayer->m_nBoom = 0;		//아이템 0
		RedCount = 0;				//레드 생성 카운트
		m_nPlayerBullets = 0;		//플레이어 총알 인덱스
		m_nBossBullets = 0;			//플레이어 총알 인덱스
		m_pPlayer->m_nPickingIndex = -1;	//피킹오브젝트인덱스
		m_pPlayer->m_bDoPick = false;		//피킹할것인지
		m_nLiveObject = 0;					//생존오브젝트 수
		m_nAttackedObject = 0;				//잡은 오브젝트 수

		m_pBossObject->m_bRenderFlag = false;
		m_pBossObject->m_bDieFlag = false;
		m_pBossObject->m_nBossLife = 50;
		m_pBossObject->m_nBossAttackTimer = 3;
		m_pBossObject->m_nBossSecondPat = 0;

		m_pPlayer->m_pCamera->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_pPlayer->m_pCamera->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pPlayer->m_pCamera->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);


		m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

		m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);//위치 초기화
		m_pPlayer->SetColor(RGB(0, 0, 255));
		m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));//카메라
	}
}
void CScene::CheckBossByWallCollisions()
{
	ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pBossObject->m_xmOOBB);
	switch (containType)
	{
	case DISJOINT:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pBossObject->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pBossObject->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pBossObject->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case INTERSECTS:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pBossObject->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pBossObject->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pBossObject->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case CONTAINS:
		break;
	}
}
void CScene::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
	}
}


void CScene::Animate(float fElapsedTime)
{
	clock_t NotRedPeriodEnd;			//빨강 오브젝트를 제외한 생성에 관한 시간					

	NotRedPeriodEnd = clock();		

	if (m_pBossObject->m_bRenderFlag == false)
	{
		if ((NotRedPeriodEnd - NotRedPeriodStart) / 1000 >= 1)
		{
			NotRedPeriodStart = NotRedPeriodEnd;
			BuildEnemyMeshXred();	//오브젝트 생성함수로 ( 빨강 오브젝트 생성함수는 이 함수 내부에 같이 존재
		}
	}
	else
	{
		if ((NotRedPeriodEnd - NotRedPeriodStart) / 1000 >= m_pBossObject->m_nBossAttackTimer)
		{
			m_pBossObject->m_nBossAttackTimer = 1;
			m_pBossObject->m_nBossSecondPat++;
			m_pBossObject->SetRotationSpeed(0.0f);
			if (m_pBossObject->m_nBossSecondPat == 2)
			{
				for (int i = 0; i < 2; i++)
					BuildEnemyMeshXred();
				m_pBossObject->SetRotationSpeed(90.0f);
				m_pBossObject->m_nBossSecondPat = 0;
			}
			NotRedPeriodStart = NotRedPeriodEnd;
			m_pBossBulletObject[m_nBossBullets].SetPosition(m_pBossObject->GetPosition().x, m_pBossObject->GetPosition().y, m_pBossObject->GetPosition().z);
			m_pBossBulletObject[m_nBossBullets].SetMovingDirection(Vector3::Subtract(m_pPlayer->m_xmf3Position, m_pBossObject->GetPosition()));
			m_pBossBulletObject[m_nBossBullets].m_bRenderFlag = false;
			m_nBossBullets++;
			if (m_nBossBullets == 5)
				m_nBossBullets = 0;
		}
	}

	m_pWallsObject->Animate(fElapsedTime);

	for (int i = 0; i < m_nPlayerBullets; i++)
	{
		if (m_pPlayerBulletObject[i].m_bRenderFlag == false)	//총알이 그려진다면
		{
			m_pPlayerBulletObject[i].m_fPlayerDistance = Vector3::Length(Vector3::Subtract(m_pPlayer->m_xmf3Position, m_pPlayerBulletObject[i].GetPosition()));
			if (m_pPlayerBulletObject[i].m_fPlayerDistance > 100.0f)	//플레이어와 플레이어 총알 간 거리 구한 후 일정 거리 이상일 시 
				m_pPlayerBulletObject[i].m_bRenderFlag = true;			//총알을 더이상 그려주지 않기위한 플래그 작동
		}
	}
	
	if (m_pPlayer->m_nPickingIndex != -1)		
	{
		if (m_pPlayer->m_nPickingIndex != -2)
		{
			CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[m_pPlayer->m_nPickingIndex];
			if (pExplosiveObject->m_bDeathFlag == true)	//피킹된 오브젝트가 폭발 시
				m_pPlayer->m_nPickingIndex = -1;
		}
	}

	if (m_pPlayer->m_xmf3Position.z > 300 && m_pBossObject->m_bDieFlag == false)		//보스 등장
		m_pBossObject->m_bRenderFlag = true;
	
	if (m_pBossObject->m_bRenderFlag)
	{
		for (int i = 0; i < 5; i++)
		{
			m_pBossBulletObject[i].m_fPlayerDistance = Vector3::Length(Vector3::Subtract(m_pBossObject->GetPosition(), m_pBossBulletObject[i].GetPosition()));
			if (m_pBossBulletObject[i].m_fPlayerDistance > 200.0f)
				m_pBossBulletObject[i].m_bRenderFlag = true;
		}
		for (int i = 0; i < 5; i++)
		{
			if (m_pBossBulletObject[i].m_bRenderFlag == false)
				m_pBossBulletObject[i].Animate(fElapsedTime);
		}
		m_pBossObject->Animate(fElapsedTime);
	}

	for (int i = 0; i < m_nObjects; i++)
	{
		CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_ppObjects[i];
		if (pExplosiveObject->m_bDeathFlag == false)
		{
			if (m_ppObjects[i]->GetPosition().z < m_pPlayer->m_xmf3Position.z && m_pPlayer->m_dwColor != 255)
			{
				m_pPlayer->SetColor(RGB(255, 192, 203));
				break;
			}
		}
	}

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Animate(fElapsedTime);
	}

	for (int i = 0; i < m_nPlayerBullets; i++)
	{
		if (m_pPlayerBulletObject[i].m_bRenderFlag == false)
			m_pPlayerBulletObject[i].Animate(fElapsedTime);
	}

	CheckPlayerByObjectCollisions();

	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();

	CheckObjectByBulletCollisions();

	CheckPlayerByBulletCollisions();

	CheckBulletByWallCollisions();

	m_nLiveObject = m_nObjects - m_nAttackedObject;

	if (m_pBossObject->m_bRenderFlag)
	{
		CheckBossByWallCollisions();
		CheckPlayerByBossCollisions();
	}
}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	TCHAR buf[1024];

	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	if (m_pBossObject->m_bRenderFlag == true)
	{
		m_pBossObject->Render(hDCFrameBuffer, pCamera);
		for (int i = 0; i < 5; i++)
		{
			if (m_pBossBulletObject[i].m_bRenderFlag == false)
				m_pBossBulletObject[i].Render(hDCFrameBuffer, pCamera);
		}
	}

	for (int i = 0; i < m_nObjects; i++)
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < m_nPlayerBullets; i++)
	{
		if (m_pPlayerBulletObject[i].m_bRenderFlag == false)
			m_pPlayerBulletObject[i].Render(hDCFrameBuffer, pCamera);
	}

	wsprintf(buf, TEXT("Objects : %d"), m_nLiveObject);
	TextOut(hDCFrameBuffer, 20, 390, buf, wcslen(buf));
	wsprintf(buf, TEXT("Bullets : %d"), 50 - m_nPlayerBullets);
	TextOut(hDCFrameBuffer, 20, 410, buf, wcslen(buf));
	wsprintf(buf, TEXT("Score : %d"), m_nScore);
	TextOut(hDCFrameBuffer, 20, 430, buf, wcslen(buf));
	wsprintf(buf, TEXT("Bomb : %d"), m_pPlayer->m_nBoom);
	TextOut(hDCFrameBuffer, 20, 450, buf, wcslen(buf));
}
