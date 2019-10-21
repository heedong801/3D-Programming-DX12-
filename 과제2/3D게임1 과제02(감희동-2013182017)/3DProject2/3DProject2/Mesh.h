#pragma once

enum Pos{ FRONTBOX= 101, MIDBOX, BACKBOX };

class CMesh
{
public:  
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);    
	virtual ~CMesh();
private:
	int m_nReferences = 0;
public: 
	void AddRef() { m_nReferences++; } 
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected: 
	ID3D12Resource * m_pd3dVertexBuffer = NULL; 
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	UINT m_nSlot = 0; 
	UINT m_nVertices = 0; 
	UINT m_nStride = 0; 
	UINT m_nOffset = 0;

public: 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected: 
	ID3D12Resource * m_pd3dIndexBuffer = NULL;
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL; 
	
	/*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�.
	�ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/ 
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	
	UINT m_nIndices = 0; 
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�. 
	
	UINT m_nStartIndex = 0; 
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�. 
	
	int m_nBaseVertex = 0; 
	//�ε��� ������ �ε����� ������ �ε����̴�.
public:
	BoundingOrientedBox			m_xmOOBB;

	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }
};

//������ ǥ���ϱ� ���� Ŭ������ �����Ѵ�.
class CVertex
{ 
protected: 
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�).   
	XMFLOAT3 m_xmf3Position;

public:
	CVertex()
	{ 
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	} 
	CVertex(XMFLOAT3 xmf3Position) 
	{ 
		m_xmf3Position = xmf3Position;
	} 
	~CVertex()
	{ 
	}
};

class CDiffusedVertex : public CVertex 
{
protected: 
	//������ �����̴�.  
	XMFLOAT4 m_xmf4Diffuse;
public: 
	CDiffusedVertex() 
	{ 
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	} 
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) 
	{ 
		m_xmf3Position = XMFLOAT3(x, y, z);
		m_xmf4Diffuse = xmf4Diffuse; 
	} 
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse)
	{
		m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CTriangleMesh : public CMesh 
{ 
public:  
	CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CTriangleMesh() { } 
};

class CCubeMeshColorful : public CMesh
{
public: 
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshColorful(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshColorful();
};

class CAirplaneMeshDiffused : public CMesh 
{
public: 
	CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f)); 
	virtual ~CAirplaneMeshDiffused(); 
};

class CWallMesh : public CMesh
{
public:
	CWallMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, XMFLOAT4 xmf4Diffuse = XMFLOAT4(1,1,1,1), int CheckTree = 0);
	virtual ~CWallMesh();
}; 

class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, XMFLOAT4 xmf4Diffuse = XMFLOAT4(1, 1, 1, 1));
	virtual ~CCubeMeshDiffused();
};

class CCubeMeshSimilar : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshSimilar(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, XMFLOAT4 xmf4Diffuse = XMFLOAT4(1, 1, 1, 1));
	virtual ~CCubeMeshSimilar();
};

class CCubeMeshRandom : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshRandom(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshRandom();
};