#include "Includes.h"

// the one and only global, and its the game object
CGameManager g_Game;
CSound g_Sound;

// This is where it all starts.. the entry point for the program..
int WINAPI WinMain(	HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, 
					int nCmdShow) 
{

	HWND hWnd;
	WNDCLASSEX winClass; 
	MSG msg;	

	// Setup and register the window class
	winClass.cbSize         = sizeof(WNDCLASSEX); 
	winClass.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc	= WndProc; 
	winClass.cbClsExtra		= 0;
	winClass.cbWndExtra		= 0; 
	winClass.hInstance		= hInstance; 
	winClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION); 
	winClass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); 
	winClass.lpszMenuName	= NULL; 
	winClass.lpszClassName	= WINDOW_CLASS_NAME; 
	winClass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winClass)) 
		return 0;

	// Try to initialize the Direct3D parts of our program. If it fails, make sure everything they might have been created gets released.
	if( !g_Game.Initilize(hInstance))
		return 0;
	

	if (g_Game.m_bFullscreen==false)
	{
		// Create a normal window with a border, a caption, and an X button
		hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,  
							 WINDOW_CLASS_NAME,     
							 WINDOW_TITLE, 
							 WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_VISIBLE, 
					 		 0, 0 ,
							 g_Game.m_displayMode.Width, g_Game.m_displayMode.Height,
							 NULL,
							 NULL,
							 hInstance,  
							 &g_Game);	
	}
	else
	{
		// Create a fullscreen window, one that doesn't have anything in it
		hWnd = CreateWindowEx(NULL,  
							 WINDOW_CLASS_NAME,     
							 WINDOW_TITLE, 
							 WS_POPUP | WS_VISIBLE, 
					 		 0, 0,
							 g_Game.m_displayMode.Width, g_Game.m_displayMode.Height,
							 NULL,
							 NULL,
							 hInstance,  
							 &g_Game);		
		

	}
	
	// Make sure the window was created properly. If it wasn't, quit.
	if (!hWnd) 
		return 0; 
	
	g_Game.Run(hWnd);
	//g_Sound.PlaySound();
	
	while(1)
	{
		// Windows message stuff
		while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
		    //TranslateMessage(&msg);
			if (msg.message == WM_QUIT)
				break;

		    DispatchMessage(&msg); 
		}
		
		if (msg.message==WM_QUIT)
			break;
	} 

	// Release Direct3D
	g_Game.Direct3DRelease();


	return 0;
} 