#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;

	int							m_nObjects = 0;			
	int							m_nPlayerBullets = 0;
	int							m_nBossBullets = 0;
	int							m_nScore = 0;
	int							m_nLiveObject = 0;
	int							m_nAttackedObject = 0;

	CGameObject					**m_ppObjects = NULL;
	CBossObject					*m_pBossObject = NULL;
	CBulletObject				*m_pPlayerBulletObject = NULL;
	CBulletObject				*m_pBossBulletObject = NULL;
	CWallsObject				*m_pWallsObject = NULL;

	clock_t						NotRedPeriodStart = -1;

	bool						m_bGameEnd = false;

	int							RedCount = 0;

	void BuildEnemyMeshXred();
	void BuildEnemyMeshRed();

	void CheckPlayerByObjectCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckBulletByWallCollisions();
	void CheckBossByWallCollisions();
	void CheckPlayerByBossCollisions();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();
	virtual void CheckObjectByWallCollisions();
	virtual void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

