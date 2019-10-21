//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;     
	m_hMenu = NULL;     

    m_hDCFrameBuffer = NULL;   
    m_hBitmapFrameBuffer = NULL;   
    m_hBitmapSelect = NULL;     

	m_cxViewport = FULLSCREEN_WIDTH;
	m_cyViewport = FULLSCREEN_HEIGHT;

	m_bActive = true;    
	m_nObjects = 0;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_xViewport = rc.left;
	m_yViewport = rc.top;
	m_cxViewport = rc.right - rc.left;
	m_cyViewport = rc.bottom - rc.top;

	m_pCamera = new CCamera();

	BuildFrameBuffer(); 

//    srand(timeGetTime());

	BuildObjects(); 

	SetupGameState();

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
    HDC hDC = ::GetDC(m_hWnd);

    if (!m_hDCFrameBuffer) m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
    if (m_hBitmapFrameBuffer) 
    {
        ::SelectObject(m_hDCFrameBuffer, NULL);
        ::DeleteObject(m_hBitmapFrameBuffer);
        m_hBitmapFrameBuffer = NULL;
    } 
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_cxViewport, m_cyViewport);    
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
    HBRUSH hBrush = ::CreateSolidBrush(RGB(255,255,255));
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_xViewport, m_yViewport, m_cxViewport, m_cyViewport);
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);	//흰색 네모를 배경만하게 그려서 없앰
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_xViewport, m_yViewport, m_cxViewport, m_cyViewport, m_hDCFrameBuffer, m_xViewport, m_yViewport, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);	//버퍼 -> 화면 프레임 씌우기
}

void CGameFramework::ResetDisplay(int cxViewport, int cyViewport)
{
	m_cxViewport = cxViewport;
	m_cyViewport = cyViewport;

	BuildFrameBuffer();
}

void CGameFramework::SetupGameState()
{
}

void CGameFramework::SetupRenderStates()
{
}

CMesh *CGameFramework::BuildCubeMesh()
{
	CMesh *pMesh = new CMesh(6);

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-2, +2, -2, RGB(255,0,0)));
	pFrontFace->SetVertex(1, CVertex(+2, +2, -2, RGB(255,0,0)));
	pFrontFace->SetVertex(2, CVertex(+2, -2, -2, RGB(255,0,0)));
	pFrontFace->SetVertex(3, CVertex(-2, -2, -2, RGB(255,0,0)));
	pMesh->SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-2, +2, +2, RGB(255,0,0)));
	pTopFace->SetVertex(1, CVertex(+2, +2, +2, RGB(255,0,0)));
	pTopFace->SetVertex(2, CVertex(+2, +2, -2, RGB(255,0,0)));
	pTopFace->SetVertex(3, CVertex(-2, +2, -2, RGB(255,0,0)));
	pMesh->SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-2, -2, +2, RGB(255,0,0)));
	pBackFace->SetVertex(1, CVertex(+2, -2, +2, RGB(255,0,0)));
	pBackFace->SetVertex(2, CVertex(+2, +2, +2, RGB(255,0,0)));
	pBackFace->SetVertex(3, CVertex(-2, +2, +2, RGB(255,0,0)));
	pMesh->SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-2, -2, -2, RGB(255,0,0)));
	pBottomFace->SetVertex(1, CVertex(+2, -2, -2, RGB(255,0,0)));
	pBottomFace->SetVertex(2, CVertex(+2, -2, +2, RGB(255,0,0)));
	pBottomFace->SetVertex(3, CVertex(-2, -2, +2, RGB(255,0,0)));
	pMesh->SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-2, +2, +2, RGB(255,0,0)));
	pLeftFace->SetVertex(1, CVertex(-2, +2, -2, RGB(255,0,0)));
	pLeftFace->SetVertex(2, CVertex(-2, -2, -2, RGB(255,0,0)));
	pLeftFace->SetVertex(3, CVertex(-2, -2, +2, RGB(255,0,0)));
	pMesh->SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+2, +2, -2, RGB(255,0,0)));
	pRightFace->SetVertex(1, CVertex(+2, +2, +2, RGB(255,0,0)));
	pRightFace->SetVertex(2, CVertex(+2, -2, +2, RGB(255,0,0)));
	pRightFace->SetVertex(3, CVertex(+2, -2, -2, RGB(255,0,0)));
	pMesh->SetPolygon(5, pRightFace);

    return(pMesh);
}

