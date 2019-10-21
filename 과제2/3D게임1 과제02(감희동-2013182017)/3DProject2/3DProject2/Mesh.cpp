#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release(); 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	//따라하기 10 추가
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release(); 
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{ 
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	//따라하기 10 추가
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release(); 
	m_pd3dIndexUploadBuffer = NULL;
}

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology); 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0); 
		//(DrawIndexedInstanced)인덱스의 개수, 인스턴스의 개수, 시작인덱스의 위치(인덱스), 각 정점 인덱스에 더해지는 값, 인스턴스 인덱스에 더해지는 값
	}
	else 
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0); 
	}
}


CTriangleMesh::CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList) 
{
	//삼각형 메쉬를 정의한다. 
	m_nVertices = 3; 
	m_nStride = sizeof(CDiffusedVertex); 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[3]; 
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(Colors::Red));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Green)); 
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	//정점 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////따라하기9 인덱스 버퍼 없을 경우
//CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) 
//{
//	//직육면체는 6개의 면 가로(x-축) 길이 
//	m_nVertices = 36; 
//	m_nStride = sizeof(CDiffusedVertex); 
//	m_nOffset = 0;
//	m_nSlot = 0;
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//    
//	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이 
//	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
//
//	CDiffusedVertex pVertices[36]; 
//	int i = 0;
//
//	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다. 
//	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
//	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
//	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//	
//	//정점 버퍼 뷰를 생성한다.
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress(); 
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////
CCubeMeshColorful::CCubeMeshColorful(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 24;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	
	CDiffusedVertex pVertices[24]; 
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(Colors::Red));
	pVertices[8] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(Colors::Yellow));
	pVertices[16] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(Colors::Cyan));

	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(Colors::Yellow));
	pVertices[9] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(Colors::Green));
	pVertices[17] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(Colors::Cyan));

	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(Colors::Green));
	pVertices[10] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(Colors::Blue));
	pVertices[18] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(Colors::Cyan));

	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(Colors::Blue));
	pVertices[11] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(Colors::Red));
	pVertices[19] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(Colors::Cyan));

	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(Colors::Red));
	pVertices[12] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(Colors::Yellow));
	pVertices[20] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(Colors::Purple));

	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(Colors::Yellow));
	pVertices[13] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(Colors::Green));
	pVertices[21] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(Colors::Purple));

	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(Colors::Green));
	pVertices[14] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(Colors::Blue));
	pVertices[22] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(Colors::Purple));

	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(Colors::Blue));
	pVertices[15] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(Colors::Red));
	pVertices[23] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(Colors::Purple));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 
	삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다.
	즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//ⓐ 윗면(Front) 사각형의 아래쪽 삼각형  
	pnIndices[0] = 19;
	pnIndices[1] = 17;
	pnIndices[2] = 16;
	//ⓑ 윗면(Front) 사각형의 위쪽 삼각형 
	pnIndices[3] = 18;
	pnIndices[4] = 17;
	pnIndices[5] = 19;
	//ⓒ 앞면(Top) 사각형의 아래쪽 삼각형  
	pnIndices[6] = 8;
	pnIndices[7] = 5;
	pnIndices[8] = 12;
	//ⓓ 앞면(Top) 사각형의 위쪽 삼각형
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 8;
	//ⓔ 옆(LEFT) 사각형의 아래쪽 삼각형 
	pnIndices[12] = 11;
	pnIndices[13] = 4;
	pnIndices[14] = 15;
	//ⓕ 옆(LEFT) 사각형의 위쪽 삼각형 
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 11;
	//ⓖ 옆(RIGHT) 사각형의 아래쪽 삼각형 
	pnIndices[18] = 9;
	pnIndices[19] = 6;
	pnIndices[20] = 13;
	//ⓗ 옆(RIGHT) 사각형의 위쪽 삼각형
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 9;
	//ⓘ 뒷면 사각형의 아래쪽 삼각형
	pnIndices[24] = 10;
	pnIndices[25] = 7;
	pnIndices[26] = 14;
	//ⓙ  뒷면 사각형의 위쪽 삼각형
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 10;
	//ⓚ 아랫면 사각형의 아래쪽 삼각형  
	pnIndices[30] = 22;
	pnIndices[31] = 20;
	pnIndices[32] = 21;
	//ⓛ 아랫면 사각형의 위쪽 삼각형   
	pnIndices[33] = 23;
	pnIndices[34] = 20;
	pnIndices[35] = 22;
	
	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress(); 
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeMeshColorful::~CCubeMeshColorful()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList) 
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex); 
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3); 
	int i = 0;
	/*XMFLOAT4 Color = RANDOM_COLOR;*/
	//비행기 메쉬의 위쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 아래쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 오른쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress(); 
	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CAirplaneMeshDiffused::~CAirplaneMeshDiffused() 
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////

