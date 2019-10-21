#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release(); 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	//�����ϱ� 10 �߰�
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release(); 
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{ 
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	//�����ϱ� 10 �߰�
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release(); 
	m_pd3dIndexUploadBuffer = NULL;
}

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology); 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0); 
		//(DrawIndexedInstanced)�ε����� ����, �ν��Ͻ��� ����, �����ε����� ��ġ(�ε���), �� ���� �ε����� �������� ��, �ν��Ͻ� �ε����� �������� ��
	}
	else 
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0); 
	}
}


CTriangleMesh::CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList) 
{
	//�ﰢ�� �޽��� �����Ѵ�. 
	m_nVertices = 3; 
	m_nStride = sizeof(CDiffusedVertex); 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[3]; 
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(Colors::Red));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Green)); 
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	//���� ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////�����ϱ�9 �ε��� ���� ���� ���
//CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) 
//{
//	//������ü�� 6���� �� ����(x-��) ���� 
//	m_nVertices = 36; 
//	m_nStride = sizeof(CDiffusedVertex); 
//	m_nOffset = 0;
//	m_nSlot = 0;
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//    
//	//fWidth: ������ü ����(x-��) ����, fHeight: ������ü ����(y-��) ����, fDepth: ������ü ����(z-��) ���� 
//	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
//
//	CDiffusedVertex pVertices[36]; 
//	int i = 0;
//
//	//���� ���� �����ʹ� �ﰢ�� ����Ʈ�̹Ƿ� 36���� ���� �����͸� �غ��Ѵ�. 
//	//�� �ո�(Front) �簢���� ���� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
//	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	//�� ����(Top) �簢���� ���� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	//�� ����(Top) �簢���� �Ʒ��� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	//�� �޸�(Back) �簢���� ���� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
//	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	//�� ����(Left) �簢���� ���� �ﰢ��
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
//	//�� ����(Left) �簢���� �Ʒ��� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
//	//�� ����(Right) �簢���� ���� �ﰢ�� 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
//	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
//	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//	
//	//���� ���� �並 �����Ѵ�.
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress(); 
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////
CCubeMeshColorful::CCubeMeshColorful(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 24;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
	
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
	
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. 
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//�� ����(Front) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[0] = 19;
	pnIndices[1] = 17;
	pnIndices[2] = 16;
	//�� ����(Front) �簢���� ���� �ﰢ�� 
	pnIndices[3] = 18;
	pnIndices[4] = 17;
	pnIndices[5] = 19;
	//�� �ո�(Top) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[6] = 8;
	pnIndices[7] = 5;
	pnIndices[8] = 12;
	//�� �ո�(Top) �簢���� ���� �ﰢ��
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 8;
	//�� ��(LEFT) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[12] = 11;
	pnIndices[13] = 4;
	pnIndices[14] = 15;
	//�� ��(LEFT) �簢���� ���� �ﰢ�� 
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 11;
	//�� ��(RIGHT) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[18] = 9;
	pnIndices[19] = 6;
	pnIndices[20] = 13;
	//�� ��(RIGHT) �簢���� ���� �ﰢ��
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 9;
	//�� �޸� �簢���� �Ʒ��� �ﰢ��
	pnIndices[24] = 10;
	pnIndices[25] = 7;
	pnIndices[26] = 14;
	//��  �޸� �簢���� ���� �ﰢ��
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 10;
	//�� �Ʒ��� �簢���� �Ʒ��� �ﰢ��  
	pnIndices[30] = 22;
	pnIndices[31] = 20;
	pnIndices[32] = 21;
	//�� �Ʒ��� �簢���� ���� �ﰢ��   
	pnIndices[33] = 23;
	pnIndices[34] = 20;
	pnIndices[35] = 22;
	
	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
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
	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3); 
	int i = 0;
	/*XMFLOAT4 Color = RANDOM_COLOR;*/
	//����� �޽��� ���� ��
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
	//����� �޽��� �Ʒ��� �� 
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
	//����� �޽��� ������ �� 
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
	//����� �޽��� ����/������ �� 
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
	//����� �޽��� ���� �� 
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
	//����� �޽��� ����/���� �� 
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
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 12;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth/2, fy = fHeight/2, fz = fDepth/11/2;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 

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

		//�� ����(Left) �簢���� ���� �ﰢ��
		pnIndices[0] = 11;
		pnIndices[1] = 9;
		pnIndices[2] = 8;
		//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
		pnIndices[3] = 10;
		pnIndices[4] = 9;
		pnIndices[5] = 11;

		//�ε��� ���۸� �����Ѵ�.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == FRONTBOX)
	{
		m_nIndices = 6;

		UINT pnIndices[6];

		//�� ����(Left) �簢���� ���� �ﰢ��
		pnIndices[0] = 8;
		pnIndices[1] = 9;
		pnIndices[2] = 11;
		//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
		pnIndices[3] = 11;
		pnIndices[4] = 9;
		pnIndices[5] = 10;

		//�ε��� ���۸� �����Ѵ�.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == MIDBOX)
	{
		m_nIndices = 24;

		UINT pnIndices[24];
		//�� �ո�(Front) �簢���� ���� �ﰢ��  
		pnIndices[0] = 0;
		pnIndices[1] = 1;
		pnIndices[2] = 3;
		//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ�� 
		pnIndices[3] = 3;
		pnIndices[4] = 1;
		pnIndices[5] = 2;
		//�� �޸�(Back) �簢���� ���� �ﰢ�� 
		pnIndices[6] = 7;
		pnIndices[7] = 4;
		pnIndices[8] = 3;
		//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
		pnIndices[9] = 3;
		pnIndices[10] = 4;
		pnIndices[11] = 0;
		//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
		pnIndices[12] = 5;
		pnIndices[13] = 6;
		pnIndices[14] = 1;
		//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
		pnIndices[15] = 1;
		pnIndices[16] = 6;
		pnIndices[17] = 2;
		//�� ����(Right) �簢���� ���� �ﰢ��  
		pnIndices[18] = 5;
		pnIndices[19] = 4;
		pnIndices[20] = 6;
		//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
		pnIndices[21] = 6;
		pnIndices[22] = 4;
		pnIndices[23] = 7;

		//�ε��� ���۸� �����Ѵ�.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}

	//�ε��� ���� �並 �����Ѵ�.
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
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 

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

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�.
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ�� 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
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
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 

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

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�.
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ�� 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
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
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex); m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 

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

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�.
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��  
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ�� 
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��  
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeMeshRandom::~CCubeMeshRandom()
{
}