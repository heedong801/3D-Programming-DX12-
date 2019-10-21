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
	
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	
	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void ReleaseUploadBuffers();
	
	ID3D12PipelineState *m_pd3dPipelineState; 
	//���������� ���¸� ��Ÿ���� �������̽� �������̴�. 

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
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
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature * m_pd3dGraphicsRootSignature = NULL;
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 

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

