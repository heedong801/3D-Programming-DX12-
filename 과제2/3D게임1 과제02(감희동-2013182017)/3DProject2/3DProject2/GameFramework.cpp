#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dRenderTargetBuffers[i] = NULL;
	m_pd3dRtvDescriptorHeap = NULL;
	m_nRtvDescriptorIncrementSize = 0;

	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDsvDescriptorHeap = NULL;
	m_nDsvDescriptorIncrementSize = 0;

	m_nSwapChainBufferIndex = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	m_pScene = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("WindowsProject ("));
}


CGameFramework::~CGameFramework()
{
}

//���� �Լ��� ���� ���α׷��� ����Ǿ� �� �����찡 �����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	BuildObjects();

	return(true);
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS 
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� �ٲ��� �ʰ� �ĸ������ ũ�⸦ ����ȭ�� ũ��� �����Ѵ�.
	dxgiSwapChainDesc.Flags = 0;
#else 
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�. 
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	//::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	//dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	//dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	//m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);
	////����ü���� �����Ѵ�.

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//��Alt+Enter�� Ű�� ������ ��Ȱ��ȭ�Ѵ�.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
}

void CGameFramework::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);
	//DXGI ���丮�� �����Ѵ�.

	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i,
		&pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}
	//��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void
			**)&m_pd3dDevice);
	}
	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.

	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void
		**)&m_pd3dFence);
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�). �̺�Ʈ�� ����Ǹ�(Signal) ��
	��Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/

	//m_d3dViewport.TopLeftX = 0;
	//m_d3dViewport.TopLeftY = 0;
	//m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dViewport.MinDepth = 0.0f;
	//m_d3dViewport.MaxDepth = 1.0f;
	////����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	//m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	////���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		_uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);
	//����(Direct) ��� ť�� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	//����(Direct) ��� �Ҵ��ڸ� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void
			**)&m_pd3dCommandList);
	//����(Direct) ��� ����Ʈ�� �����Ѵ�.

	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	//����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
}

