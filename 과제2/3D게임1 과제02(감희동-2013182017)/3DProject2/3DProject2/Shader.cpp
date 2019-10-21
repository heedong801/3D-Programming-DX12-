#include "stdafx.h"
#include "Shader.h"


CShader::CShader()
{
}


CShader::~CShader() 
{ 
	if (m_ppd3dPipelineStates) 
	{
		for (int i = 0; i < m_nPipelineStates; i++)
			if (m_ppd3dPipelineStates[i]) 
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	} 
}

//래스터라이저 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState() 
{ 
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC)); 
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; 
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0; 
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE; 
	d3dRasterizerDesc.ForcedSampleCount = 0; 
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	
	return(d3dRasterizerDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState() 
{ 
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC)); 
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; 
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00; 
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP; 
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP; 
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP; 
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	
	return(d3dDepthStencilDesc);
}

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState() 
{ 
	D3D12_BLEND_DESC d3dBlendDesc; 
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE; 
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; 
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; 
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout() 
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL; 
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0; 
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) 
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode; 
	d3dShaderByteCode.BytecodeLength = 0; 
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다. 
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{ 
	UINT nCompileFlags = 0;
#if defined(_DEBUG) 
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION; 
#endif
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, ppd3dShaderBlob);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다. 
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc; 
	
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC)); 
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature; 
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob); 
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState(); 
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState(); 
	d3dPipelineStateDesc.InputLayout = CreateInputLayout(); 
	d3dPipelineStateDesc.SampleMask = UINT_MAX; 
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1; 
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release(); 
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) 
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{ 
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]); 
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList); 
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) 
{
}
void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) 
{
	XMFLOAT4X4 xmf4x4World; 
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 
}
void CShader::ReleaseShaderVariables() 
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////

CPlayerShader::CPlayerShader()
{
}
CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout() 
{
	UINT nInputElementDescs = 2; 
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }; 
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc; 
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs; 
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(((WCHAR*)(L"Shaders.hlsl")), "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(((WCHAR*)(L"Shaders.hlsl")), "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}


void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////
CObjectsShader::CObjectsShader() 
{
}
CObjectsShader::~CObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout() 
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc; 
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(((WCHAR*)(L"Shaders.hlsl")), "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(((WCHAR*)(L"Shaders.hlsl")), "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) 
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

CMesh *CObjectsShader::m_pRedCubeMesh = NULL;

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) 
{
	srand((unsigned int)time(NULL));
	
	//벽
	{
		float fWidth = 300;
		float fHeight = 300;
		float fDepth = 1000;

		float fHalfWidth = fWidth * 0.5f;
		float fHalfHeight = fHeight * 0.5f;
		float fHalfDepth = fDepth * 0.5f;

		//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
		CWallMesh *pCubeMeshWhiteStart = new CWallMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 101);
		CWallMesh *pCubeMeshWhiteEnd = new CWallMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 103);
		CWallMesh *pCubeMeshWhiteMid = new CWallMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 102);
		CWallMesh *pCubeMeshBlack = new CWallMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(0, 0, 0, 1), 102);

		m_nWalls = 13;
		m_ppWallObjects = new CWallsObject*[13];

		CWallsObject *pWallObject = NULL;

		/*pWallObject = new CWallsObject();
		pWallObject->SetMesh(pCubeMeshWhiteStart);
		pWallObject->SetPosition(0, 0,0);
		m_ppObjects[0] = pWallObject;*/

		int k = 0;
		int nDistanceZero = 0;
		int DepthGap = 45;

		for (int i = 0; i < 13; i++)
		{
			pWallObject = new CWallsObject();
			if (i == 0)
			{
				pWallObject->SetMesh(pCubeMeshWhiteEnd);
				pWallObject->SetPosition(0, 0, -455 + (nDistanceZero * (2 * DepthGap + 1)) - DepthGap);
			}
			else if (i == 12)
			{
				pWallObject->SetMesh(pCubeMeshWhiteStart);
				pWallObject->SetPosition(0, 0, -455 + (nDistanceZero * (2 * DepthGap + 1)) + DepthGap);
			}
			else
			{
				if (i % 2 == 1)
					pWallObject->SetMesh(pCubeMeshWhiteMid);
				else
					pWallObject->SetMesh(pCubeMeshBlack);
				pWallObject->SetPosition(0, 0, -455 + (nDistanceZero * 91));
				if (i < 11)
					nDistanceZero++;
			}
			pWallObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			pWallObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
			pWallObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
			pWallObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
			pWallObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
			pWallObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
			pWallObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
			//각 정육면체 객체의 위치를 설정한다.
			m_ppWallObjects[k++] = pWallObject;
		}
	}

	//오브젝트
	{
		CCubeMeshColorful *pObjectCubeMesh = new CCubeMeshColorful(pd3dDevice, pd3dCommandList, 16, 16, 16);
		pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		m_nObjects = 0;
		m_ppObjects = new CGameObject*[500];

		CGameObject *pRotatingObject = NULL;

		for (int i = 0; i < 500; i++)
		{
			pRotatingObject = new CExplosiveObject();
			pRotatingObject->SetMesh(pObjectCubeMesh);
			//각 정육면체 객체의 위치를 설정한다.
			pRotatingObject->SetPosition(0, 0, 100);
			pRotatingObject->SetRotationAxis(XMFLOAT3(1, 0, rand() % 2));
			pRotatingObject->SetRotationSpeed(50);
			pRotatingObject->SetMovingSpeed(20.0f);
			pRotatingObject->SetMovingDirection(XMFLOAT3(1, rand() % 2, rand() % 2));
			//추가
			pRotatingObject->m_bRenderFlag = true;
			pRotatingObject->m_pxmf4Planes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 8);	//왼
			pRotatingObject->m_pxmf4Planes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 8);	//오
			pRotatingObject->m_pxmf4Planes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, 8);	//아래
			pRotatingObject->m_pxmf4Planes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, 8);	//위
			pRotatingObject->m_pxmf4Planes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, 8);	//앞
			pRotatingObject->m_pxmf4Planes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, 8);	//뒤
			m_ppObjects[i] = pRotatingObject;
		}

		//////////////////////////////////////////////////////////////////////////////////////////
		////////////////
		//레드오브젝트
		m_pRedCubeMesh = new CCubeMeshSimilar(pd3dDevice, pd3dCommandList, 16, 16, 16, XMFLOAT4(1, 0, 0, 1));
		m_pRedCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	//총알
	{
		CCubeMeshDiffused *pObjectCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4, 4, 4, XMFLOAT4((float)155/255, 0.0f, 0.0f, 1.0f));
		pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		CCubeMeshDiffused *pObjectBossMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4, 4, 4, XMFLOAT4(0.0f, (float)155 / 255, (float)155 / 255, 1.0f));
		pObjectBossMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		m_nPlayerBullets = 0;
		m_pPlayerBullets = new CGameObject[20];

		m_nBossBullets = 0;
		m_pBossBullets = new CGameObject[5];

		for (int i = 0; i < 20; i++)
		{
			m_pPlayerBullets[i].SetMesh(pObjectCubeMesh);
			//각 정육면체 객체의 위치를 설정한다.
			m_pPlayerBullets[i].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
			m_pPlayerBullets[i].SetRotationSpeed(350);
			m_pPlayerBullets[i].SetMovingSpeed(250.0f);
		}

		for (int i = 0; i < 5; i++)
		{
			m_pBossBullets[i].SetMesh(pObjectBossMesh);
			//각 정육면체 객체의 위치를 설정한다.
			m_pBossBullets[i].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
			m_pBossBullets[i].SetRotationSpeed(350);
			m_pBossBullets[i].SetMovingSpeed(50.0f);
		}
	}
	{
		//보스
		CCubeMeshRandom *pBossMesh = new CCubeMeshRandom(pd3dDevice, pd3dCommandList, 30, 30, 30);
		pBossMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(15.0f, 15.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		m_pBossObject = new CBossObject;

		m_pBossObject->SetMesh(pBossMesh);
		m_pBossObject->SetMovingSpeed(100.0f);
		m_pBossObject->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		m_pBossObject->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects) 
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j]; 
		}
		delete[] m_ppObjects;
	}

	if (m_ppWallObjects)
	{
		for (int j = 0; j < m_nWalls; j++)
		{
			if (m_ppWallObjects[j]) delete m_ppWallObjects[j];
		}
		delete[] m_ppWallObjects;
	}

	if (m_pPlayerBullets)
		delete[] m_pPlayerBullets;
	
	if (m_pBossObject)
		delete m_pBossObject;
}

