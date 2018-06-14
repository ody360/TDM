#include "gamemenu.h"
#include "Includes.h"

CGameMenu::CGameMenu(void)
{
	memset(this,0,sizeof(CGameMenu) );
}

CGameMenu::~CGameMenu(void)
{
}

void CGameMenu::Release()
{
	//SAFE_RELEASE( m_pVertexBuffer )
	//SAFE_RELEASE( m_Texture )

	while( m_pVertexBuffer && m_pVertexBuffer->Release() );
	while( m_Texture && m_Texture->Release() );

	m_pVertexBuffer = 0;
	m_Texture = 0;
}

void CGameMenu::Redraw( IDirect3DDevice8* device )
{
    D3DXMATRIX  matWorld;
    D3DXMATRIX  matInvsView;
    D3DXMATRIX  matXWorld;
    D3DXMATRIX  matYWorld;

	D3DXVECTOR3 vCameraPosition;
	D3DXVECTOR3 vBillPosition;

    // *****  Draw the menu ****** //
	device->SetMaterial(&m_matMaterial);
	device->SetVertexShader(D3DFVF_BILLVERTEX);
	device->SetStreamSource(0,m_pVertexBuffer,sizeof(BillVertex));
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Tell it to use our texture
	// kinda works, hides ones behind it //m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	//device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1  );
	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	device->SetTexture(0, m_Texture);

	D3DVIEWPORT8 viewPort;
	device->GetViewport( &viewPort );
	GetRayFromMouse( device, viewPort.Width/2, viewPort.Height/2, D3DXVECTOR3(), vCameraPosition);

	//if( g_Game.getState() != inMenu )
	//{
//		D3DXMatrixTranslation(&matWorld, 0, 0, g_Game.m_theField.getCameraDistance()*.7f );
//		device->SetTransform( D3DTS_WORLD, &matWorld );
	//}

	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 10); //draw menu..

	// release texture & vertex buffer
	device->SetStreamSource(0,0,0);
	device->SetTexture(0, NULL);

    // Restore device state
    D3DXMatrixIdentity( &matWorld );
    device->SetTransform( D3DTS_WORLD, &matWorld );

}

// x and y are the mouse coordinates in the client window
void CGameMenu::GetRayFromMouse(IDirect3DDevice8 *device, int x, int y, D3DXVECTOR3 &vPickRayDir, D3DXVECTOR3 &vPickRayOrig)
{
	D3DXMATRIX matProj;

	device->GetTransform( D3DTS_PROJECTION, &matProj );

	POINT ptCursor = { x, y };

	D3DVIEWPORT8 viewPort;
	D3DXVECTOR3 v;
	UINT width, height;

	device->GetViewport( &viewPort );

	width = viewPort.Width;
	height = viewPort.Height;

	v.x =  ( ( ( 2.0f * ptCursor.x ) / width  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / height ) - 1 ) / matProj._22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIX matView, m;
	device->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixInverse( &m, NULL, &matView );

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
}


