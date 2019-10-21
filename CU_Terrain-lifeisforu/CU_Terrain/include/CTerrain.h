/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CTERRAIN_H
#define CTERRAIN_H

#include "stdafx.h"
#include "CWorldTransform.h"
#include "CTriangleStripPlane.h"
#include "CUtility.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CustomVertex.h"

class CTerrain : public CWorldTransform
{
public:

    CTerrain();
    ~CTerrain() { Release(); }

    BOOL Initialize( LPDIRECT3DDEVICE9 pDevice, char* rawFile, char* terrainTexture );
    void Render( LPDIRECT3DDEVICE9 pDevice);
    void Release();

private:
    CVertexBuffer m_vb;
    CIndexBuffer m_ib;
    LPDIRECT3DTEXTURE9 m_pTexture;
    UCHAR* m_pHeight;
    UINT m_numVertices;
    UINT m_numIndices;
};

#endif