void CGameFramework::CreateRenderTargetView()
{
	HRESULT hResult;
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void
			**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1)
		: 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);
	//����-���ٽ� ���۸� �����Ѵ�.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
	//����-���ٽ� ���� �並 �����Ѵ�.
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	m_pScene = new CScene(); 
	
	if (m_pScene)
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature()); 

	m_pPlayer = pAirplanePlayer;
	
	m_pCamera = m_pPlayer->GetCamera();
	m_pScene->m_pPlayer = m_pPlayer;

	m_pd3dCommandList->Close(); 
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete(); 

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	
	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
		m_pScene->ReleaseObjects();
	if (m_pScene)
		delete m_pScene;
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		/*��F1�� Ű�� ������ 1��Ī ī�޶�,
		��F2�� Ű�� ������ �����̽�-�� ī�޶�� �����Ѵ�, 
		��F3�� Ű�� ������ 3��Ī ī�޶� �� �����Ѵ�.*/
		case VK_F1:   
		case VK_F2:   
		case VK_F3:                    
			if (m_pPlayer)
			{
				m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
				m_pCamera = m_pPlayer->GetCamera();
			}
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
			//��F9�� Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�. 
		case VK_F9:
		{
			BOOL bFullScreenState = FALSE;
			m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);

			if (!bFullScreenState)
			{
				DXGI_MODE_DESC dxgiTargetParameters;
				dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				dxgiTargetParameters.Width = m_nWndClientWidth;
				dxgiTargetParameters.Height = m_nWndClientHeight;
				dxgiTargetParameters.RefreshRate.Numerator = 60;
				dxgiTargetParameters.RefreshRate.Denominator = 1;
				dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
			}
			m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
			OnResizeBackBuffers();
			break;
		}
		case VK_SPACE:
			if (m_pPlayer->m_bDoPick)
			{
				m_pPlayer->m_bDoPick = false;
				m_pPlayer->m_nPickingIndex = -1;
			}
			else
				m_pPlayer->m_bDoPick = true;
			break;
		case 'Z':
			if (m_pPlayer->m_nBoom > 0)
			{
				for (int i = 0; i < m_pScene->m_pShaders[0].m_nObjects; i++)
				{
					CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pScene->m_pShaders[0].m_ppObjects[i];

					if (pExplosiveObject->m_bDeathFlag == false)
					{
						if (Vector3::Length(Vector3::Subtract(m_pPlayer->m_xmf3Position, m_pScene->m_pShaders[0].m_ppObjects[i]->GetPosition())) <= 100)
						{
							m_pScene->m_nAttackedObject++;
							pExplosiveObject->m_bBlowingUp = true;
						}
					}
				}
				m_pPlayer->m_nBoom--;
			}
			break;
		case 'R':
			m_pScene->GameReset();
		default:
			break;
		}
	default:
		break;
	}
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�.
		::ReleaseCapture();
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	/*WM_SIZE �޽����� �����찡 ������ �� �ѹ� ȣ��ǰų� �������� ũ�Ⱑ ����� �� ȣ��ȴ�.
	�� �������� ũ�� �� ����ڰ� ������ �� �����Ƿ� �������� ũ�Ⱑ ����Ǵ� ���� ������ ���� ��üȭ�� ����� ��ȯ�� �߻��� �� �̴�.*/
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		//�������� ũ�Ⱑ ����Ǹ� �ĸ������ ũ�⸦ �����Ѵ�. 
		OnResizeBackBuffers();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256]; 
	DWORD dwDirection = 0; 
	/*Ű������ ���� ������ ��ȯ�Ѵ�. 
	ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(���� x-��), ��/ ��(���� z-��)�� �̵��Ѵ�. 
	��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(���� y-��)�� �̵��Ѵ�.*/ 
	if (::GetKeyboardState(pKeyBuffer)) 
	{
		if (pKeyBuffer[0x57] & 0xF0)
			dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[0x53] & 0xF0)
			dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[0x41] & 0xF0)
			dwDirection |= DIR_LEFT;
		if (pKeyBuffer[0x44] & 0xF0)
			dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[0x51] & 0xF0)
			dwDirection |= DIR_UP;
		if (pKeyBuffer[0x45] & 0xF0)
			dwDirection |= DIR_DOWN;
		if (GetKeyState(VK_CONTROL) & 0x80000000)
		{
			static int ShotDelay = 0;
			bool bReload = true;
			ShotDelay += 1;

			if (ShotDelay == 30)
			{
				if (m_pScene->m_pShaders[0].m_nPlayerBullets < 20)
				{
					m_pScene->m_pShaders[0].m_pPlayerBullets[m_pScene->m_pShaders[0].m_nPlayerBullets].m_bRenderFlag = true;
					m_pScene->m_pShaders[0].m_pPlayerBullets[m_pScene->m_pShaders[0].m_nPlayerBullets].SetMovingDirection(m_pScene->m_pPlayer->m_xmf3Look);
					m_pScene->m_pShaders[0].m_pPlayerBullets[m_pScene->m_pShaders[0].m_nPlayerBullets].SetPosition(m_pPlayer->m_xmf3Position.x + 4 * m_pPlayer->GetUp().x, m_pPlayer->m_xmf3Position.y + 4 * m_pPlayer->GetUp().y, m_pPlayer->m_xmf3Position.z + 4 * m_pPlayer->GetUp().z);
					m_pScene->m_pShaders[0].m_pPlayerBullets[m_pScene->m_pShaders[0].m_nPlayerBullets].m_xmf3RememberPlayerPosition = m_pPlayer->m_xmf3Position;
					m_pScene->m_pShaders[0].m_nPlayerBullets++;
				}
				else
				{
					for (int i = 0; i < 20; i++)
						if (m_pScene->m_pShaders[0].m_pPlayerBullets[i].m_bRenderFlag == true)		//�Ѿ��� 1���� �Ⱦ������ٸ�
						{
							bReload = false;		//��������
							break;
						}

					if (bReload)	//�� �������� ��� ������ �ǰ�
						m_pScene->m_pShaders[0].m_nPlayerBullets = 0;
				}
			}
			else if (ShotDelay > 30 )
				ShotDelay = 0;
		}
	}
	float cxDelta = 0.0f, cyDelta = 0.0f; 
	POINT ptCursorPos; 

	XMFLOAT3 ptPickingPos;

	if (m_pPlayer->m_nPickingIndex != -1)		//��ŷ �ε����� �ʱⰪ�� -1 �̹Ƿ� �� ���� ���� ��ŷ�� �Ǿ��ٴ� ��
	{
		if(m_pPlayer->m_nPickingIndex == -2 )
			m_pPlayer->SetLookAt(m_pScene->m_pShaders[0].m_pBossObject->GetPosition(), m_pPlayer->GetUpVector());
		else
			m_pPlayer->SetLookAt(m_pScene->m_pShaders[0].m_ppObjects[m_pPlayer->m_nPickingIndex]->GetPosition(), m_pPlayer->GetUpVector());
	}
	/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�. 
	���콺 ���� �Ǵ� ������ ��ư�� ������ ���� �޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���. 
	�׷��Ƿ� ���콺�� ĸ�ĵ� ���� ���콺 ��ư�� ������ ���¸� �ǹ��Ѵ�. 
	���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� �÷��̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/ 
	if (::GetCapture() == m_hWnd)
	{
		//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
		::SetCursor(NULL); 
		//���� ���콺 Ŀ���� ��ġ�� �����´�.
		::GetCursorPos(&ptCursorPos);

		if (m_pScene->m_pPlayer->m_bDoPick)
		{
			::ScreenToClient(m_hWnd, &ptCursorPos);

			//ȭ�� -> ����
			ptPickingPos.x = ((2.0f * (ptCursorPos.x - m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX)) / m_pPlayer->m_pCamera->m_d3dViewport.Width) - 1;
			ptPickingPos.y = (-(2.0f * (ptCursorPos.y - m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY)) / m_pPlayer->m_pCamera->m_d3dViewport.Height) + 1;

			//���� -> ī�޶�
			ptPickingPos.x = ptPickingPos.x / m_pPlayer->m_pCamera->m_xmf4x4Projection._11;
			ptPickingPos.y = ptPickingPos.y / m_pPlayer->m_pCamera->m_xmf4x4Projection._22;
			ptPickingPos.z = 1.0f;

			//ī�޶� -> ����
			XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&m_pPlayer->m_pCamera->m_xmf4x4View));
			XMMATRIX ViewTrans = XMMatrixInverse(&det, XMLoadFloat4x4(&m_pPlayer->m_pCamera->m_xmf4x4View));

			ptPickingPos = Vector3::TransformCoord(ptPickingPos, ViewTrans);
			ptPickingPos = Vector3::Subtract(ptPickingPos, m_pPlayer->m_pCamera->m_xmf3Position);

			XMFLOAT3 xmf3Raylen = ptPickingPos;
			ptPickingPos = Vector3::Normalize(ptPickingPos);

			XMFLOAT3 xmf3StartRayPos = m_pPlayer->m_pCamera->m_xmf3Position;
			XMFLOAT3 xmf3Ray = xmf3StartRayPos;

			bool pickCheck = false;

			while (Vector3::Length(xmf3Raylen) < 200)
			{
				if (m_pScene->m_pShaders[0].m_pBossObject->m_bRenderFlag)
				{
					if (m_pScene->m_pShaders[0].m_pBossObject->m_xmOOBB.Contains(XMLoadFloat3(&xmf3Ray)) != DISJOINT)
					{
						m_pPlayer->m_nPickingIndex = -2;
						break;
					}
				}

				for (int i = 0; i < m_pScene->m_pShaders[0].m_nObjects; i++)
				{
					if (m_pScene->m_pShaders[0].m_ppObjects[i]->m_xmOOBB.Contains(XMLoadFloat3(&xmf3Ray)) != DISJOINT)
					{
						CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pScene->m_pShaders[0].m_ppObjects[i];
						if (pExplosiveObject->m_bDeathFlag == false)
						{
							m_pPlayer->m_nPickingIndex = i;
							pickCheck = true;

							XMVECTOR xmvresult;
							XMFLOAT4 xmfresult;

							XMMATRIX WorldInverse;

							XMVECTOR det2 = XMMatrixDeterminant(XMLoadFloat4x4(&m_pScene->m_pShaders[0].m_ppObjects[i]->m_xmf4x4World));
							XMFLOAT3 ModelRayPos;

							float Distance = 0;

							for (int k = 0; k < 6; k++)
							{
								xmvresult = XMLoadFloat4(&m_pScene->m_pShaders[0].m_ppObjects[i]->m_pxmf4Planes[k]);
								WorldInverse = XMMatrixInverse(&det2, XMLoadFloat4x4(&m_pScene->m_pShaders[0].m_ppObjects[i]->m_xmf4x4World));
							
								ModelRayPos = Vector3::TransformCoord(xmf3Ray, WorldInverse);
								XMStoreFloat4(&xmfresult, xmvresult);

								XMStoreFloat4(&xmfresult, ::XMPlaneDotCoord(xmvresult, XMLoadFloat3(&ModelRayPos)));
								Distance = xmfresult.x;

								if (m_pScene->m_pShaders[0].m_ppObjects[i]->m_nPickMeshDistance > Distance)
								{
									pExplosiveObject->m_nPickingPlaneColor = k;
									m_pScene->m_pShaders[0].m_ppObjects[i]->m_nPickMeshDistance = Distance;
								}
							}
							break;
						}
					}
				}
				if (m_pPlayer->m_nPickingIndex != -1 && pickCheck == true)
					break;
				xmf3Ray = Vector3::Add(xmf3Ray, ptPickingPos);
				xmf3Raylen = Vector3::Subtract(xmf3Ray, xmf3StartRayPos);
			}
			::ClientToScreen(m_hWnd, &ptCursorPos);
		}
		//���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�.
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		
	}
	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		static DWORD prevPlayreDir;
		CWallsObject** tmpObject = m_pScene->GetShaders()->GetWalls();

		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (tmpObject[5]->m_xmOOBB.Contains(m_pPlayer->m_xmOOBB) == CONTAINS)
		{
			prevPlayreDir = dwDirection;
			if (dwDirection)
				m_pPlayer->Move(dwDirection, 1.5f);

			for (int i = 0; i < m_pScene->m_pShaders[0].m_nObjects; i++)
			{
				if( m_pScene->m_nLiveObject > 45)
					m_pScene->m_nPresentPlayerColor = 3;
				else if (m_pPlayer->m_xmf3Position.z > m_pScene->m_pShaders[0].m_ppObjects[i]->GetPosition().z)
				{
					m_pScene->m_nPresentPlayerColor = 1;
					break;
				}
				else
				{
					m_pScene->m_nPresentPlayerColor = 0;
				}
			}
		}
		else
		{
			m_pPlayer->Move(prevPlayreDir, -15.5f);
			m_pScene->m_nPresentPlayerColor = 2;
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}


