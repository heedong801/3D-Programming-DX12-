//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
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

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
//	char *pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

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
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, MATERIAL *pMaterial)
{
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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

	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPlayer", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPlayer", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CPlayerShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, 
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbPlayer->Map(0, NULL, (void **)&m_pcbMappedPlayer);
}

void CPlayerShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayerShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	::memcpy(&m_pcbMappedPlayer->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
}

void CPlayerShader::ReleaseShaderVariables()
{
	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}
}

void CPlayerShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int Object)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	if (Object == 0)
	{
		m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * 200,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);


		m_pd3dcbBombObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * 4000,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbBombObjects->Map(0, NULL, (void **)&m_pcbMappedBombObjects);
	}
	else if ( Object == 1 )
	{
		m_pd3dcbWallObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * m_nWalls,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbWallObjects->Map(0, NULL, (void **)&m_pcbMappedWallObjects);
	}
	else if (Object == 2)
	{
		m_pd3dcbPlayerBullets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * 20,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbPlayerBullets->Map(0, NULL, (void **)&m_pcbMappedPlayerBullets);
	}
	else if (Object == 3)
	{
		m_pd3dcbBossBullets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * 5,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbBossBullets->Map(0, NULL, (void **)&m_pcbMappedBossBullets);
	}
	else if (Object == 4)
	{
		m_pd3dcbBossObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_pd3dcbBossObject->Map(0, NULL, (void **)&m_pcbMappedBossObject);
	}
}

void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, int Object, int index)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;

	if (Object == 0)	//적 오브젝트
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[index]->m_xmf4x4World)));
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)(m_pcbMappedGameObjects + (index * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbGameObject->m_nMaterial = m_ppObjects[index]->m_pMaterial->m_nReflection;
	}
	else if (Object == 1)	//벽 오브젝트
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppWallObjects[index]->m_xmf4x4World)));
		CB_GAMEOBJECT_INFO *pbMappedcbWallObject = (CB_GAMEOBJECT_INFO *)(m_pcbMappedWallObjects + (index * ncbGameObjectBytes));
		::memcpy(&pbMappedcbWallObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbWallObject->m_nMaterial = m_ppWallObjects[index]->m_pMaterial->m_nReflection;
	}
	else if (Object == 2)	//플레이어 총알
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pPlayerBullets[index].m_xmf4x4World)));
		CB_GAMEOBJECT_INFO *pbMappedcbPlayerBullet = (CB_GAMEOBJECT_INFO *)(m_pcbMappedPlayerBullets + (index * ncbGameObjectBytes));
		::memcpy(&pbMappedcbPlayerBullet->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbPlayerBullet->m_nMaterial = m_pPlayerBullets[index].m_pMaterial->m_nReflection;
	}
	else if (Object == 3)	//보스 총알
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pBossBullets[index].m_xmf4x4World)));
		CB_GAMEOBJECT_INFO *pbMappedcbBossBullet = (CB_GAMEOBJECT_INFO *)(m_pcbMappedBossBullets + (index * ncbGameObjectBytes));
		::memcpy(&pbMappedcbBossBullet->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbBossBullet->m_nMaterial = m_pBossBullets[index].m_pMaterial->m_nReflection;
	}
	else if (Object == 4)	//보스
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pBossObject->m_xmf4x4World)));
		CB_GAMEOBJECT_INFO *pbMappedcbBoss = (CB_GAMEOBJECT_INFO *)(m_pcbMappedBossObject + (index * ncbGameObjectBytes));
		::memcpy(&pbMappedcbBoss->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbBoss->m_nMaterial = m_pBossObject->m_pMaterial->m_nReflection;
	}
	
}