void CObjectsShader::AnimateObjects(float fTimeElapsed) 
{
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]->m_bRenderFlag)
			m_ppObjects[j]->Animate(fTimeElapsed);
	}
	
	for (int j = 0; j < m_nPlayerBullets; j++)
	{
		if(m_pPlayerBullets[j].m_bRenderFlag)
			m_pPlayerBullets[j].Animate(fTimeElapsed);
	}

	if (m_pBossObject->m_bRenderFlag)
	{
		m_pBossObject->Animate(fTimeElapsed);

		for (int j = 0; j < m_nBossBullets; j++)
		{
			if (m_pBossBullets[j].m_bRenderFlag)
				m_pBossBullets[j].Animate(fTimeElapsed);
		}
	}
}

void CObjectsShader::ReleaseUploadBuffers() 
{
	if (m_ppObjects) 
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers(); 
	}
	
	if (m_ppWallObjects)
	{
		for (int j = 0; j < m_nWalls; j++) m_ppWallObjects[j]->ReleaseUploadBuffers();
	}
	if (m_pPlayerBullets)
	{
		for (int j = 0; j < m_nPlayerBullets; j++) m_pPlayerBullets[j].ReleaseUploadBuffers();
	}
	if (m_pBossBullets)
	{
		for (int j = 0; j < m_nBossBullets; j++) m_pBossBullets[j].ReleaseUploadBuffers();
	}
	if (m_pBossObject)
	{
		m_pBossObject->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]->m_bRenderFlag)
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
	
	for (int j = 0; j < m_nWalls; j++)
	{
		if (m_ppWallObjects[j])
		{
			m_ppWallObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int j = 0; j < m_nPlayerBullets; j++)
	{
		if (m_pPlayerBullets[j].m_bRenderFlag)
		{
			m_pPlayerBullets[j].Render(pd3dCommandList, pCamera);
		}
	}
	for (int j = 0; j < m_nBossBullets; j++)
	{
		if (m_pBossBullets[j].m_bRenderFlag)
		{
			m_pBossBullets[j].Render(pd3dCommandList, pCamera);
		}
	}
	if(m_pBossObject->m_bRenderFlag)
		m_pBossObject->Render(pd3dCommandList, pCamera);
}

CGameObject** CObjectsShader::GetObjects()
{
	return m_ppObjects;
}

CWallsObject** CObjectsShader::GetWalls()
{
	return m_ppWallObjects;
}