CWallMesh::CWallMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse, int CheckPos) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 12;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth/2, fy = fHeight/2, fz = fDepth/11/2;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 

	CDiffusedVertex pVertices[12];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), xmf4Diffuse);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), xmf4Diffuse);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), xmf4Diffuse);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), xmf4Diffuse);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), xmf4Diffuse);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), xmf4Diffuse);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), xmf4Diffuse);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), xmf4Diffuse);

	pVertices[8] = CDiffusedVertex(XMFLOAT3(-fx, +fy, 0), XMFLOAT4(0,0,0,1));
	pVertices[9] = CDiffusedVertex(XMFLOAT3(fx, +fy, 0), XMFLOAT4(0, 0, 0, 1));
	pVertices[10] = CDiffusedVertex(XMFLOAT3(fx, -fy,0), XMFLOAT4(0, 0, 0, 1));
	pVertices[11] = CDiffusedVertex(XMFLOAT3(-fx, -fy,0), XMFLOAT4(0, 0, 0, 1));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (CheckPos == BACKBOX)
	{
		m_nIndices = 6;

		UINT pnIndices[6];

		//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
		pnIndices[0] = 11;
		pnIndices[1] = 9;
		pnIndices[2] = 8;
		//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
		pnIndices[3] = 10;
		pnIndices[4] = 9;
		pnIndices[5] = 11;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == FRONTBOX)
	{
		m_nIndices = 6;

		UINT pnIndices[6];

		//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
		pnIndices[0] = 8;
		pnIndices[1] = 9;
		pnIndices[2] = 11;
		//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
		pnIndices[3] = 11;
		pnIndices[4] = 9;
		pnIndices[5] = 10;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == MIDBOX)
	{
		m_nIndices = 24;

		UINT pnIndices[24];
		//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
		pnIndices[0] = 0;
		pnIndices[1] = 1;
		pnIndices[2] = 3;
		//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형 
		pnIndices[3] = 3;
		pnIndices[4] = 1;
		pnIndices[5] = 2;
		//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형 
		pnIndices[6] = 7;
		pnIndices[7] = 4;
		pnIndices[8] = 3;
		//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
		pnIndices[9] = 3;
		pnIndices[10] = 4;
		pnIndices[11] = 0;
		//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
		pnIndices[12] = 5;
		pnIndices[13] = 6;
		pnIndices[14] = 1;
		//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
		pnIndices[15] = 1;
		pnIndices[16] = 6;
		pnIndices[17] = 2;
		//ⓚ 옆면(Right) 사각형의 위쪽 삼각형  
		pnIndices[18] = 5;
		pnIndices[19] = 4;
		pnIndices[20] = 6;
		//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형  
		pnIndices[21] = 6;
		pnIndices[22] = 4;
		pnIndices[23] = 7;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}

	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CWallMesh::CWallMesh::~CWallMesh()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), xmf4Diffuse);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), xmf4Diffuse);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), xmf4Diffuse);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), xmf4Diffuse);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), xmf4Diffuse);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), xmf4Diffuse);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), xmf4Diffuse);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), xmf4Diffuse);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다.
	삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다.
	즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////

CCubeMeshSimilar::CCubeMeshSimilar(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다.
	삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다.
	즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeMeshSimilar::~CCubeMeshSimilar()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////
CCubeMeshRandom::CCubeMeshRandom(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다.
	삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다.
	즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeMeshRandom::~CCubeMeshRandom()
{
}