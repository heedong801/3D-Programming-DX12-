#include "stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	// 화면 너비와 높이를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 기본 뷰 매트릭스를 저장합니다.
	m_baseViewMatrix = baseViewMatrix;

	// 폰트 객체를 생성합니다.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// 폰트 객체를 초기화 합니다.
	bool result = m_Font->Initialize(device, "../Dx11Terrain_02/data/fontdata.txt", L"../Dx11Terrain_02/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// 첫 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence1, 150, device);
	if(!result)
	{
		return false;
	}

	// 두 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence2, 32, device);
	if(!result)
	{
		return false;
	}

	// 세 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence3, 16, device);
	if(!result)
	{
		return false;
	}

	// 네 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence4, 16, device);
	if(!result)
	{
		return false;
	}

	// 다섯 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence5, 16, device);
	if(!result)
	{
		return false;
	}

	// 여섯 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence6, 16, device);
	if(!result)
	{
		return false;
	}

	// 일곱 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence7, 16, device);
	if(!result)
	{
		return false;
	}

	// 여덟 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence8, 16, device);
	if(!result)
	{
		return false;
	}

	// 아홉 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence9, 16, device);
	if(!result)
	{
		return false;
	}

	// 열 번째 문장을 초기화합니다
	result = InitializeSentence(&m_sentence10, 16, device);
	if(!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// 폰트 객체를 해제합니다.
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// 문장들을 해제합니다.
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);
	ReleaseSentence(&m_sentence9);
	ReleaseSentence(&m_sentence10);
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 문장을 그립니다.
	bool result = RenderSentence(m_sentence1, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence2, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence3, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence4, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence5, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence6, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence7, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence8, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence9, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence10, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// 새로운 문장 개체를 만듭니다.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// 문장 버퍼를 null로 초기화합니다.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// 문장의 최대 길이를 설정합니다.
	(*sentence)->maxLength = maxLength;

	// 정점 배열의 정점 수를 설정합니다.
	(*sentence)->vertexCount = 6 * maxLength;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// 인덱스 배열을 초기화합니다.
	for (int i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 인 정점 버퍼의 설명을 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 만든다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	// 더 이상 필요하지 않은 정점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	// 더 이상 필요하지 않은 인덱스 배열을 해제합니다.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// 문장의 색을 저장한다.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// 가능한 버퍼 오버 플로우를 확인합니다.
	if ((int)strlen(text) > sentence->maxLength)
	{
		return false;
	}

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// 그리기를 시작할 화면에서 X 및 Y 픽셀 위치를 계산합니다.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// 폰트 클래스를 사용하여 문장 텍스트와 문장 그리기 위치에서 정점 배열을 만듭니다.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// 더 이상 필요하지 않은 정점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// 문장 버텍스 버퍼를 해제합니다.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// 문장 인덱스 버퍼를 해제합니다.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// 문장을 해제합니다.
		delete *sentence;
		*sentence = 0;
	}
}


bool TextClass::RenderSentence(SentenceType* sentence, ID3D11DeviceContext* deviceContext, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 정점 버퍼 간격 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 입력 된 문장 색상으로 픽셀 색상 벡터를 만듭니다.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// 폰트 셰이더를 사용하여 텍스트를 렌더링합니다.
	return FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
}


bool TextClass::SetVideoCardInfo(const char* videoCardName, int videoCardMemory, ID3D11DeviceContext* deviceContext)
{
	char dataString[150] = { 0, };
	char tempString[16] = { 0, };
	char memoryString[32] = { 0, };

	// 비디오 카드 이름 문자열을 설정합니다.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if(!UpdateSentence(m_sentence1, dataString, 10, 10, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	// 버퍼 오버플로우를 막기 위해 메모리 값을 자릅니다.
	if(videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// 비디오 메모리 정수 값을 문자열 형식으로 변환합니다.
	_itoa_s(videoCardMemory, tempString, 10);

	// 비디오 메모리 문자열을 설정합니다.
	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if(!UpdateSentence(m_sentence2, memoryString, 10, 30, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char fpsString[16] = { 0, };

	// 버퍼 오버플로우를 방지하기 위해 fps를 자릅니다.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// fps 정수를 문자열 형식으로 변환합니다.
	_itoa_s(fps, tempString, 10);

	// fps 문자열을 설정합니다.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if(!UpdateSentence(m_sentence3, fpsString, 10, 70, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char cpuString[16] = { 0, };

	// cpu 정수를 문자열 형식으로 변환합니다.
	_itoa_s(cpu, tempString, 10);

	// cpu 문자열을 설정합니다.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if(!UpdateSentence(m_sentence4, cpuString, 10, 90, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraPosition(XMFLOAT3 pos, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char dataString[16] = { 0, };

	// 부동 소수점에서 정수로 위치를 변환합니다.
	int positionX = (int)pos.x;
	int positionY = (int)pos.y;
	int positionZ = (int)pos.z;

	// 9999 또는 -9999를 초과하면 위치를 자릅니다.
	if(positionX > 9999) { positionX = 9999; }
	if(positionY > 9999) { positionY = 9999; }
	if(positionZ > 9999) { positionZ = 9999; }

	if(positionX < -9999) { positionX = -9999; }
	if(positionY < -9999) { positionY = -9999; }
	if(positionZ < -9999) { positionZ = -9999; }

	// X 위치 문자열을 설정합니다.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence5, dataString, 10, 130, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}
	
	// Y 위치 문자열을 설정합니다.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence6, dataString, 10, 150, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Z 위치 문자열을 설정합니다.
	_itoa_s(positionZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence7, dataString, 10, 170, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraRotation(XMFLOAT3 rot, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char dataString[16] = { 0, };

	// 회전값을 정수로 변환합니다.
	int rotationX = (int)rot.x;
	int rotationY = (int)rot.y;
	int rotationZ = (int)rot.z;

	// X 회전 문자열을 설정합니다.
	_itoa_s(rotationX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence8, dataString, 10, 210, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Y 회전 문자열을 설정합니다.
	_itoa_s(rotationY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence9, dataString, 10, 230, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Z 회전 문자열을 설정합니다.
	_itoa_s(rotationZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence10, dataString, 10, 250, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}