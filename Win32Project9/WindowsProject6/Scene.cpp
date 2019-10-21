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


//void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
//{ 
//	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
//	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
//	
//	CTriangleMesh *pMesh = new CTriangleMesh(pd3dDevice, pd3dCommandList);
//	m_nObjects = 1;
//
//	m_ppObjects = new CGameObject*[m_nObjects];
//
//	CRotatingObject *pRotatingObject = new CRotatingObject(); 
//	pRotatingObject->SetMesh(pMesh);
//
//	CDiffusedShader *pShader = new CDiffusedShader();
//
//	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
//	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
//	pRotatingObject->SetShader(pShader);
//	m_ppObjects[0] = pRotatingObject;
//
//	////���� �׸��� ���� ���̴� ��ü�� �����Ѵ�. 
//	//m_nShaders = 1; 
//	//m_ppShaders = new CShader*[m_nShaders];
//	//CShader *pShader = new CShader(); 
//	//pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
//	//pShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
//	//m_ppShaders[0] = pShader;
//}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	
	//����x����x���̰� 12x12x12�� ������ü �޽��� �����Ѵ�.
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	
	CRotatingObject *pRotatingObject = new CRotatingObject(); 
	pRotatingObject->SetMesh(pCubeMesh);

	CDiffusedShader *pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pRotatingObject->SetShader(pShader);
	m_ppObjects[0] = pRotatingObject;
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

//void CScene::ReleaseObjects() 
//{
//	if (m_pd3dGraphicsRootSignature) 
//		m_pd3dGraphicsRootSignature->Release();
//	if (m_ppShaders)
//	{
//		for (int i = 0; i < m_nShaders; i++) 
//		{ 
//			m_ppShaders[i]->ReleaseShaderVariables();
//			m_ppShaders[i]->ReleaseObjects();
//			m_ppShaders[i]->Release(); 
//		} 
//		delete[] m_ppShaders;
//	}
//}

void CScene::ReleaseObjects() 
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects) 
	{
		for (int j = 0; j < m_nObjects; j++) 
			if (m_ppObjects[j])
				delete m_ppObjects[j]; 
		delete[] m_ppObjects; 
	}
}


//void CScene::ReleaseUploadBuffers() 
//{ 
//	if (m_ppShaders)
//	{
//		for (int j = 0; j < m_nShaders; j++)
//			if (m_ppShaders[j]) 
//				m_ppShaders[j]->ReleaseUploadBuffers(); 
//	} 
//}

void CScene::ReleaseUploadBuffers() 
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers(); 
	}
}


ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{ 
	return(m_pd3dGraphicsRootSignature);
}

//ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) 
//{
//	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
//	//�Ű������� ���� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
//	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc; 
//	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
//	d3dRootSignatureDesc.NumParameters = 0; 
//	d3dRootSignatureDesc.pParameters = NULL;
//	d3dRootSignatureDesc.NumStaticSamplers = 0; 
//	d3dRootSignatureDesc.pStaticSamplers = NULL; 
//	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	ID3DBlob *pd3dSignatureBlob = NULL; 
//	ID3DBlob *pd3dErrorBlob = NULL; 
//	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob); 
//	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature); if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
//	if (pd3dErrorBlob) pd3dErrorBlob->Release();
//	
//	return(pd3dGraphicsRootSignature);
//}


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

//void CScene::AnimateObjects(float fTimeElapsed)
//{
//	for (int i = 0; i < m_nShaders; i++)
//	{
//		m_ppShaders[i]->AnimateObjects(fTimeElapsed); 
//	} 
//}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed); 
	}
}

//void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList) 
//{ 
//	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�.
//	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
//	//���� �������ϴ� ���� ���� �����ϴ� ���̴�(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�.
//	for (int i = 0; i < m_nShaders; i++)
//	{ 
//		m_ppShaders[i]->Render(pd3dCommandList);
//	} 
//}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList); 
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (pCamera)
		pCamera->UpdateShaderVariables(pd3dCommandList);
	//���� �������ϴ� ���� ���� �����ϴ� ���� ��ü(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�.
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}