void CGameFramework::AnimateObjects()
{
	//if (m_GameTimer.GetMobJenFlag() == true)
	//	m_pScene->m_bMobJen = true;
	//else
	//	m_pScene->m_bMobJen = false;

	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
	if (m_pPlayer) m_pPlayer->Animate(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	//CPU �潺�� ���� �����Ѵ�.

	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i]) m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS 
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else 
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = 0;
#endif 
	CreateRenderTargetView();
	CreateDepthStencilView();

	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP
void CGameFramework::FrameAdvance() 
{
	if (m_GameTimer.GetMobJenFlag())
		m_pScene->m_bMobJen = true;

	if (m_pScene->m_bGameEnd)
	{
		TCHAR buf1[4];
		TCHAR buf2[12];

		wsprintf(buf1, TEXT("END"), NULL);
		wsprintf(buf2, TEXT("GAME CLEAR"), NULL);

		if (MessageBox(m_hWnd, buf1, buf2, MB_OK) == IDOK)
			::PostQuitMessage(0);
	}

	if (m_pScene->m_bGameReset)
	{
		m_GameTimer.Reset();
		m_pScene->m_bGameReset = false;
	}


	m_GameTimer.Tick(0.0f);
	
	ProcessInput();
	
	AnimateObjects();
	
	HRESULT hResult = m_pd3dCommandAllocator->Reset(); 
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	
	D3D12_RESOURCE_BARRIER d3dResourceBarrier; 
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
	
	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	if (m_pScene)
		m_pScene->Render(m_pd3dCommandList, m_pCamera);
	//3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�. 
#ifdef _WITH_PLAYER_TOP 
	//���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�. 
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif 
	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�. 
	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; 
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; 
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; 
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList }; 
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	
	WaitForGpuComplete();
	
	m_pdxgiSwapChain->Present(0, 0);
	
	MoveToNextFrame();

	/*m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37); 
	::SetWindowText(m_hWnd, m_pszFrameRate);*/
	
	CCamera *pRememberCamera = NULL;
	pRememberCamera = m_pCamera;
	m_GameTimer.GetFrameRate(m_pszFrameRate + 16, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
	m_pCamera = pRememberCamera;

}
