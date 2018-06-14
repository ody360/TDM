#include "Includes.h"
#ifdef GAMEMANAGER
#pragma once


enum menuState { quit, basic, intermediate, expert };

class CGameMenu
{
public:
	CGameMenu(void);
	~CGameMenu(void);
	
	void GetRayFromMouse(IDirect3DDevice8 *device, int x, int y, D3DXVECTOR3 &vPickRayDir, D3DXVECTOR3 &vPickRayOrig);
	
	void Redraw(IDirect3DDevice8 *device);
	BOOL RebuildVertexBuffer(IDirect3DDevice8 *device);

	menuState FindIntersect( int x, int y);
	void OnMouseClick( int x, int y, WPARAM ButtonState, UINT mouseButton );

	void Release();

private:
		IDirect3DVertexBuffer8* m_pVertexBuffer;
		LPDIRECT3DTEXTURE8 m_Texture; // cube texture
		D3DMATERIAL8 m_matMaterial;
};

#endif