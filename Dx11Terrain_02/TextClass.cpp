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
	// ȭ�� �ʺ�� ���̸� �����մϴ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �⺻ �� ��Ʈ������ �����մϴ�.
	m_baseViewMatrix = baseViewMatrix;

	// ��Ʈ ��ü�� �����մϴ�.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// ��Ʈ ��ü�� �ʱ�ȭ �մϴ�.
	bool result = m_Font->Initialize(device, "../Dx11Terrain_02/data/fontdata.txt", L"../Dx11Terrain_02/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// ù ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence1, 150, device);
	if(!result)
	{
		return false;
	}

	// �� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence2, 32, device);
	if(!result)
	{
		return false;
	}

	// �� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence3, 16, device);
	if(!result)
	{
		return false;
	}

	// �� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence4, 16, device);
	if(!result)
	{
		return false;
	}

	// �ټ� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence5, 16, device);
	if(!result)
	{
		return false;
	}

	// ���� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence6, 16, device);
	if(!result)
	{
		return false;
	}

	// �ϰ� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence7, 16, device);
	if(!result)
	{
		return false;
	}

	// ���� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence8, 16, device);
	if(!result)
	{
		return false;
	}

	// ��ȩ ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence9, 16, device);
	if(!result)
	{
		return false;
	}

	// �� ��° ������ �ʱ�ȭ�մϴ�
	result = InitializeSentence(&m_sentence10, 16, device);
	if(!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// ��Ʈ ��ü�� �����մϴ�.
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// ������� �����մϴ�.
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
	// ������ �׸��ϴ�.
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
	// ���ο� ���� ��ü�� ����ϴ�.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// ���� ���۸� null�� �ʱ�ȭ�մϴ�.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// ������ �ִ� ���̸� �����մϴ�.
	(*sentence)->maxLength = maxLength;

	// ���� �迭�� ���� ���� �����մϴ�.
	(*sentence)->vertexCount = 6 * maxLength;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// �ε��� �迭�� �ʱ�ȭ�մϴ�.
	for (int i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� �� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	// �� �̻� �ʿ����� ���� ���� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	// �� �̻� �ʿ����� ���� �ε��� �迭�� �����մϴ�.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// ������ ���� �����Ѵ�.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// ������ ���� ���� �÷ο츦 Ȯ���մϴ�.
	if ((int)strlen(text) > sentence->maxLength)
	{
		return false;
	}

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// �׸��⸦ ������ ȭ�鿡�� X �� Y �ȼ� ��ġ�� ����մϴ�.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// ��Ʈ Ŭ������ ����Ͽ� ���� �ؽ�Ʈ�� ���� �׸��� ��ġ���� ���� �迭�� ����ϴ�.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// ���ؽ� ���۸� �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// ���� ������ ����� �����մϴ�.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// �� �̻� �ʿ����� ���� ���� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// ���� ���ؽ� ���۸� �����մϴ�.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// ���� �ε��� ���۸� �����մϴ�.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// ������ �����մϴ�.
		delete *sentence;
		*sentence = 0;
	}
}


bool TextClass::RenderSentence(SentenceType* sentence, ID3D11DeviceContext* deviceContext, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �Է� �� ���� �������� �ȼ� ���� ���͸� ����ϴ�.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// ��Ʈ ���̴��� ����Ͽ� �ؽ�Ʈ�� �������մϴ�.
	return FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
}


bool TextClass::SetVideoCardInfo(const char* videoCardName, int videoCardMemory, ID3D11DeviceContext* deviceContext)
{
	char dataString[150] = { 0, };
	char tempString[16] = { 0, };
	char memoryString[32] = { 0, };

	// ���� ī�� �̸� ���ڿ��� �����մϴ�.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	if(!UpdateSentence(m_sentence1, dataString, 10, 10, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	// ���� �����÷ο츦 ���� ���� �޸� ���� �ڸ��ϴ�.
	if(videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// ���� �޸� ���� ���� ���ڿ� �������� ��ȯ�մϴ�.
	_itoa_s(videoCardMemory, tempString, 10);

	// ���� �޸� ���ڿ��� �����մϴ�.
	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
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

	// ���� �����÷ο츦 �����ϱ� ���� fps�� �ڸ��ϴ�.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// fps ������ ���ڿ� �������� ��ȯ�մϴ�.
	_itoa_s(fps, tempString, 10);

	// fps ���ڿ��� �����մϴ�.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
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

	// cpu ������ ���ڿ� �������� ��ȯ�մϴ�.
	_itoa_s(cpu, tempString, 10);

	// cpu ���ڿ��� �����մϴ�.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
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

	// �ε� �Ҽ������� ������ ��ġ�� ��ȯ�մϴ�.
	int positionX = (int)pos.x;
	int positionY = (int)pos.y;
	int positionZ = (int)pos.z;

	// 9999 �Ǵ� -9999�� �ʰ��ϸ� ��ġ�� �ڸ��ϴ�.
	if(positionX > 9999) { positionX = 9999; }
	if(positionY > 9999) { positionY = 9999; }
	if(positionZ > 9999) { positionZ = 9999; }

	if(positionX < -9999) { positionX = -9999; }
	if(positionY < -9999) { positionY = -9999; }
	if(positionZ < -9999) { positionZ = -9999; }

	// X ��ġ ���ڿ��� �����մϴ�.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence5, dataString, 10, 130, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}
	
	// Y ��ġ ���ڿ��� �����մϴ�.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence6, dataString, 10, 150, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Z ��ġ ���ڿ��� �����մϴ�.
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

	// ȸ������ ������ ��ȯ�մϴ�.
	int rotationX = (int)rot.x;
	int rotationY = (int)rot.y;
	int rotationZ = (int)rot.z;

	// X ȸ�� ���ڿ��� �����մϴ�.
	_itoa_s(rotationX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence8, dataString, 10, 210, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Y ȸ�� ���ڿ��� �����մϴ�.
	_itoa_s(rotationY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence9, dataString, 10, 230, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Z ȸ�� ���ڿ��� �����մϴ�.
	_itoa_s(rotationZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence10, dataString, 10, 250, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}