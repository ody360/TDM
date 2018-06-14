#include "Includes.h"
#include "gamemanager.h"

CGameManager::CGameManager(void)
{
	m_GameState = inMenu;  // Added by Hedim & Mike, March 31st
}

CGameManager::~CGameManager(void)
{
}

BOOL CGameManager::Initilize(HINSTANCE hInstance )
{
	m_hInst = hInstance;

	if (Direct3DInit()==false)
	{
		Direct3DRelease();
		return FALSE;
	}

	return TRUE;
}

void CGameManager::Run(HWND hWnd )
{
	m_hWnd = hWnd;
	if( !LoadD3Device() ) // loads d3d device..
		SendMessage( m_hWnd, WM_QUIT, 0, 0); // FATAL ERROR, QUIT and FAST!!

	if( !InitSceneObjects() ) // loads light & scene & menu
		SendMessage( m_hWnd, WM_QUIT, 0, 0); // FATAL ERROR, QUIT and FAST!!

	m_GameState = inMenu;
}


// NEED TO ADD CHECKING HERE TO SEE IF DEVICE HAS BEEN LOST!!
void CGameManager::RepairDeviceState()
{
	// NEED to attemp device RESTORE!!
	HRESULT deviceState = m_pDevice->TestCooperativeLevel();
	if( deviceState != D3D_OK && GetFocus() == m_hWnd )
	{
		static long timeToWait = 0;

		if( !timeToWait )
			timeToWait = GetTickCount();

		if( GetTickCount() - timeToWait > 1000 ) // wait 1/4 a second before trying to restore..
		{
			m_Menu.Release();
			m_theField.Release();

			if(  deviceState == D3DERR_DEVICENOTRESET )
			{
				timeToWait = 0;

				//DbgDumpObjectRegister();

				if( !LoadD3Device() )
				{
					Direct3DRelease();
					SendMessage( m_hWnd, WM_QUIT, 0, 0);
				}
				else //SUCCESS, device restored
					InitSceneObjects();
			}
			else if( deviceState == D3DERR_DEVICELOST )
			{
				Direct3DRelease();
				SendMessage( m_hWnd, WM_QUIT, 0, 0);
			}
		}
	}
}


void CGameManager::SetState( GameState newState )
{
	m_GameState = newState;

	if( m_GameState == gameOver || m_GameState == gameWon )
		m_GameStartTime = GetTickCount() - m_GameStartTime;
}

GameState CGameManager::getState()
{
	return m_GameState;
}

BOOL CGameManager::NewGame( iCoords size, int mines )
{
	m_GameState = playing;
	m_GameStartTime = GetTickCount();
	m_theField.Release();
	if( !m_theField.NewGame( m_pDevice, size, mines ))
		return FALSE;

	m_Menu.Release();
	m_Menu.RebuildVertexBuffer(m_pDevice);

	return TRUE;
}

void CGameManager::NewGame(void)
{
	m_GameState = playing;
	m_GameStartTime = GetTickCount();
	m_theField.NewGame( m_pDevice );
}

float CGameManager::GetGameTime()
{
	if( m_GameState == playing )
		return (GetTickCount() - m_GameStartTime)/1000.0f;
	else if( m_GameState == gameOver || m_GameState == gameWon )
		return m_GameStartTime/1000.0f;
	else
		return 0.0f;

}

void CGameManager::Update()
{
	if( !m_pDevice )
		return;

	if( m_pDevice->TestCooperativeLevel() == D3D_OK )
	{
		m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0,0), 1.0f, 0 );
		// Make it so we can draw stuff
		m_pDevice->BeginScene();

		if(m_GameState == inMenu)
		{
			//OutputDebugString( "gamestate == inMenu\n");
			m_GameState = playing;
			m_theField.Redraw( m_pDevice );
			m_GameState = inMenu;

			m_Menu.Redraw( m_pDevice );
		}
		else
			m_theField.Redraw( m_pDevice );

		m_pDevice->EndScene();
		// Put the back buffer onto the screen
		m_pDevice->Present(NULL, NULL, NULL, NULL);

	}
	else
		RepairDeviceState();
}


bool CGameManager::Direct3DInit()
{
	// Create the IDirect3D object
	m_pDirect3D = Direct3DCreate8(D3D_SDK_VERSION);
	if (m_pDirect3D==NULL)
		return FALSE;
	INT_PTR hresult = DialogBoxParam(m_hInst, MAKEINTRESOURCE(IDD_SETUP), 0, DialogProc, (LPARAM)this);
	DWORD errocode = GetLastError();

	if( hresult == IDOK )
		return TRUE;
	else
		return FALSE;	
}


