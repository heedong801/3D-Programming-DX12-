#include "stdafx.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(int nVertices)
{
	if (nVertices > 0)
	{
		m_nVertices = nVertices;
		m_pVertices = new CVertex[nVertices];
	}
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	if (nPolygons > 0)
	{
		m_nPolygons = nPolygons;
		m_ppPolygons = new CPolygon*[nPolygons];
		m_nReferences = 0;
	}
}

CMesh::~CMesh(void)
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons) && m_ppPolygons)
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFrameBuffer, XMFLOAT4X4& xmf4x4World, CCamera *pCamera)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Multiply(xmf4x4World, pCamera->m_xmf4x4ViewProject);
	for (int j = 0; j < m_nPolygons; j++)
	{
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex *pVertices = m_ppPolygons[j]->m_pVertices;
		XMFLOAT3 xmf3Previous(-1.0f, 0.0f, 0.0f);
		for (int i = 0; i <= nVertices; i++)
		{
			CVertex vertex = pVertices[i % nVertices];
			//World/View/Projection Transformation(Perspective Divide)
			XMFLOAT3 xmf3Current = Vector3::TransformCoord(vertex.m_xmf3Position, xmf4x4Transform);
			if ((xmf3Current.z >= 0.0f) && (xmf3Current.z <= 1.0f))
			{
				//Screen Transformation
				xmf3Current.x = +xmf3Current.x * (pCamera->m_Viewport.m_nWidth * 0.5f) + pCamera->m_Viewport.m_xStart + (pCamera->m_Viewport.m_nWidth * 0.5f);
				xmf3Current.y = -xmf3Current.y * (pCamera->m_Viewport.m_nHeight * 0.5f) + pCamera->m_Viewport.m_yStart + (pCamera->m_Viewport.m_nHeight * 0.5f);

				if (xmf3Previous.x >= 0.0f)
				{
					::MoveToEx(hDCFrameBuffer, (long)xmf3Previous.x, (long)xmf3Previous.y, NULL);
					::LineTo(hDCFrameBuffer, (long)xmf3Current.x, (long)xmf3Current.y);
				}
				xmf3Previous = xmf3Current;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);
}

CCubeMesh::~CCubeMesh(void)
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
BossMesh::BossMesh(float fWidth, float fHeight, float fDepth) : CMesh(30)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	float fQuaterWidth = fWidth * 0.25f;
	float fQuaterHeight = fHeight * 0.25f;
	float fQuaterDepth = fDepth * 0.25f;

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	CPolygon *pLeftBoxFrontFace = new CPolygon(4);
	pLeftBoxFrontFace->SetVertex(0, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	pLeftBoxFrontFace->SetVertex(1, CVertex(-fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pLeftBoxFrontFace->SetVertex(2, CVertex(-fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pLeftBoxFrontFace->SetVertex(3, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	SetPolygon(6, pLeftBoxFrontFace);

	CPolygon *pLeftBoxBackFace = new CPolygon(4);
	pLeftBoxBackFace->SetVertex(0, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxBackFace->SetVertex(1, CVertex(-fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxBackFace->SetVertex(2, CVertex(-fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	pLeftBoxBackFace->SetVertex(3, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(7, pLeftBoxBackFace);

	CPolygon *pLeftBoxLeftFace = new CPolygon(4);
	pLeftBoxLeftFace->SetVertex(0, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxLeftFace->SetVertex(1, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	pLeftBoxLeftFace->SetVertex(2, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	pLeftBoxLeftFace->SetVertex(3, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(8, pLeftBoxLeftFace);

	CPolygon *pLeftBoxRightFace = new CPolygon(4);
	pLeftBoxRightFace->SetVertex(0, CVertex(-fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxRightFace->SetVertex(1, CVertex(-fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pLeftBoxRightFace->SetVertex(2, CVertex(-fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pLeftBoxRightFace->SetVertex(3, CVertex(-fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(9, pLeftBoxRightFace);

	CPolygon *pLeftBoxTopFace = new CPolygon(4);
	pLeftBoxTopFace->SetVertex(0, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxTopFace->SetVertex(1, CVertex(-fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pLeftBoxTopFace->SetVertex(2, CVertex(-fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pLeftBoxTopFace->SetVertex(3, CVertex(-fHalfWidth - fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	SetPolygon(10, pLeftBoxTopFace);

	CPolygon *pLeftBoxDownFace = new CPolygon(4);
	pLeftBoxDownFace->SetVertex(0, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	pLeftBoxDownFace->SetVertex(1, CVertex(-fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	pLeftBoxDownFace->SetVertex(2, CVertex(-fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pLeftBoxDownFace->SetVertex(3, CVertex(-fHalfWidth - fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	SetPolygon(11, pLeftBoxDownFace);

	CPolygon *pRightBoxFrontFace = new CPolygon(4);
	pRightBoxFrontFace->SetVertex(0, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	pRightBoxFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pRightBoxFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pRightBoxFrontFace->SetVertex(3, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	SetPolygon(12, pRightBoxFrontFace);

	CPolygon *pRightBoxBackFace = new CPolygon(4);
	pRightBoxBackFace->SetVertex(0, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxBackFace->SetVertex(1, CVertex(+fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxBackFace->SetVertex(2, CVertex(+fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	pRightBoxBackFace->SetVertex(3, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(13, pRightBoxBackFace);

	CPolygon *pRightBoxLeftFace = new CPolygon(4);
	pRightBoxLeftFace->SetVertex(0, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxLeftFace->SetVertex(1, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	pRightBoxLeftFace->SetVertex(2, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	pRightBoxLeftFace->SetVertex(3, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(14, pRightBoxLeftFace);

	CPolygon *pRightBoxRightFace = new CPolygon(4);
	pRightBoxRightFace->SetVertex(0, CVertex(+fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxRightFace->SetVertex(1, CVertex(+fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pRightBoxRightFace->SetVertex(2, CVertex(+fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pRightBoxRightFace->SetVertex(3, CVertex(+fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	SetPolygon(15, pRightBoxRightFace);

	CPolygon *pRightBoxTopFace = new CPolygon(4);
	pRightBoxTopFace->SetVertex(0, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxTopFace->SetVertex(1, CVertex(+fHalfWidth, +fQuaterHeight, +fQuaterDepth));
	pRightBoxTopFace->SetVertex(2, CVertex(+fHalfWidth, +fQuaterHeight, -fQuaterDepth));
	pRightBoxTopFace->SetVertex(3, CVertex(+fHalfWidth + fQuaterWidth, +fQuaterHeight, -fQuaterDepth));
	SetPolygon(16, pRightBoxTopFace);

	CPolygon *pRightBoxDownFace = new CPolygon(4);
	pRightBoxDownFace->SetVertex(0, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, +fQuaterDepth));
	pRightBoxDownFace->SetVertex(1, CVertex(+fHalfWidth, -fQuaterHeight, +fQuaterDepth));
	pRightBoxDownFace->SetVertex(2, CVertex(+fHalfWidth, -fQuaterHeight, -fQuaterDepth));
	pRightBoxDownFace->SetVertex(3, CVertex(+fHalfWidth + fQuaterWidth, -fQuaterHeight, -fQuaterDepth));
	SetPolygon(17, pRightBoxDownFace);

	CPolygon *pTopBoxFrontFace = new CPolygon(4);
	pTopBoxFrontFace->SetVertex(0, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxFrontFace->SetVertex(1, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxFrontFace->SetVertex(2, CVertex(-fQuaterWidth, fHalfHeight, -fQuaterDepth));
	pTopBoxFrontFace->SetVertex(3, CVertex(+fQuaterWidth, fHalfHeight, -fQuaterDepth));
	SetPolygon(18, pTopBoxFrontFace);

	CPolygon *pTopBoxBackFace = new CPolygon(4);
	pTopBoxBackFace->SetVertex(0, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	pTopBoxBackFace->SetVertex(1, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	pTopBoxBackFace->SetVertex(2, CVertex(-fQuaterWidth, fHalfHeight, +fQuaterDepth));
	pTopBoxBackFace->SetVertex(3, CVertex(+fQuaterWidth, fHalfHeight, +fQuaterDepth));
	SetPolygon(19, pTopBoxBackFace);

	CPolygon *pTopBoxLeftFace = new CPolygon(4);
	pTopBoxLeftFace->SetVertex(0, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	pTopBoxLeftFace->SetVertex(1, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxLeftFace->SetVertex(2, CVertex(-fQuaterWidth, fHalfHeight, -fQuaterDepth));
	pTopBoxLeftFace->SetVertex(3, CVertex(-fQuaterWidth, fHalfHeight, +fQuaterDepth));
	SetPolygon(20, pTopBoxLeftFace);

	CPolygon *pTopBoxRightFace = new CPolygon(4);
	pTopBoxRightFace->SetVertex(0, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	pTopBoxRightFace->SetVertex(1, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxRightFace->SetVertex(2, CVertex(+fQuaterWidth, fHalfHeight, -fQuaterDepth));
	pTopBoxRightFace->SetVertex(3, CVertex(+fQuaterWidth, fHalfHeight, +fQuaterDepth));
	SetPolygon(21, pTopBoxRightFace);

	CPolygon *pTopBoxTopFace = new CPolygon(4);
	pTopBoxTopFace->SetVertex(0, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	pTopBoxTopFace->SetVertex(1, CVertex(+fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxTopFace->SetVertex(2, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, -fQuaterDepth));
	pTopBoxTopFace->SetVertex(3, CVertex(-fQuaterWidth, fHalfHeight + fQuaterHeight, +fQuaterDepth));
	SetPolygon(22, pTopBoxTopFace);

	CPolygon *pTopBoxDownFace = new CPolygon(4);
	pTopBoxDownFace->SetVertex(0, CVertex(+fQuaterWidth, fHalfHeight, +fQuaterDepth));
	pTopBoxDownFace->SetVertex(1, CVertex(+fQuaterWidth, fHalfHeight, -fQuaterDepth));
	pTopBoxDownFace->SetVertex(2, CVertex(-fQuaterWidth, fHalfHeight, -fQuaterDepth));
	pTopBoxDownFace->SetVertex(3, CVertex(-fQuaterWidth, fHalfHeight, +fQuaterDepth));
	SetPolygon(23, pTopBoxDownFace);

	CPolygon *pDownBoxFrontFace = new CPolygon(4);
	pDownBoxFrontFace->SetVertex(0, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxFrontFace->SetVertex(1, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxFrontFace->SetVertex(2, CVertex(-fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	pDownBoxFrontFace->SetVertex(3, CVertex(+fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	SetPolygon(24, pDownBoxFrontFace);

	CPolygon *pDownBoxBackFace = new CPolygon(4);
	pDownBoxBackFace->SetVertex(0, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	pDownBoxBackFace->SetVertex(1, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	pDownBoxBackFace->SetVertex(2, CVertex(-fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	pDownBoxBackFace->SetVertex(3, CVertex(+fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	SetPolygon(25, pDownBoxBackFace);

	CPolygon *pDownBoxLeftFace = new CPolygon(4);
	pDownBoxLeftFace->SetVertex(0, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	pDownBoxLeftFace->SetVertex(1, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxLeftFace->SetVertex(2, CVertex(-fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	pDownBoxLeftFace->SetVertex(3, CVertex(-fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	SetPolygon(26, pDownBoxLeftFace);					  

	CPolygon *pDownBoxRightFace = new CPolygon(4);
	pDownBoxRightFace->SetVertex(0, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	pDownBoxRightFace->SetVertex(1, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxRightFace->SetVertex(2, CVertex(+fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	pDownBoxRightFace->SetVertex(3, CVertex(+fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	SetPolygon(27, pDownBoxRightFace);

	CPolygon *pDownBoxTopFace = new CPolygon(4);
	pDownBoxTopFace->SetVertex(0, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	pDownBoxTopFace->SetVertex(1, CVertex(+fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxTopFace->SetVertex(2, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, -fQuaterDepth));
	pDownBoxTopFace->SetVertex(3, CVertex(-fQuaterWidth, -fHalfHeight - fQuaterHeight, +fQuaterDepth));
	SetPolygon(28, pDownBoxTopFace);

	CPolygon *pDownBoxDownFace = new CPolygon(4);
	pDownBoxDownFace->SetVertex(0, CVertex(+fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	pDownBoxDownFace->SetVertex(1, CVertex(+fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	pDownBoxDownFace->SetVertex(2, CVertex(-fQuaterWidth, -fHalfHeight, -fQuaterDepth));
	pDownBoxDownFace->SetVertex(3, CVertex(-fQuaterWidth, -fHalfHeight, +fQuaterDepth));
	SetPolygon(29, pDownBoxDownFace);
}

BossMesh::~BossMesh(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallMesh::CWallMesh(float fWidth, float fHeight, float fDepth, int nSubRects) : CMesh((4 * nSubRects * nSubRects) + 2)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellHeight = fHeight * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	int k = 0;
	CPolygon *pLeftFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pLeftFace = new CPolygon(4);
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight+(i*fCellHeight), -fHalfDepth+(j*fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight+((i+1)*fCellHeight), -fHalfDepth+(j*fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight+((i+1)*fCellHeight), -fHalfDepth+((j+1)*fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight+(i*fCellHeight), -fHalfDepth+((j+1)*fCellDepth)));
			SetPolygon(k++, pLeftFace);
		}
	}

	CPolygon *pRightFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i*fCellHeight), -fHalfDepth + (j*fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1)*fCellHeight), -fHalfDepth + (j*fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1)*fCellHeight), -fHalfDepth + ((j + 1)*fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i*fCellHeight), -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(k++, pRightFace);
		}
	}

	CPolygon *pTopFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i*fCellWidth), +fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), +fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i*fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(k++, pTopFace);
		}
	}

	CPolygon *pBottomFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i*fCellWidth), -fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), -fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i*fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(k++, pBottomFace);
		}
	}

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(k++, pFrontFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(k++, pBackFace);
}

CWallMesh::~CWallMesh(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMesh::CAirplaneMesh(float fWidth, float fHeight, float fDepth) : CMesh(24)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1)*x2 + (fy - y3);
	int i = 0;

	//Upper Plane
	CPolygon *pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	//Lower Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	//Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	//Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);
}

CAirplaneMesh::~CAirplaneMesh()
{
}