void CGameFramework::BuildObjects()
{
	CMesh *pMesh = BuildCubeMesh();

	m_nObjects = 2;
	m_pObjects = new CGameObject[m_nObjects];
    m_pObjects[0].SetMesh(pMesh);
    m_pObjects[1].SetMesh(pMesh);
    m_pObjects[0].SetPosition(-3.5f, +2.0f, +14.0f);
    m_pObjects[1].SetPosition(+3.5f, -2.0f, +14.0f);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pObjects) delete [] m_pObjects;
	m_pObjects = NULL;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hMenu) DestroyMenu(m_hMenu);
    m_hMenu = NULL;

    SetMenu(m_hWnd, NULL);
    if (m_hWnd) DestroyWindow(m_hWnd);
    m_hWnd = NULL;
}

void CGameFramework::ProcessInput()
{
    //if (GetKeyState(VK_LEFT) & 0xFF00) m_pCamera->m_fxPosition -= 25.0f * 0.0005f;
    //if (GetKeyState(VK_RIGHT) & 0xFF00) m_pCamera->m_fxPosition += 25.0f * 0.0005f;
    //if (GetKeyState(VK_UP) & 0xFF00) m_pCamera->m_fzPosition += 25.0f * 0.0005f;
    //if (GetKeyState(VK_DOWN) & 0xFF00) m_pCamera->m_fzPosition -= 25.0f * 0.0005f;

    if (GetKeyState(VK_LEFT) & 0xFF00) m_pCamera->m_fxRotation -= 25.0f * 0.005f;
    if (GetKeyState(VK_RIGHT) & 0xFF00) m_pCamera->m_fxRotation += 25.0f * 0.005f;
    if (GetKeyState(VK_UP) & 0xFF00) m_pCamera->m_fzRotation += 25.0f * 0.005f;
    if (GetKeyState(VK_DOWN) & 0xFF00) m_pCamera->m_fzRotation -= 25.0f * 0.005f;
}

void CGameFramework::AnimateObjects()
{
    float fYaw = 0.0f, fPitch = 0.0f, fRoll = 0.0f;

	for (int i = 0; i < m_nObjects; i++)
	{
        fYaw   = 0.02f * (i+1);
        fPitch = 0.02f * (i+1);
        fRoll  = 0.01f * (i+1);
        m_pObjects[i].Rotate(fPitch, fYaw, fRoll);	//게임오브젝트클래스에 회전변수에 더해줌
	}
}

