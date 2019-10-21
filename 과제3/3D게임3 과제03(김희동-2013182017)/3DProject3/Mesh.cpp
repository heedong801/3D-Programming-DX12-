//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_nReferences = 0;

	m_pd3dVertexBuffer = NULL;
	m_pd3dVertexUploadBuffer = NULL;

	m_pd3dIndexBuffer = NULL;
	m_pd3dIndexUploadBuffer = NULL;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nVertices = 0;
	m_nStride = 0;
	m_nOffset = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::AddRef() 
{ 
	m_nReferences++; 
}

void CMesh::Release() 
{ 
	m_nReferences--; 
	if (m_nReferences <= 0) delete this;
}

void CMesh::ReleaseUploadBuffers() 
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMeshDiffused(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CDiffusedVertex pVertices[8];

	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	UINT pnIndices[36];

	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMeshDiffused(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CDiffusedVertex pVertices[24 * 3];

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

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

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshIlluminated::CMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i*3+0;
		nIndex1 = i*3+1;
		nIndex2 = i*3+2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i*3+0];
			nIndex1 = pnIndices[i*3+1];
			nIndex2 = pnIndices[i*3+2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			if (pnIndices) 
				CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
			else
				CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
			break;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
			break;
		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	//색상은 재질과 다름. 색상을 정의해주고, 재질을 바꿔주면 일단 우리 눈에는 다른 색깔로 보임 하지만, 근본적으로 색상은 존재함.
	m_nVertices = 24;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	//인덱스 버퍼를 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	/////////////////////////////////////////////////노멀값을 정의해준다.///////////////////////////////////////////////////////
	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[6];
	/*for (int i = 0; i < 8; i++) 
		pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, 8, pnIndices, m_nIndices);*/

	pxmf3Normals[0] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pxmf3Normals[2] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pxmf3Normals[4] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pxmf3Normals[5] = XMFLOAT3(0.0f, -1.0f, 0.0f);

	CIlluminatedVertex pVertices[24];

	pVertices[0] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[0], XMFLOAT4(Colors::Red));
	pVertices[8] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[1], XMFLOAT4(Colors::Yellow));
	pVertices[16] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[4], XMFLOAT4(Colors::Cyan));

	pVertices[1] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[1], XMFLOAT4(Colors::Yellow));
	pVertices[9] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[2], XMFLOAT4(Colors::Green));
	pVertices[17] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[4], XMFLOAT4(Colors::Cyan));

	pVertices[2] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[2], XMFLOAT4(Colors::Green));
	pVertices[10] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[3], XMFLOAT4(Colors::Blue));
	pVertices[18] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[4], XMFLOAT4(Colors::Cyan));

	pVertices[3] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[3], XMFLOAT4(Colors::Blue));
	pVertices[11] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[0], XMFLOAT4(Colors::Red));
	pVertices[19] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[4], XMFLOAT4(Colors::Cyan));

	pVertices[4] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[0], XMFLOAT4(Colors::Red));
	pVertices[12] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[1], XMFLOAT4(Colors::Yellow));
	pVertices[20] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[5], XMFLOAT4(Colors::Purple));

	pVertices[5] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[1], XMFLOAT4(Colors::Yellow));
	pVertices[13] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[2], XMFLOAT4(Colors::Green));
	pVertices[21] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[5], XMFLOAT4(Colors::Purple));

	pVertices[6] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[2], XMFLOAT4(Colors::Green));
	pVertices[14] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[3], XMFLOAT4(Colors::Blue));
	pVertices[22] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[5], XMFLOAT4(Colors::Purple));

	pVertices[7] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[3], XMFLOAT4(Colors::Blue));
	pVertices[15] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[0], XMFLOAT4(Colors::Red));
	pVertices[23] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[5], XMFLOAT4(Colors::Purple));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////