// this is called during program init, and if we loose the device
bool CGameManager::LoadD3Device()
{
	HRESULT hr;
	// Set up a structure with either the current display mode for windowed mode or the desired display mode for fullscreen
	m_displayMode.RefreshRate=0; // forces default mode..

	if( m_bFullscreen == false )
	{
		m_displayMode.Width=320;
		m_displayMode.Height=240;
	}

	// Setup the present parameters
	D3DPRESENT_PARAMETERS presentParameters;
	memset(&presentParameters, 0, sizeof(D3DPRESENT_PARAMETERS));
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = m_displayMode.Format;
	presentParameters.BackBufferWidth = m_displayMode.Width;
	presentParameters.BackBufferHeight = m_displayMode.Height;

	// zbuffer setup
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT ;

	if (m_bFullscreen==false)
		presentParameters.Windowed   = TRUE;
	else
		presentParameters.Windowed   = FALSE;


	if(  m_pDevice && m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		hr = m_pDevice->Reset( &presentParameters );
	else // Create the device
	{
		hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
			                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				                      &presentParameters, &m_pDevice ); 
		if( FAILED(hr) )
			hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										&presentParameters, &m_pDevice ); 
	}

	if (FAILED(hr))
		return false;

	// Matrix code
	D3DXMATRIX mat;
	
	D3DXMatrixPerspectiveFovLH(&mat, D3DX_PI/6, (float)m_displayMode.Width/(float)m_displayMode.Height, 1.0, 100.0);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &(D3DMATRIX)mat);

	D3DXMatrixIdentity(&mat);
	m_pDevice->SetTransform(D3DTS_WORLD, &(D3DMATRIX)mat);

	D3DXMatrixTranslation(&mat, 0, 0, 75);
	m_pDevice->SetTransform(D3DTS_VIEW, &(D3DMATRIX)mat);

	// set timer for redraw
	SetTimer(m_hWnd, 1, 10, 0 );

	return true;
}

BOOL CGameManager::InitSceneObjects()
{

	if( !m_theField.RebuildVertexBuffer( m_pDevice ) )
		return FALSE;

	if( !m_Menu.RebuildVertexBuffer(m_pDevice) )
		return FALSE;

	//Let there be light
	D3DLIGHT8 d3dLight;

	//Initialize the light structure.
	ZeroMemory(&d3dLight, sizeof(D3DLIGHT8));

	//Set up a white point light at (0, 0, -10).
	d3dLight.Type = D3DLIGHT_POINT;

	d3dLight.Diffuse.r = 1.0f;
	d3dLight.Diffuse.g = 1.0f;
	d3dLight.Diffuse.b = 1.0f;

	d3dLight.Ambient.r = 0.0f;
	d3dLight.Ambient.g = 0.0f;
	d3dLight.Ambient.b = 0.0f;

	d3dLight.Specular.r = 0.0f;
	d3dLight.Specular.g = 0.0f;
	d3dLight.Specular.b = 0.0f;

	d3dLight.Position.x = 0.0f;
	d3dLight.Position.y = 0.0f;
	d3dLight.Position.z = -50;

	d3dLight.Attenuation0 = 0.05f; 
	d3dLight.Attenuation1 = 0.05f; 
	d3dLight.Attenuation2 = 0.0f;
	d3dLight.Range = 100.0;

	//Assign the point light to our device in poisition (index) 0
	m_pDevice->SetLight(0, &d3dLight);

	//Enable our point light in position (index) 0
	m_pDevice->LightEnable(0, TRUE);

	//Turn on lighting
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//Set ambient light level
	m_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(32, 32, 32));

	// Make it so that it'll draw triangles facing either towards or away from the camera
    m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE  );
	// Turn off Direct3D's lighting since we use our own vertex color.
	// enable z-buffer
	m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState( D3DRS_CLIPPING, FALSE);


	return TRUE;
}

void CGameManager::Direct3DRelease()
{
	m_Menu.Release();
	m_theField.Release();

	// Release the device
	//SAFE_RELEASE( m_pDevice )
	// Release the Direct3D object
	//SAFE_RELEASE( m_pDirect3D )

	while( m_pDevice && m_pDevice->Release() )
		;
	while( m_pDirect3D && m_pDirect3D->Release() )
		;

	m_pDirect3D = 0;
	m_pDevice = 0;
} 



LRESULT CALLBACK WndProc(HWND wpHWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CGameManager* Game = (CGameManager*)GetWindowLongPtr( wpHWnd, GWLP_USERDATA );
	switch(msg)
	{
		case WM_CREATE:
			SetWindowLongPtr( wpHWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams );// sets extra window data to g_Game*
			break;

		case WM_MOUSEMOVE:
            
			if( !Game->m_bFullscreen ) // must adjust so that x, y match client like fullscreen..
				Game->m_theField.OnMouseMove( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)+22, wParam );
			else
				Game->m_theField.OnMouseMove( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam );


			// Update the scene each frame
			if( Game->getState() != inMenu )
				Game->m_theField.MoveScene();
			// Draw the scene each frame
			Game->Update();

			break;
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			if( g_Game.getState() == inMenu )
			{
				if( !Game->m_bFullscreen ) // must adjust so that x, y match client like fullscreen..
					Game->m_Menu.OnMouseClick( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)+22, wParam, msg );
				else
					Game->m_Menu.OnMouseClick( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam, msg);
				//Insert code here to interact with the menu 
				//...
				g_Game.SetState( playing );
			}
			else
			{
				if( !Game->m_bFullscreen ) // must adjust so that x, y match client like fullscreen..
					Game->m_theField.OnMouseClick( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)+22, wParam, msg);
				else
					Game->m_theField.OnMouseClick( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam, msg);

				Game->m_theField.MoveScene();
				Game->Update();
			}

			break;
		case WM_TIMER:
			Game->m_theField.MoveScene();
			Game->Update();
			break;
		case WM_DESTROY: 
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE )
				Game->SetState(inMenu);
			else if( wParam == VK_F2 )
				Game->NewGame();
			else if( GetAsyncKeyState(VK_MENU) && wParam == VK_F4 )
				PostQuitMessage(0);

			break;

		default:break; 
	} 

	return DefWindowProc(wpHWnd, msg, wParam, lParam);
}


BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int index = 0;
	char temp[128];
    UINT nModes = 0;
	D3DDISPLAYMODE displayMode;
	CGameManager* Game = (CGameManager*)GetWindowLongPtr( hwnd, GWLP_USERDATA );

	switch (message)
    {
    case WM_INITDIALOG:
		Game = (CGameManager*)lParam; // get window pointer above doesnt work on the first call..
		SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)lParam );// sets extra window data to g_Game*

		index = 0;
		nModes = Game->m_pDirect3D->GetAdapterModeCount( D3DADAPTER_DEFAULT );
		memset( &displayMode, 0, sizeof(D3DDISPLAYMODE) );
		
		for( int i = nModes-1; i >= 0; i-- )
		{
			if( D3D_OK == Game->m_pDirect3D->EnumAdapterModes( D3DADAPTER_DEFAULT, i, &Game->m_displayMode ) )
			{
				if( ( displayMode.Width != Game->m_displayMode.Width ||	// cull out low refresh rates..
					displayMode.Height != Game->m_displayMode.Height ||
					displayMode.Format != Game->m_displayMode.Format ) && 
					Game->m_displayMode.Width > 512 ) // cull out low resolutions
				{
					Game->m_displayMode.RefreshRate = 0; // forces default

					if( Game->m_displayMode.Format == D3DFMT_X8R8G8B8 ||
						Game->m_displayMode.Format == D3DFMT_A8R8G8B8 ||
						Game->m_displayMode.Format == D3DFMT_R8G8B8 )
						sprintf( temp, "%iX%iX32", Game->m_displayMode.Width, Game->m_displayMode.Height );
					else
						sprintf( temp, "%iX%iX16", Game->m_displayMode.Width, Game->m_displayMode.Height );

					SendMessage( GetDlgItem(hwnd,IDC_RES_BOX), CB_INSERTSTRING, (WPARAM)index, (LPARAM)&temp );
					SendMessage( GetDlgItem(hwnd,IDC_RES_BOX), CB_SETITEMDATA, (WPARAM)index, (LPARAM)(DWORD)i );
					index++;
				}
				displayMode = Game->m_displayMode;
			}
		}
		index = SendMessage( GetDlgItem(hwnd,IDC_RES_BOX), CB_GETCOUNT, 0, 0);
		SendMessage( GetDlgItem(hwnd,IDC_RES_BOX), CB_SETCURSEL, (WPARAM)index/2, 0 );
		CheckDlgButton( hwnd, IDC_FULLSCREEN, BST_CHECKED );
        return TRUE;
    case WM_COMMAND:
		switch (LOWORD(wParam)) 
        { 
            case IDOK:
				UINT selItem;
				UINT displayModeID;
				selItem = SendMessage( GetDlgItem(hwnd,IDC_RES_BOX), CB_GETCURSEL, 0, 0);
				
				// get selected display mode
				displayModeID = SendMessage( GetDlgItem(hwnd,IDC_RES_BOX),
											CB_GETITEMDATA, (WPARAM)selItem, 0);

				Game->m_pDirect3D->EnumAdapterModes( D3DADAPTER_DEFAULT, displayModeID, &Game->m_displayMode );

				// get fullscreen
				Game->m_bFullscreen = IsDlgButtonChecked( hwnd, IDC_FULLSCREEN) == BST_CHECKED;

				if( !Game->m_bFullscreen )
				{
					g_Game.m_displayMode.Width = 320;
					g_Game.m_displayMode.Height = 240;
				}

				// fall through
			case IDCANCEL:
				EndDialog(hwnd, wParam); 
				return TRUE;

			case IDC_FULLSCREEN:
				if( HIWORD(wParam) == BN_CLICKED )
				{
					bool checked = IsDlgButtonChecked( hwnd, IDC_FULLSCREEN ) == BST_CHECKED;
					EnableWindow( GetDlgItem(hwnd, IDC_RES_BOX), checked );
				}
				break;
		}
        return TRUE;
    case WM_DESTROY:
        //PostQuitMessage(0);
        return TRUE;
    case WM_CLOSE:
        DestroyWindow (hwnd);
        return TRUE;
    }
    return FALSE;
}

