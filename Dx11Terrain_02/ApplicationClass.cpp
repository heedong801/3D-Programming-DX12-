#include "stdafx.h"
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "colorshaderclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	// 입력 개체를 생성합니다.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// 입력 개체를 초기화합니다.
	bool result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Direct3D 개체를 생성합니다.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D 개체를 초기화합니다.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// 카메라 객체를 생성합니다.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// 2D 사용자 인터페이스 렌더링을 위해 카메라로 기본 뷰 행렬을 초기화합니다.
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// 카메라의 초기 위치를 설정합니다.
	XMFLOAT3 camera = XMFLOAT3(50.0f, 2.0f, -7.0f);
	m_Camera->SetPosition(camera);

	// 지형 객체를 생성합니다.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}

	// 지형 객체를 초기화 합니다.
	result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Dx11Terrain_02/data/heightmap01.bmp");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// 색상 쉐이더 객체를 생성합니다.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	// 색상 쉐이더 객체를 초기화합니다.
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// 타이머 객체를 생성합니다.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// 타이머 객체를 초기화 합니다.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// 위치 개체를 생성합니다.
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	// 뷰어의 초기 위치를 초기 카메라 위치와 동일하게 설정합니다.
	m_Position->SetPosition(camera);

	// fps 객체를 생성합니다.
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	// fps 객체를 초기화합니다.
	m_Fps->Initialize();

	// cpu 객체를 생성합니다.
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	// cpu 객체를 초기화 합니다.
	m_Cpu->Initialize();

	// 폰트 셰이더 객체를 생성합니다.
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}

	// 폰트 셰이더 객체를 초기화합니다.
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// 텍스트 객체를 생성합니다.
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// 텍스트 객체를 초기화합니다.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// 비디오 카드 정보를 가져옵니다.
	char videoCard[128] = { 0, };
	int videoMemory = 0;
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// 텍스트 객체에 비디오 카드 정보를 설정합니다.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// 텍스트 객체를 해제합니다.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// 폰트 쉐이더 객체를 해제합니다..
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// cpu 객체를 해제합니다.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// fps 객체를 해제합니다.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// 위치 객체를 해제합니다.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// 타이머 객체를 해제합니다.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// 색상 셰이더 객체를 해제합니다.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// 지형 객체를 해제합니다.
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// 카메라 객체를 해제합니다.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D 객체를 해제합니다.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// 입력 객체를 해제합니다.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
}


bool ApplicationClass::Frame()
{
	// 사용자 입력을 읽습니다.
	bool result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// 사용자가 ESC를 눌렀을 때 응용 프로그램을 종료 할 것인지 확인합니다.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// 시스템 통계를 업데이트 합니다.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// 텍스트 개체에서 FPS 값을 업데이트 합니다.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	
	// 텍스트 개체의 CPU 사용값을 업데이트 합니다.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// 프레임 입력 처리를 수행합니다.
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	// 그래픽을 렌더링 합니다.
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	// 갱신된 위치를 계산하기 위한 프레임 시간을 설정합니다.
	m_Position->SetFrameTime(frameTime);

	// 입력을 처리합니다.
	m_Position->TurnLeft(m_Input->IsLeftPressed());
	m_Position->TurnRight(m_Input->IsRightPressed());
	m_Position->MoveForward(m_Input->IsUpPressed());
	m_Position->MoveBackward(m_Input->IsDownPressed());
	m_Position->MoveUpward(m_Input->IsAPressed());
	m_Position->MoveDownward(m_Input->IsZPressed());
	m_Position->LookUpward(m_Input->IsPgUpPressed());
	m_Position->LookDownward(m_Input->IsPgDownPressed());
	
	// 시점 위치 / 회전을 가져옵니다.
	m_Position->GetPosition(pos);
	m_Position->GetRotation(rot);

	// 카메라의 위치를 ​​설정합니다.
	m_Camera->SetPosition(pos);
	m_Camera->SetRotation(rot);

	// 텍스트 개체의 위치 값을 업데이트 합니다.
	if(!m_Text->SetCameraPosition(pos, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// 텍스트 객체의 회전 값을 업데이트 합니다.
	if(!m_Text->SetCameraRotation(rot, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	return true;
}


bool ApplicationClass::RenderGraphics()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	// 장면을 지웁니다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 및 ortho 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 지형 버퍼를 렌더링 합니다.
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// 색상 쉐이더를 사용하여 모델을 렌더링 합니다.
	if(!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();
		
	// 텍스트를 렌더링하기 전에 알파 블렌딩을 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 텍스트 사용자 인터페이스 요소를 렌더링 합니다.
	if(!m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 텍스트를 렌더링 한 후 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켭니다.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}