CWallMeshIlluminated::CWallMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth , float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse, int CheckPos): CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	//색상은 재질과 다름. 색상을 정의해주고, 재질을 바꿔주면 일단 우리 눈에는 다른 색깔로 보임 하지만, 근본적으로 색상은 존재함.
	m_nVertices = 32;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (CheckPos == BACKBOX)
	{
		m_nIndices = 6;

		UINT pnIndices[6];

		pnIndices[0] = 31;
		pnIndices[1] = 29;
		pnIndices[2] = 28; 

		pnIndices[3] = 30;
		pnIndices[4] = 29;
		pnIndices[5] = 31;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == FRONTBOX)
	{
		m_nIndices = 6;

		UINT pnIndices[6];

		//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
		pnIndices[0] = 24;
		pnIndices[1] = 25;
		pnIndices[2] = 27;
		//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형  
		pnIndices[3] = 27;
		pnIndices[4] = 25;
		pnIndices[5] = 26;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}
	else if (CheckPos == MIDBOX)
	{
		m_nIndices = 24;

		UINT pnIndices[24];
		//LEFT UP
		pnIndices[0] = 11;
		pnIndices[1] = 4;
		pnIndices[2] = 0;
		//LEFT DOWN
		pnIndices[3] = 15;
		pnIndices[4] = 4;
		pnIndices[5] = 11;
		//RIGHT UP
		pnIndices[6] = 13;
		pnIndices[7] = 6;
		pnIndices[8] = 9;
		//RIGHT DOWN
		pnIndices[9] = 9;
		pnIndices[10] = 6;
		pnIndices[11] = 2;
		//UP UP
		pnIndices[12] = 16;
		pnIndices[13] = 17;
		pnIndices[14] = 19;
		//UP DOWN
		pnIndices[15] = 19;
		pnIndices[16] = 17;
		pnIndices[17] = 18;
		//ⓚDOWN UP  
		pnIndices[18] = 22;
		pnIndices[19] = 20;
		pnIndices[20] = 23;
		//DOWN DOWN
		pnIndices[21] = 21;
		pnIndices[22] = 20;
		pnIndices[23] = 22;

		//인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	}

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	/////////////////////////////////////////////////노멀값을 정의해준다.///////////////////////////////////////////////////////
	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth / 11/ 2;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[6];
	/*for (int i = 0; i < 8; i++)
	pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, 8, pnIndices, m_nIndices);*/

	pxmf3Normals[0] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pxmf3Normals[2] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pxmf3Normals[4] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	pxmf3Normals[5] = XMFLOAT3(0.0f, 1.0f, 0.0f);

	CIlluminatedVertex pVertices[32];

	pVertices[0] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[8] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[16] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[1] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[9] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[17] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[2] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[10] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[18] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[3] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[11] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[19] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[4] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[12] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[20] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[5] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[13] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[21] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[6] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[14] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[22] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[7] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[15] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[23] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[24] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, 0), pxmf3Normals[1], XMFLOAT4(Colors::Black));
	pVertices[25] = CIlluminatedVertex(XMFLOAT3(fx, fy, 0), pxmf3Normals[1], XMFLOAT4(Colors::Black));
	pVertices[26] = CIlluminatedVertex(XMFLOAT3(fx, -fy, 0), pxmf3Normals[1], XMFLOAT4(Colors::Black));
	pVertices[27] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, 0), pxmf3Normals[1], XMFLOAT4(Colors::Black));

	pVertices[28] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, 0), pxmf3Normals[3], XMFLOAT4(Colors::Black));
	pVertices[29] = CIlluminatedVertex(XMFLOAT3(fx, fy, 0), pxmf3Normals[3], XMFLOAT4(Colors::Black));
	pVertices[30] = CIlluminatedVertex(XMFLOAT3(fx, -fy, 0), pxmf3Normals[3], XMFLOAT4(Colors::Black));
	pVertices[31] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, 0), pxmf3Normals[3], XMFLOAT4(Colors::Black));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CWallMeshIlluminated::~CWallMeshIlluminated()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminatedOneColor::CCubeMeshIlluminatedOneColor(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	//색상은 재질과 다름. 색상을 정의해주고, 재질을 바꿔주면 일단 우리 눈에는 다른 색깔로 보임 하지만, 근본적으로 색상은 존재함.
	m_nVertices = 24;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	//인덱스 버퍼를 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	/////////////////////////////////////////////////노멀값을 정의해준다.///////////////////////////////////////////////////////
	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[6];
	/*for (int i = 0; i < 8; i++)
	pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, 8, pnIndices, m_nIndices);*/

	pxmf3Normals[0] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pxmf3Normals[2] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pxmf3Normals[4] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pxmf3Normals[5] = XMFLOAT3(0.0f, -1.0f, 0.0f);

	CIlluminatedVertex pVertices[24];

	pVertices[0] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[8] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[16] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[1] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[9] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[17] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[2] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[10] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[18] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[3] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[11] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[19] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[4], xmf4Diffuse);

	pVertices[4] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[12] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[20] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[5] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[1], xmf4Diffuse);
	pVertices[13] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[21] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[6] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[2], xmf4Diffuse);
	pVertices[14] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[22] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[5], xmf4Diffuse);

	pVertices[7] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[3], xmf4Diffuse);
	pVertices[15] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[0], xmf4Diffuse);
	pVertices[23] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[5], xmf4Diffuse);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminatedOneColor::~CCubeMeshIlluminatedOneColor()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminatedSimilar::CCubeMeshIlluminatedSimilar(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Diffuse) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	//색상은 재질과 다름. 색상을 정의해주고, 재질을 바꿔주면 일단 우리 눈에는 다른 색깔로 보임 하지만, 근본적으로 색상은 존재함.
	m_nVertices = 24;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	//인덱스 버퍼를 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	/////////////////////////////////////////////////노멀값을 정의해준다.///////////////////////////////////////////////////////
	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[6];
	/*for (int i = 0; i < 8; i++)
	pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, 8, pnIndices, m_nIndices);*/

	pxmf3Normals[0] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pxmf3Normals[2] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pxmf3Normals[4] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pxmf3Normals[5] = XMFLOAT3(0.0f, -1.0f, 0.0f);

	CIlluminatedVertex pVertices[24];

	pVertices[0] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[0], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[8] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[1], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[16] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, -fz), pxmf3Normals[4], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[1] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[1], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[9] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[2], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[17] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, -fz), pxmf3Normals[4], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[2] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[2], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[10] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[3], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[18] = CIlluminatedVertex(XMFLOAT3(+fx, +fy, +fz), pxmf3Normals[4], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[3] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[3], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[11] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[0], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[19] = CIlluminatedVertex(XMFLOAT3(-fx, +fy, +fz), pxmf3Normals[4], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[4] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[0], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[12] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[1], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[20] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, -fz), pxmf3Normals[5], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[5] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[1], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[13] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[2], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[21] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, -fz), pxmf3Normals[5], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[6] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[2], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[14] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[3], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[22] = CIlluminatedVertex(XMFLOAT3(+fx, -fy, +fz), pxmf3Normals[5], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	pVertices[7] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[3], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[15] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[0], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));
	pVertices[23] = CIlluminatedVertex(XMFLOAT3(-fx, -fy, +fz), pxmf3Normals[5], Vector4::Add(xmf4Diffuse, RANDOM_COLOR));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminatedSimilar::~CCubeMeshIlluminatedSimilar()
{
}