void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	if (m_pd3dcbWallObjects)
	{
		m_pd3dcbWallObjects->Unmap(0, NULL);
		m_pd3dcbWallObjects->Release();
	}
	if (m_pd3dcbBombObjects)
	{
		m_pd3dcbBombObjects->Unmap(0, NULL);
		m_pd3dcbBombObjects->Release();
	}
	if (m_pd3dcbPlayerBullets)
	{
		m_pd3dcbPlayerBullets->Unmap(0, NULL);
		m_pd3dcbPlayerBullets->Release();
	}
	if (m_pd3dcbBossBullets)
	{
		m_pd3dcbBossBullets->Unmap(0, NULL);
		m_pd3dcbBossBullets->Release();
	}
	if (m_pd3dcbBossObject)
	{
		m_pd3dcbBossObject->Unmap(0, NULL);
		m_pd3dcbBossObject->Release();
	}
}

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	srand((unsigned int)time(NULL));
	
	//오브젝트
	{
		CCubeMeshIlluminated *pCubeMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 16.0f, 16.0f, 16.0f);
		pCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		CCubeMeshIlluminatedSimilar *pRedCubeMesh = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 16, 16, 16, XMFLOAT4(1, 0, 0, 1));
		pRedCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		m_nObjects = 0;
		m_ppObjects = new CGameObject*[200];

		CGameObject *pRotatingObject = NULL;

		int j = 0;

		for (int i = 0; i < 200; i++)
		{
			pRotatingObject = new CExplosiveObject();
			if (i == 9 + 10 * j)
			{
				pRotatingObject->SetMesh(pRedCubeMesh);
				pRotatingObject->m_bRed = true;
				j++;
			}
			else
			{
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->m_bRed = false;
			}
			//각 정육면체 객체의 위치를 설정한다.
			pRotatingObject->SetPosition((float)(rand() % 250 - 125), (float)(rand() % 250 - 125), 100.0f);
			pRotatingObject->SetRotationAxis(XMFLOAT3(rand() % 2, 1, rand() % 2));
			pRotatingObject->SetRotationSpeed(150);
			pRotatingObject->SetMovingSpeed(25.0f);
			pRotatingObject->SetMovingDirection(XMFLOAT3(rand() % 2, rand() % 2, rand() % 2));
			//추가
			pRotatingObject->m_bRenderFlag = true;
			pRotatingObject->m_pxmf4Planes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 8);	//왼
			pRotatingObject->m_pxmf4Planes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 8);	//오
			pRotatingObject->m_pxmf4Planes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, 8);	//아래
			pRotatingObject->m_pxmf4Planes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, 8);	//위
			pRotatingObject->m_pxmf4Planes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, 8);	//앞
			pRotatingObject->m_pxmf4Planes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, 8);	//뒤
			pRotatingObject->SetMaterial((UINT)0);
			m_ppObjects[i] = pRotatingObject;
		}

		CreateShaderVariables(pd3dDevice, pd3dCommandList, 0);
	}
	//벽
	{
		float fWidth = 300.0f, fHeight = 300.0f, fDepth = 1000.0f;
		//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
		CWallMeshIlluminated *pCubeMeshWhiteStart = new CWallMeshIlluminated(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 101);
		CWallMeshIlluminated *pCubeMeshWhiteEnd = new CWallMeshIlluminated(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 103);
		CWallMeshIlluminated *pCubeMeshWhiteMid = new CWallMeshIlluminated(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(1, 1, 1, 1), 102);
		CWallMeshIlluminated *pCubeMeshBlack = new CWallMeshIlluminated(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, XMFLOAT4(0, 0, 0, 1), 102);

		m_nWalls = 13;
		m_ppWallObjects = new CWallsObject*[m_nWalls];

		CWallsObject *pWallObject = NULL;

		int k = 0;
		int nDistanceZero = 0;
		int DepthGap = 45;

		for (int i = 0; i < m_nWalls; i++)
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
			if( i % 2 == 1 )
				pWallObject->SetMaterial(static_cast<UINT>(4));
			else
				pWallObject->SetMaterial(static_cast<UINT>(5));
			pWallObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fWidth / 2, fHeight / 2, fDepth / 2), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			pWallObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fWidth/2);
			pWallObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fWidth/2);
			pWallObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHeight/2);
			pWallObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHeight/2);
			pWallObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fDepth/2);
			pWallObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fDepth/2);
			//각 정육면체 객체의 위치를 설정한다.
			m_ppWallObjects[k++] = pWallObject;
		}
		CreateShaderVariables(pd3dDevice, pd3dCommandList, 1);
	}
	//총알
	{
		CCubeMeshIlluminatedOneColor *pObjectCubeMesh = new CCubeMeshIlluminatedOneColor(pd3dDevice, pd3dCommandList, 4, 4, 4, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


		m_nPlayerBullets = 0;
		m_pPlayerBullets = new CGameObject[20];

		m_nBossBullets = 0;
		m_pBossBullets = new CGameObject[5];

		for (int i = 0; i < 20; i++)
		{
			m_pPlayerBullets[i].SetMesh(pObjectCubeMesh);
			m_pPlayerBullets[i].SetMaterial((UINT)1);
			//각 정육면체 객체의 위치를 설정한다.
			m_pPlayerBullets[i].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
			m_pPlayerBullets[i].SetRotationSpeed(350);
			m_pPlayerBullets[i].SetMovingSpeed(250.0f);
		}
		CreateShaderVariables(pd3dDevice, pd3dCommandList, 2);

		for (int i = 0; i < 5; i++)
		{
			m_pBossBullets[i].SetMesh(pObjectCubeMesh);
			m_pBossBullets[i].SetMaterial((UINT)2);
			//각 정육면체 객체의 위치를 설정한다.
			m_pBossBullets[i].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
			m_pBossBullets[i].SetRotationSpeed(350);
			m_pBossBullets[i].SetMovingSpeed(50.0f);
		}
		CreateShaderVariables(pd3dDevice, pd3dCommandList, 3);
	}
	//보스
	{
		//보스
		CCubeMeshIlluminatedSimilar *pBossMesh = new CCubeMeshIlluminatedSimilar(pd3dDevice, pd3dCommandList, 30, 30, 30, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		pBossMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(15.0f, 15.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		m_pBossObject = new CBossObject;

		m_pBossObject->SetMesh(pBossMesh);
		m_pBossObject->SetMovingSpeed(100.0f);
		m_pBossObject->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		m_pBossObject->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
		m_pBossObject->SetMaterial((UINT)3);

		CreateShaderVariables(pd3dDevice, pd3dCommandList, 4);
	}

}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < 200; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
	if (m_ppWallObjects)
	{
		for (int j = 0; j < m_nWalls; j++) if (m_ppWallObjects[j]) delete m_ppWallObjects[j];
		delete[] m_ppWallObjects;
	}

	if (m_pPlayerBullets)
		delete[] m_pPlayerBullets;

	if (m_pBossBullets)
		delete[] m_pBossBullets;

	if (m_pBossObject)
		delete m_pBossObject;
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
	for (int j = 0; j < m_nPlayerBullets; j++)
	{
		if (m_pPlayerBullets[j].m_bRenderFlag)
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
		for (int j = 0; j < 200; j++) 
			if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
	if (m_ppWallObjects)
	{
		for (int j = 0; j < m_nWalls; j++) 
			if (m_ppWallObjects[j])
				m_ppWallObjects[j]->ReleaseUploadBuffers();
	}
	if (m_pPlayerBullets)
	{
		for (int j = 0; j < 20; j++) m_pPlayerBullets[j].ReleaseUploadBuffers();
	}
	if (m_pBossBullets)
	{
		for (int j = 0; j < 5; j++) m_pBossBullets[j].ReleaseUploadBuffers();
	}
	if (m_pBossObject)
		m_pBossObject->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbWallObjectGpuVirtualAddress = m_pd3dcbWallObjects->GetGPUVirtualAddress();

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbPlayerBulletsGpuVirtualAddress = m_pd3dcbPlayerBullets->GetGPUVirtualAddress();

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbBossGpuVirtualAddress = m_pd3dcbBossObject->GetGPUVirtualAddress();

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbBossBulletsGpuVirtualAddress = m_pd3dcbBossBullets->GetGPUVirtualAddress();

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			UpdateShaderVariables(pd3dCommandList, 0, j);

			m_ppObjects[j]->m_d3dcbBombGpuVirtualAddress = m_pd3dcbBombObjects->GetGPUVirtualAddress();
			m_ppObjects[j]->m_pcbMappedBombObjects = m_pcbMappedBombObjects;

			pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
		if (j == m_nObjects - 1)
		{
			CExplosiveObject::m_nBoonindex = 0;
		}
	}
	
	for (int j = 0; j < m_nWalls; j++)
	{
		if (m_ppWallObjects[j])
		{
			UpdateShaderVariables(pd3dCommandList, 1, j);

			pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbWallObjectGpuVirtualAddress + (ncbGameObjectBytes * j));
			m_ppWallObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int j = 0; j < m_nPlayerBullets; j++)
	{
		if (m_pPlayerBullets[j].m_bRenderFlag)
		{
			UpdateShaderVariables(pd3dCommandList, 2, j);

			pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbPlayerBulletsGpuVirtualAddress + (ncbGameObjectBytes * j));
			m_pPlayerBullets[j].Render(pd3dCommandList, pCamera);
		}
	}
	if (m_pBossObject->m_bRenderFlag)
	{
		UpdateShaderVariables(pd3dCommandList, 4, 0);

		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbBossGpuVirtualAddress );
		m_pBossObject->Render(pd3dCommandList, pCamera);

		for (int i = 0; i < m_nBossBullets; i++)
		{
			UpdateShaderVariables(pd3dCommandList, 3, i);

			pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbBossBulletsGpuVirtualAddress + (ncbGameObjectBytes * i));
			m_pBossBullets[i].Render(pd3dCommandList, pCamera);
		}
	}
}

CWallsObject** CObjectsShader::GetWalls()
{
	return m_ppWallObjects;
}