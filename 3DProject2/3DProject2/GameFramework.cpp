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

//다음 함수는 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다는 것에 유의하라.
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
	//전체화면 모드에서 바탕화면의 해상도를 바꾸지 않고 후면버퍼의 크기를 바탕화면 크기로 변경한다.
	dxgiSwapChainDesc.Flags = 0;
#else 
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다. 
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
	////스왑체인을 생성한다.

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//“Alt+Enter” 키의 동작을 비활성화한다.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
}

void CGameFramework::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);
	//DXGI 팩토리를 생성한다.

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
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void
			**)&m_pd3dDevice);
	}
	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.

	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void
		**)&m_pd3dFence);
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	//펜스를 생성하고 펜스 값을 0으로 설정한다.

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다). 이벤트가 실행되면(Signal) 이
	벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/

	//m_d3dViewport.TopLeftX = 0;
	//m_d3dViewport.TopLeftY = 0;
	//m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dViewport.MinDepth = 0.0f;
	//m_d3dViewport.MaxDepth = 1.0f;
	////뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다.

	//m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	////씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다.

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
	//직접(Direct) 명령 큐를 생성한다.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	//직접(Direct) 명령 할당자를 생성한다.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void
			**)&m_pd3dCommandList);
	//직접(Direct) 명령 리스트를 생성한다.

	hResult = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
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
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
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
	//깊이-스텐실 버퍼를 생성한다.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
	//깊이-스텐실 버퍼 뷰를 생성한다.
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
		/*‘F1’ 키를 누르면 1인칭 카메라,
		‘F2’ 키를 누르면 스페이스-쉽 카메라로 변경한다, 
		‘F3’ 키를 누르면 3인칭 카메라 로 변경한다.*/
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
			//“F9” 키가 눌려지면 윈도우 모드와 전체화면 모드의 전환을 처리한다. 
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
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
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
	/*WM_SIZE 메시지는 윈도우가 생성될 때 한번 호출되거나 윈도우의 크기가 변경될 때 호출된다.
	주 윈도우의 크기 를 사용자가 변경할 수 없으므로 윈도우의 크기가 변경되는 경우는 윈도우 모드와 전체화면 모드의 전환이 발생할 때 이다.*/
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		//윈도우의 크기가 변경되면 후면버퍼의 크기를 변경한다. 
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
	/*키보드의 상태 정보를 반환한다. 
	화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/ 뒤(로컬 z-축)로 이동한다. 
	‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/ 
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
						if (m_pScene->m_pShaders[0].m_pPlayerBullets[i].m_bRenderFlag == true)		//총알이 1개라도 안없어졌다면
						{
							bReload = false;		//장전못해
							break;
						}

					if (bReload)	//다 없어졌을 경우 들어오게 되고
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

	if (m_pPlayer->m_nPickingIndex != -1)		//피킹 인덱스가 초기값이 -1 이므로 이 뜻은 뭔가 피킹이 되었다는 뜻
	{
		if(m_pPlayer->m_nPickingIndex == -2 )
			m_pPlayer->SetLookAt(m_pScene->m_pShaders[0].m_pBossObject->GetPosition(), m_pPlayer->GetUpVector());
		else
			m_pPlayer->SetLookAt(m_pScene->m_pShaders[0].m_ppObjects[m_pPlayer->m_nPickingIndex]->GetPosition(), m_pPlayer->GetUpVector());
	}
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 
	마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의 메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 
	그러므로 마우스가 캡쳐된 것은 마우스 버튼이 눌려진 상태를 의미한다. 
	마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플레이어를 x-축 또는 y-축으로 회전한다.*/ 
	if (::GetCapture() == m_hWnd)
	{
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		::SetCursor(NULL); 
		//현재 마우스 커서의 위치를 가져온다.
		::GetCursorPos(&ptCursorPos);

		if (m_pScene->m_pPlayer->m_bDoPick)
		{
			::ScreenToClient(m_hWnd, &ptCursorPos);

			//화면 -> 투영
			ptPickingPos.x = ((2.0f * (ptCursorPos.x - m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX)) / m_pPlayer->m_pCamera->m_d3dViewport.Width) - 1;
			ptPickingPos.y = (-(2.0f * (ptCursorPos.y - m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY)) / m_pPlayer->m_pCamera->m_d3dViewport.Height) + 1;

			//투영 -> 카메라
			ptPickingPos.x = ptPickingPos.x / m_pPlayer->m_pCamera->m_xmf4x4Projection._11;
			ptPickingPos.y = ptPickingPos.y / m_pPlayer->m_pCamera->m_xmf4x4Projection._22;
			ptPickingPos.z = 1.0f;

			//카메라 -> 월드
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
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다.
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다. 
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		
	}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
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
	//CPU 펜스의 값을 증가한다.

	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
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
	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 
#ifdef _WITH_PLAYER_TOP 
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다. 
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif 
	//3인칭 카메라일 때 플레이어를 렌더링한다. 
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
