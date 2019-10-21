/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : WinMain.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain

Created :  08/26/2005
Modified : 12/06/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CGameApp.h"

char* g_instructions = "WASD: Move the terrain\r\nQE: Scale the mountains\r\nClick and drag the mouse to rotate the terrain\r\nEsc: Quit\r\nF5: Toggle fullscreen\r\nF6: Toggle wireframe";

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Default constructor
Parameters:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CGameApp::CGameApp()
{
    m_pFramework = NULL;
    m_showInstructions = FALSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Clean up resources
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::Release()
{
    SAFE_RELEASE( m_pFramework );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Sets the CFramework instnace of the application.
Parameters:
[in] pFramework - Pointer to a CFramework instance
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::SetFramework( CFramework* pFramework )
{
    m_pFramework = pFramework;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Initialize application-specific resources and states here.
Returns: TRUE on success, FALSE on failure
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CGameApp::Initialize()
{
    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: 
This callback function will be called immediately after the Direct3D device has been created. This is 
the best location to create D3DPOOL_MANAGED resources. Resources created here should be released in 
the OnDestroyDevice callback. 
Parameters:
[in] pDevice - Pointer to a DIRECT3DDEVICE9 instance
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnCreateDevice( LPDIRECT3DDEVICE9 pDevice )
{
    D3DXCreateSprite( pDevice, &m_pTextSprite );
    m_font.Initialize( pDevice, "Arial", 12 );

    m_terrain.Initialize( pDevice, "heightMap.raw", "terrain.jpg" );
    m_terrain.ScaleAbs( 0.5f, 0.15f, 0.5f );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: 
This callback function will be called immediately after the Direct3D device has been created. This is 
the best location to create D3DPOOL_DEFAULT resources. Resources created here should be released in 
the OnLostDevice callback. 
Parameters:
[in] pDevice - Pointer to a DIRECT3DDEVICE9 instance
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnResetDevice( LPDIRECT3DDEVICE9 pDevice )
{
    m_pTextSprite->OnResetDevice();
    m_font.OnResetDevice();

    // Set transforms
    D3DXVECTOR3 cameraPosition( 0.0f, 50.0f, 80.0f );
    D3DXVECTOR3 cameraTarget( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 cameraUp( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX viewMatrix;
    D3DXMatrixLookAtLH( &viewMatrix, &cameraPosition, &cameraTarget, &cameraUp );
    pDevice->SetTransform( D3DTS_VIEW, &viewMatrix );
    D3DXMATRIX projection;
    float aspect = (float)m_pFramework->GetWidth() / (float)m_pFramework->GetHeight();
    D3DXMatrixPerspectiveFovLH( &projection, D3DX_PI / 3.0f, aspect, 0.1f, 1000.0f );
    pDevice->SetTransform( D3DTS_PROJECTION, &projection );

    // Set render states
    pDevice->SetRenderState( D3DRS_FILLMODE, m_pFramework->GetFillMode() );      
    pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );  
    pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: 
This callback function will be called immediately after the Direct3D device has entered a lost state 
and before IDirect3DDevice9::Reset is called. Resources created in the OnResetDevice callback should 
be released here, which generally includes all D3DPOOL_DEFAULT resources.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnLostDevice()
{
    m_pTextSprite->OnLostDevice();
    m_font.OnLostDevice();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: 
This callback function will be called immediately after the Direct3D device has been destroyed. 
Resources created in the OnCreateDevice callback should be released here, which generally includes 
all D3DPOOL_MANAGED resources.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnDestroyDevice()
{
    SAFE_RELEASE( m_pTextSprite );
    m_font.Release();
    m_terrain.Release();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Updates the current frame.
Parameters:
[in] pDevice - Pointer to a DIRECT3DDEVICE9 instance
[in] elapsedTime - Time elapsed since last frame
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnUpdateFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime )
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Renders the current frame.
Parameters:
[in] pDevice - Pointer to a DIRECT3DDEVICE9 instance
[in] elapsedTime - Time elapsed since last frame
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::OnRenderFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime )
{
    sprintf( m_fps, "%.2f fps", m_pFramework->GetFPS() );

    pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 100, 200, 255 ), 1.0f, 0 ); 
    pDevice->BeginScene();

    m_terrain.Render( pDevice );

    // Display framerate and instructions
    m_pTextSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
    m_font.Print( m_fps, 5, 5, D3DCOLOR_XRGB( 255, 0, 0 ), m_pTextSprite );
    if ( m_showInstructions )
    {
        m_font.Print( g_instructions, 5, 20, D3DCOLOR_XRGB( 255, 255, 255 ), m_pTextSprite );
    }
    else
    {
        m_font.Print( "Hit F1 to view the instructions.", 5, 20, D3DCOLOR_XRGB( 255, 255, 255 ), m_pTextSprite );
    }
    m_pTextSprite->End();

    pDevice->EndScene();
    pDevice->Present( 0, 0, 0, 0 );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Responds to key presses
Parameters:
[in] xDelta - Change in mouse x-axis since last frame
[in] yDelta - Change in mouse y-axis since last frame
[in] zDelta - Change in mouse z-axis since last frame
[in] pMouseButtons - Mouse button states
[in] pPressedKeys - Keyboard keys that are pressed and not locked
[in] elapsedTime - Time elapsed since last frame
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGameApp::ProcessInput( long xDelta, long yDelta, long zDelta, BOOL* pMouseButtons, BOOL* pPressedKeys, float elapsedTime )
{
    if ( pMouseButtons[0] )
    {
        m_terrain.RotateRel( yDelta * .01f, xDelta * .01f, 0.0f );
    }
    if ( pPressedKeys[DIK_W] )
    {
        m_terrain.TranslateRel( 0.0f, 20.0f * elapsedTime, 0.0f );
    }
    if ( pPressedKeys[DIK_A] )
    {
        m_terrain.TranslateRel( -20.0f * elapsedTime, 0.0f, 0.0f );
    }
    if ( pPressedKeys[DIK_S] )
    {
        m_terrain.TranslateRel( 0.0f, -20.0f * elapsedTime, 0.0f );
    }
    if ( pPressedKeys[DIK_D] )
    {
        m_terrain.TranslateRel( 20.0f * elapsedTime, 0.0f, 0.0f );
    }
    if ( pPressedKeys[DIK_Q] )
    {
        m_terrain.ScaleRel( 0.0f, -0.5f * elapsedTime, 0.0f );
    }
    if ( pPressedKeys[DIK_E] )
    {
        m_terrain.ScaleRel( 0.0f , 0.5f * elapsedTime, 0.0f  );
    }
    if ( pPressedKeys[DIK_ESCAPE] )
    {
        m_pFramework->LockKey( DIK_ESCAPE );
        PostQuitMessage( 0 );
    }
    if ( pPressedKeys[DIK_F1] )
    {
        m_pFramework->LockKey( DIK_F1 );
        m_showInstructions = !m_showInstructions;
    }
    if ( pPressedKeys[DIK_F5] )
    {
        m_pFramework->LockKey( DIK_F5 );
        if ( m_pFramework != NULL )
        {
            m_pFramework->ToggleFullscreen();
        }
    }
    if ( pPressedKeys[DIK_F6] )
    {
        m_pFramework->LockKey( DIK_F6 );
        if ( m_pFramework != NULL )
        {
            m_pFramework->ToggleWireframe();
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Application entry point
Parameters:
[in] hInstance - Application instance
[in] hPrevInstance - Junk
[in] lpCmdLine - Command line arguments
[in] nCmdShow - Window display flags
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{ 
    CGameApp* pApplication = new CGameApp();
    CFramework* pFramework = new CFramework( (CBaseApp*)pApplication );

    pApplication->SetFramework( pFramework );

    // Initialize any application resources
    if ( !pApplication->Initialize() )
    {
        return 0;
    }

    // Initialize the Framework
    if ( !pFramework->Initialize( "Terrain", hInstance, 640, 480, TRUE ) )
    {
        return 0;
    }

    // Rock and roll
    pFramework->Run();
    
    // Clean up resources
    SAFE_RELEASE( pApplication );

    return 0;
}