BOOL CGameMenu::RebuildVertexBuffer( IDirect3DDevice8 * device)
{
	HRESULT g_hr;
	
	BillVertex* pVerts = 0;

	if( m_Texture )
		Release();
	//Menu size actually changes how far from view in Z direction you want menu to be
	//Larger number = farther(smaller) menu.
	float menuSize = 7.0;
	float scale = 0.0;
	float fDistanceToCamera = 1.0;
	
	// create the material for the cube
	//Set material default values (R, G, B, A)
	D3DCOLORVALUE rgbaDiffuse = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaAmbient = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaSpecular = {0.0, 0.0, 0.0, 0.0,};
	D3DCOLORVALUE rgbaEmissive = {0.0, 0.0, 0.0, 0.0,};

	m_matMaterial.Diffuse = rgbaDiffuse;
	m_matMaterial.Ambient  = rgbaAmbient;
	m_matMaterial.Emissive = rgbaEmissive;
	m_matMaterial.Specular = rgbaSpecular;
	m_matMaterial.Power = 0.0;

	// now load menu texture
    if( FAILED( D3DXCreateTextureFromFile( device, "MainMenu.tga", &m_Texture ) ) )
       return FALSE;

	// ****** first build the menu verticies ******** //

	// create the vertex buffer..
	g_hr = device->CreateVertexBuffer( 12*sizeof(BillVertex),
										D3DUSAGE_POINTS,
										D3DFVF_BILLVERTEX,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer );

	if( FAILED(g_hr))
		return FN_FAILED;

	g_hr = m_pVertexBuffer->Lock( 0, 12*sizeof(BillVertex), (BYTE**)&pVerts, 0 );

	if( FAILED(g_hr))
		return FN_FAILED;
	
	// The next two lines determine Z depth of menu. Regardless of cube distance from camera,
	// the menu will stay the same distance from view.
	scale = (menuSize-g_Game.m_theField.getCameraDistance())/(-g_Game.m_theField.getCameraDistance());
	fDistanceToCamera = g_Game.m_theField.getCameraDistance()*scale;

	pVerts[11].SetXYZ(-1.5,-1, -fDistanceToCamera);
	pVerts[10].SetXYZ(1.5,-1, -fDistanceToCamera);
	pVerts[9].SetXYZ(-1.5,-.5, -fDistanceToCamera);
	pVerts[8].SetXYZ(1.5,-.5, -fDistanceToCamera);
	pVerts[7].SetXYZ(-1.5,0, -fDistanceToCamera);
	pVerts[6].SetXYZ(1.5,0, -fDistanceToCamera);
	pVerts[5].SetXYZ(-1.5,.5, -fDistanceToCamera);
	pVerts[4].SetXYZ(1.5,.5, -fDistanceToCamera);
	pVerts[3].SetXYZ(-1.5,1, -fDistanceToCamera);
	pVerts[2].SetXYZ(1.5,1, -fDistanceToCamera);
	pVerts[1].SetXYZ(-1.5,1.5, -fDistanceToCamera);
	pVerts[0].SetXYZ(1.5,1.5, -fDistanceToCamera);

	pVerts[0].SetUV(1,0);
	pVerts[1].SetUV(0,0);
	pVerts[2].SetUV(1,0.2f);
	pVerts[3].SetUV(0,0.2f);
	pVerts[4].SetUV(1,0.4f);
	pVerts[5].SetUV(0,0.4f);
	pVerts[6].SetUV(1,0.6f);
	pVerts[7].SetUV(0,0.6f);
	pVerts[8].SetUV(1,.8f);
	pVerts[9].SetUV(0,.8f);
	pVerts[10].SetUV(1,1);
	pVerts[11].SetUV(0,1);

	m_pVertexBuffer->Unlock();

	// ****** done building the menu ******** //

	return TRUE;
}

menuState CGameMenu::FindIntersect( int x, int y)
{
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vert1;
	D3DXVECTOR3 vert2;
	D3DXVECTOR3 vert3;
	int numVerts = 12;

	BillVertex* pVerts = 0;
	//float textCordsx, textCordsy;

	menuState option = basic;

	// get the ray from the mouse
	GetRayFromMouse( g_Game.m_pDevice, x, y, vPickRayDir, vPickRayOrig );

	// lock vertex buffer..
	m_pVertexBuffer->Lock( 0, numVerts*sizeof(BillVertex), (BYTE**)&pVerts, 0 );

	for( int vertex = 0; vertex < numVerts; vertex += 1 )
	{
		vert1 = (D3DXVECTOR3)pVerts[vertex];
		vert2 = (D3DXVECTOR3)pVerts[vertex+1];
		vert3 = (D3DXVECTOR3)pVerts[vertex+2];

		if( D3DXIntersectTri( &vert1, &vert2, &vert3,
								&vPickRayOrig, &vPickRayDir,
								/*&textCordsx*/ 0, /*&textCordsy*/ 0, 0 ) )
		{
			switch( vertex )
			{
			case 2:
			case 3:
				option = basic;
				g_Game.SetState(playing);
				break;
			case 4:
			case 5:
				option = intermediate;
				g_Game.SetState(playing);
				break;
			case 6:
			case 7:
				option = expert;
				g_Game.SetState(playing);
				break;
			case 8:
			case 9:
				option = quit;
				g_Game.SetState(playing);
				break;
			default:
				break;
			}
		}

	}

		
	m_pVertexBuffer->Unlock();
	return option;
}

void CGameMenu::OnMouseClick( int x, int y, WPARAM ButtonState, UINT mouseButton )
{
	if( g_Game.getState() == inMenu )
	{
		menuState state = FindIntersect( x, y );
		switch( state )
		{
		case 1: //basic
			g_Game.SetState(playing);
			g_Game.NewGame(iCoords(3,3,3), 7);	// 12% mines
			break;
		case 2: //intermediate
			g_Game.SetState(playing);
			g_Game.NewGame(iCoords(5,5,5), 31);	// 15% mines
			break;
		case 3: //expert
			g_Game.SetState(playing);
			g_Game.NewGame(iCoords(10,10,10), 250); // 20% mines
			break;
		case 0: //exit
			PostQuitMessage(0);
			break;
		}
	}
}