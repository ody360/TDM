#pragma once
#include "Includes.h"
#define GAMEMANAGER
#include "MineField.h"
#include "GameMenu.h"
#undef GAMEMANAGER

enum GameState { playing = 0, gameOver = 1, inMenu = 2, gameWon = 3 };

class CGameManager
{
	// friend functions, they cant be member functions because
	// all member funcitons have the "hidden" paramter added, the 'this' pointer
	// Windows message processing function
	friend LRESULT CALLBACK WndProc(HWND wpHWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	// message processing function for dialog
	friend BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend class CMineField;
	friend class CGameMenu;
public:
	CGameManager();
	~CGameManager(void);

	BOOL Initilize(HINSTANCE hInstance); // call for direct3d initilization NOT device init
	void Run(HWND hWnd );
	void Update();	// call to have everything redrawn
	
	void SetState( GameState newState );
	GameState getState();
	
	BOOL NewGame( iCoords size, int mines );
	float GetGameTime();

	// realease everything
	void Direct3DRelease();
	BOOL InitSceneObjects();

	// Whether or not it's fullscreen, and display mode
	bool m_bFullscreen;
	D3DDISPLAYMODE m_displayMode;
	IDirect3DDevice8 *m_pDevice;

private:
	// describes game state
	GameState m_GameState;
	long m_GameStartTime;

	// creates and initilizes the Direct3D object
	bool Direct3DInit();

	// creates and initlizes the Direct3D device
	bool LoadD3Device();


	void RepairDeviceState();

	// Important Direct3D variables
	IDirect3D8 *m_pDirect3D;

	// The one and only minefield
	CMineField m_theField;
	CGameMenu m_Menu;
	
	HWND m_hWnd;
	HINSTANCE m_hInst;

public:
	void NewGame(void);
};
