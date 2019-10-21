#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"
#include "Player.h"
class CScene
{
public:    
	CScene();    
	~CScene();
	
	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	
	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void ReleaseUploadBuffers();
	
	ID3D12PipelineState *m_pd3dPipelineState; 
	//파이프라인 상태를 나타내는 인터페이스 포인터이다. 

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice); 
	ID3D12RootSignature *GetGraphicsRootSignature();

	CObjectsShader* GetShaders();

	CPlayer			*m_pPlayer = NULL;

	bool			m_bMobJen = false;
	bool			m_bGameEnd = false;
	bool			m_bGameReset = false;

	int				RedCount = 0;
	int				m_nScore = 0;
	int				m_nLiveObject = 0;
	int				m_nAttackedObject = 0;

	int				m_nBossAttackStart = 0;

	int				m_nPrevPlayerColor = 0;
	int				m_nPresentPlayerColor = 0;
	//protected:
public: 
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature * m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 

public:
	void CheckObjectByWallCollisions();
	void CheckPlayerByObjectCollisions();
	void CheckObjectByObjectCollisions();
	void CheckBulletByWallCollisions();
	void CheckObjectByBulletCollisions();
	void CheckBossByWallCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckPlayerByBossCollisions();
	void GameReset();

	void BuildEnemyMeshXred();
	void BuildEnemyMeshRed();
};

