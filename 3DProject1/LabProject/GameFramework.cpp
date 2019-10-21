//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    ::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer(); 

	BuildObjects(); 

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
    HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight);
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				default:
					m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
					break;
			}
			break;
		default:
			break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
		case WM_SIZE:
			break;
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

void CGameFramework::BuildObjects()
{
	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer = new CPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, -400.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
	m_pPlayer->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3, 3, 0.5), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pScene = new CScene();
	m_pScene->m_pPlayer = m_pPlayer;

	m_pScene->BuildObjects();

}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	DWORD dwDirection = 0;
	if (GetKeyboardState(pKeyBuffer))
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
			bool bReload = true;
			if (m_pScene->m_nPlayerBullets < 50)
			{
				m_pScene->m_pPlayerBulletObject[m_pScene->m_nPlayerBullets].SetMovingDirection(m_pScene->m_pPlayer->m_xmf3Look);
				m_pScene->m_pPlayerBulletObject[m_pScene->m_nPlayerBullets].SetPosition(m_pPlayer->m_xmf3Position.x, m_pPlayer->m_xmf3Position.y, m_pPlayer->m_xmf3Position.z);
				m_pScene->m_pPlayerBulletObject[m_pScene->m_nPlayerBullets].m_bRenderFlag = false;
				m_pScene->m_nPlayerBullets++;
			}
			else
			{
				if (m_pScene->m_nPlayerBullets == 50)		//총알을 다썼다면
				{
					for (int i = 0; i < 50; i++)
						if (m_pScene->m_pPlayerBulletObject[i].m_bRenderFlag == false)		//총알이 1개라도 안없어졌다면
						{
							bReload = false;		//장전못해
							break;
						}

					if (bReload)	//다 없어졌을 경우 들어오게 되고
						m_pScene->m_nPlayerBullets = 0;	
				}
			}
		}
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	float pxDelta = 0.0f, pyDelta = 0.0f;
	POINT ptCursorPos;
	XMFLOAT3 ptPickingPos;

	if (m_pPlayer->m_nPickingIndex != -1)		//피킹 인덱스가 초기값이 -1 이므로 이 뜻은 뭔가 피킹이 되었다는 뜻
	{
		if (m_pPlayer->m_nPickingIndex != -2)	//피킹 인덱스가 음수인 건 오브젝트가 아니라 보스가 피킹되었다는 의미, 현재 이것은 오브젝트 피킹
		{
			XMFLOAT3 xmf3ObjectVecX = m_pScene->m_ppObjects[m_pPlayer->m_nPickingIndex]->GetPosition();
			xmf3ObjectVecX.y = 0;
			XMFLOAT3 xmf3PlayerVecX = m_pPlayer->m_xmf3Position;
			xmf3PlayerVecX.y = 0;

			XMFLOAT3 xmf3ObjectVecY = m_pScene->m_ppObjects[m_pPlayer->m_nPickingIndex]->GetPosition();
			xmf3ObjectVecY.x = 0;
			XMFLOAT3 xmf3PlayerVecY = m_pPlayer->m_xmf3Position;
			xmf3PlayerVecY.x = 0;

			XMFLOAT3 xmf3PlayerObjectVecX = Vector3::Normalize(Vector3::Subtract(xmf3ObjectVecX, xmf3PlayerVecX));
			XMFLOAT3 xmf3PlayerObjectVecY = Vector3::Normalize(Vector3::Subtract(xmf3ObjectVecY, xmf3PlayerVecY));

			float deltaX = acos(Vector3::DotProduct(xmf3PlayerObjectVecX, m_pPlayer->m_xmf3Look));
			float deltaY = acos(Vector3::DotProduct(xmf3PlayerObjectVecY, m_pPlayer->m_xmf3Look));

			/*XMFLOAT3 deltax;
			XMStoreFloat3(&deltax, XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3PlayerObjectVecX), XMLoadFloat3(&m_pPlayer->m_xmf3Look)));
			XMVECTOR deltaVx = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3PlayerObjectVecY), XMLoadFloat3(&m_pPlayer->m_xmf3Look));*/
			
			//대각선벡터
			XMFLOAT3 xmf3LeftCheckVec = Vector3::Normalize(Vector3::Add(m_pPlayer->m_xmf3Look, Vector3::ScalarProduct(m_pPlayer->m_xmf3Right, -1, false)));
			XMFLOAT3 xmf3RightCheckVec = Vector3::Normalize(Vector3::Add(m_pPlayer->m_xmf3Look, m_pPlayer->m_xmf3Right));
			XMFLOAT3 xmf3UpCheckVec = Vector3::Normalize(Vector3::Add(m_pPlayer->m_xmf3Look, m_pPlayer->m_xmf3Up));
			XMFLOAT3 xmf3DownCheckVec = Vector3::Normalize(Vector3::Add(m_pPlayer->m_xmf3Look, Vector3::ScalarProduct(m_pPlayer->m_xmf3Up, -1, false)));
			XMFLOAT3 xmf3PlayerObjVec = Vector3::Normalize(Vector3::Subtract(m_pScene->m_ppObjects[m_pPlayer->m_nPickingIndex]->GetPosition(), m_pPlayer->m_xmf3Position));

			float fLeftLen = Vector3::Length(Vector3::Subtract(xmf3LeftCheckVec, xmf3PlayerObjVec));
			float fRightLen = Vector3::Length(Vector3::Subtract(xmf3RightCheckVec, xmf3PlayerObjVec));
			float fUpLen = Vector3::Length(Vector3::Subtract(xmf3UpCheckVec, xmf3PlayerObjVec));
			float fDownLen = Vector3::Length(Vector3::Subtract(xmf3DownCheckVec, xmf3PlayerObjVec));

			if (deltaX >= Sensitivity && deltaY >= Sensitivity)			//Sensitivity란 얼마나 정확하게 고개를 맞출지에 대한 감도이다.
			{
				if (fLeftLen < fRightLen)
					m_pPlayer->Rotate(0.0f, -deltaX / 30, 0.0f, true);
				else
					m_pPlayer->Rotate(0.0f, deltaX / 30, 0.0f, true);
				if (fUpLen < fDownLen)
					m_pPlayer->Rotate(-deltaY / 30, 0.0f, 0.0f, true);
				else
					m_pPlayer->Rotate(deltaY / 30, 0.0f, 0.0f, true);
			}
			else
			{
				if (0 < deltaX && deltaX < Sensitivity && 0 < deltaY && deltaY < Sensitivity)
				{

				}
				else
				{
					if (0 < deltaX && deltaX < Sensitivity)
					{
						if (fUpLen < fDownLen)
							m_pPlayer->Rotate(-deltaY / 30, 0.0f, 0.0f, true);
						else
							m_pPlayer->Rotate(deltaY / 30, 0.0f, 0.0f, true);
					}
					if (0 < deltaY && deltaY < Sensitivity)
					{
						if (fLeftLen < fRightLen)
							m_pPlayer->Rotate(0.0f, -deltaX / 30, 0.0f, true);
						else
							m_pPlayer->Rotate(0.0f, deltaX / 30, 0.0f, true);
					}
				}
			}
		}
		else
		{
			XMFLOAT3 xmf3ObjectVecX = m_pScene->m_pBossObject->GetPosition();
			xmf3ObjectVecX.y = 0;
			XMFLOAT3 xmf3PlayerVecX = m_pPlayer->m_xmf3Position;
			xmf3PlayerVecX.y = 0;

			XMFLOAT3 xmf3ObjectVecY = m_pScene->m_pBossObject->GetPosition();
			xmf3ObjectVecY.x = 0;
			XMFLOAT3 xmf3PlayerVecY = m_pPlayer->m_xmf3Position;
			xmf3PlayerVecY.x = 0;

			XMFLOAT3 xmf3PlayerObjectVecX = Vector3::Normalize(Vector3::Subtract(xmf3ObjectVecX, xmf3PlayerVecX));
			XMFLOAT3 xmf3PlayerObjectVecY = Vector3::Normalize(Vector3::Subtract(xmf3ObjectVecY, xmf3PlayerVecY));

			float deltaX = acos(Vector3::DotProduct(xmf3PlayerObjectVecX, m_pPlayer->m_xmf3Look));
			float deltaY = acos(Vector3::DotProduct(xmf3PlayerObjectVecY, m_pPlayer->m_xmf3Look));

			/*XMFLOAT3 feltaX;
			XMStoreFloat3(&feltaX, XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3PlayerObjectVecX), XMLoadFloat3(&m_pPlayer->m_xmf3Look)));
			XMVECTOR feltaY = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3PlayerObjectVecY), XMLoadFloat3(&m_pPlayer->m_xmf3Look));*/

			XMFLOAT3 xmf3LeftCheckVec = Vector3::Add(m_pPlayer->m_xmf3Look, Vector3::ScalarProduct(m_pPlayer->m_xmf3Right, -1, false));
			XMFLOAT3 xmf3RightCheckVec = Vector3::Add(m_pPlayer->m_xmf3Look, m_pPlayer->m_xmf3Right);
			XMFLOAT3 xmf3UpCheckVec = Vector3::Add(m_pPlayer->m_xmf3Look, Vector3::ScalarProduct(m_pPlayer->m_xmf3Up, -1, false));
			XMFLOAT3 xmf3DownCheckVec = Vector3::Add(m_pPlayer->m_xmf3Look, m_pPlayer->m_xmf3Up);
			XMFLOAT3 xmf3PlayerObjVec = Vector3::Subtract(m_pScene->m_pBossObject->GetPosition(), m_pPlayer->m_xmf3Position);

			float fLeftLen = Vector3::Length(Vector3::Subtract(xmf3LeftCheckVec, xmf3PlayerObjVec));
			float fRightLen = Vector3::Length(Vector3::Subtract(xmf3RightCheckVec, xmf3PlayerObjVec));
			float fUpLen = Vector3::Length(Vector3::Subtract(xmf3UpCheckVec, xmf3PlayerObjVec));
			float fDownLen = Vector3::Length(Vector3::Subtract(xmf3DownCheckVec, xmf3PlayerObjVec));

			if (deltaX >= 0.1 && deltaY >= 0.1)
			{
				if (fLeftLen > fRightLen)
					m_pPlayer->Rotate(0.0f, deltaX / 30, 0.0f, true);
				else
					m_pPlayer->Rotate(0.0f, -deltaX / 30, 0.0f, true);
				if (fUpLen > fDownLen)
					m_pPlayer->Rotate(-deltaY / 30, 0.0f, 0.0f, true);
				else
					m_pPlayer->Rotate(deltaY / 30, 0.0f, 0.0f, true);
			}
			else
			{
				if (0 < deltaX && deltaX < 0.1 && 0 < deltaY && deltaY < 0.1)
				{

				}
				else
				{
					if (0 < deltaX && deltaX < 0.1)
					{
						if (fUpLen > fDownLen)
							m_pPlayer->Rotate(-deltaY / 30, 0.0f, 0.0f, true);
						else
							m_pPlayer->Rotate(deltaY / 30, 0.0f, 0.0f, true);
					}
					if (0 < deltaY && deltaY < 0.1)
					{
						if (fLeftLen > fRightLen)
							m_pPlayer->Rotate(0.0f, deltaX / 30, 0.0f, true);
						else
							m_pPlayer->Rotate(0.0f, -deltaX / 30, 0.0f, true);
					}
				}
			}
		}
	}

	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);

		if (m_pScene->m_pPlayer->m_bDoPick)
		{
			::ScreenToClient(m_hWnd, &ptCursorPos);

			//화면 -> 투영
			ptPickingPos.x = ((2.0f * (ptCursorPos.x - m_pPlayer->m_pCamera->m_Viewport.m_xStart)) / m_pPlayer->m_pCamera->m_Viewport.m_nWidth) - 1;
			ptPickingPos.y = (-(2.0f * (ptCursorPos.y - m_pPlayer->m_pCamera->m_Viewport.m_yStart)) / m_pPlayer->m_pCamera->m_Viewport.m_nHeight) + 1;

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

			int pickIndex = -1;
			bool pickCheck = false;

			while (Vector3::Length(xmf3Raylen) < 200)
			{
				if (m_pScene->m_pBossObject->m_xmOOBB.Contains(XMLoadFloat3(&xmf3Ray)) != DISJOINT)
				{
					m_pPlayer->m_nPickingIndex = -2;
					break;
				}

				for (int i = 0; i < m_pScene->m_nObjects; i++)
				{
					if (m_pScene->m_ppObjects[i]->m_xmOOBB.Contains(XMLoadFloat3(&xmf3Ray)) != DISJOINT)
					{
						CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_pScene->m_ppObjects[i];
						if (pExplosiveObject->m_bDeathFlag == false)
						{
							m_pPlayer->m_nPickingIndex = i;
							pickCheck = true;
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

		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		static DWORD prevPlayreDir;
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (m_pScene->m_pWallsObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == CONTAINS)
		{
			prevPlayreDir = dwDirection;
			if (dwDirection)
				m_pPlayer->Move(dwDirection, 0.15f);
			m_pPlayer->SetColor(RGB(0, 0, 255));
		}
		else
		{
			m_pPlayer->Move(prevPlayreDir, -0.45f);
			m_pPlayer->SetColor(RGB(255, 0, 0));
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;
	if (m_pScene->m_bGameEnd)
	{
		TCHAR buf1[4];
		TCHAR buf2[12];

		wsprintf(buf1, TEXT("END"), NULL);
		wsprintf(buf2, TEXT("GAME CLEAR"), NULL);

		if (MessageBox(m_hWnd, buf1, buf2, MB_OK) == IDOK)
			::PostQuitMessage(0);

	}
	if (m_pScene->NotRedPeriodStart == -1)
		m_pScene->NotRedPeriodStart = clock();

	m_GameTimer.Tick(0.0f);

	ProcessInput();

	m_pScene->Animate(m_GameTimer.GetTimeElapsed());
	
	ClearFrameBuffer(RGB(255, 255, 255));

	m_pScene->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 107);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