void CGameFramework::DrawPrimitive(CPolygon *pPolygon, CGameObject *pObject)
{
	CVertex vertex;
    CPoint vPrevious, vCurrent, vRotated;
    for (int i = 0; i <= pPolygon->m_nVertices; i++) 
    {
		vertex = pPolygon->m_pVertices[i % pPolygon->m_nVertices];
        vRotated = vCurrent = vertex.m_vPosition;	
//Scale Transformation
		vCurrent.x *= pObject->m_fxScale;
		vCurrent.y *= pObject->m_fyScale;
		vCurrent.z *= pObject->m_fzScale;
//Rotation Transformation
		if (pObject->m_fxRotation != 0.0f) 
		{
			 vRotated.y = float(vCurrent.y * cos(DegreeToRadian(pObject->m_fxRotation)) - vCurrent.z * sin(DegreeToRadian(pObject->m_fxRotation)));
			 vRotated.z = float(vCurrent.y * sin(DegreeToRadian(pObject->m_fxRotation)) + vCurrent.z * cos(DegreeToRadian(pObject->m_fxRotation)));
			 vCurrent = vRotated;
		}
		if (pObject->m_fyRotation != 0.0f) 
		{
			 vRotated.x = float(vCurrent.x * cos(DegreeToRadian(pObject->m_fyRotation)) + vCurrent.z * sin(DegreeToRadian(pObject->m_fyRotation)));
			 vRotated.z = float(-vCurrent.x * sin(DegreeToRadian(pObject->m_fyRotation)) + vCurrent.z * cos(DegreeToRadian(pObject->m_fyRotation)));
			 vCurrent = vRotated;
		}
		if (pObject->m_fzRotation != 0.0f) 
		{
			 vRotated.x = float(vCurrent.x * cos(DegreeToRadian(pObject->m_fzRotation)) - vCurrent.y * sin(DegreeToRadian(pObject->m_fzRotation)));
			 vRotated.y = float(vCurrent.x * sin(DegreeToRadian(pObject->m_fzRotation)) + vCurrent.y * cos(DegreeToRadian(pObject->m_fzRotation)));
			 vCurrent = vRotated;
		}
//Translation Transformation
		vCurrent.x += pObject->m_fxPosition;
		vCurrent.y += pObject->m_fyPosition;
		vCurrent.z += pObject->m_fzPosition;
//View Transformation
		vCurrent.x -= m_pCamera->m_fxPosition;		//카메라 좌표계 -> 월드좌표계의 원점으로 바꾸는 것?
		vCurrent.y -= m_pCamera->m_fyPosition;
		vCurrent.z -= m_pCamera->m_fzPosition;
		if (m_pCamera->m_fxRotation != 0.0f) 
		{
			 vRotated.y = float(vCurrent.y * cos(DegreeToRadian(-m_pCamera->m_fxRotation)) - vCurrent.z * sin(DegreeToRadian(-m_pCamera->m_fxRotation)));
			 vRotated.z = float(vCurrent.y * sin(DegreeToRadian(-m_pCamera->m_fxRotation)) + vCurrent.z * cos(DegreeToRadian(-m_pCamera->m_fxRotation)));
			 vCurrent.y = vRotated.y;
			 vCurrent.z = vRotated.z;
		}
		if (m_pCamera->m_fyRotation != 0.0f) 
		{
			 vRotated.x = float(vCurrent.x * cos(DegreeToRadian(-m_pCamera->m_fyRotation)) + vCurrent.z * sin(DegreeToRadian(-m_pCamera->m_fyRotation)));
			 vRotated.z = float(-vCurrent.x * sin(DegreeToRadian(-m_pCamera->m_fyRotation)) + vCurrent.z * cos(DegreeToRadian(-m_pCamera->m_fyRotation)));
			 vCurrent.x = vRotated.x;
			 vCurrent.z = vRotated.z;
		}
		if (m_pCamera->m_fzRotation != 0.0f) 
		{
			 vRotated.x = float(vCurrent.x * cos(DegreeToRadian(-m_pCamera->m_fzRotation)) - vCurrent.y * sin(DegreeToRadian(-m_pCamera->m_fzRotation)));
			 vRotated.y = float(vCurrent.x * sin(DegreeToRadian(-m_pCamera->m_fzRotation)) + vCurrent.y * cos(DegreeToRadian(-m_pCamera->m_fzRotation)));
			 vCurrent.x = vRotated.x;
			 vCurrent.y = vRotated.y;
		}
//Perspective Transformation
	    float fAspect = (float)m_cxViewport / (float)m_cyViewport;	//종횡비 = 너비 / 높이
		if (vCurrent.z != 0.0f) vCurrent.x /= (fAspect * vCurrent.z);	//직사각형이 되는 걸 방지, 원근 나눗셈
		if (vCurrent.z != 0.0f) vCurrent.y /= vCurrent.z;	//원근 나눗셈
//Screen Transformation
        vCurrent.x = +vCurrent.x * (m_cxViewport / 2) + m_xViewport + (m_cxViewport / 2);
        vCurrent.y = -vCurrent.y * (m_cyViewport / 2) + m_yViewport + (m_cyViewport / 2);

        if ((i != 0) && (vCurrent.z > 0.0f)) 
		{
			HPEN hPen = ::CreatePen(PS_SOLID, 0, vertex.m_dwColor);
			HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
			::MoveToEx(m_hDCFrameBuffer, (long)vPrevious.x, (long)vPrevious.y, NULL);
			::LineTo(m_hDCFrameBuffer, (long)vCurrent.x, (long)vCurrent.y);
			::SelectObject(m_hDCFrameBuffer, hOldPen);
			::DeleteObject(hPen);
		}
        vPrevious = vCurrent; 
    }
}

void CGameFramework::DrawObject(CGameObject *pObject)
{ 
    CMesh *pMesh = pObject->m_pMesh;
    for (int j = 0; j < pMesh->m_nPolygons; j++)
    {
        DrawPrimitive(pMesh->m_ppPolygons[j], pObject);	//오브젝트 -> 화면
    } 
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;	//클래스가 전역변수로 선언 즉 False

	ProcessInput();	//키보드 처리

	AnimateObjects();	//자동으로 움직이는 애니메이션 

    ClearFrameBuffer(0x00FFFFFF);

	for (int i = 0; i < m_nObjects; i++) DrawObject(&m_pObjects[i]);

	PresentFrameBuffer();
}


