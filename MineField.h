#include "Includes.h"

#ifdef GAMEMANAGER
#pragma once

class CDataPoint;

struct iCoords
{
	iCoords() {};
	iCoords( int ix, int iy, int iz ) { x = ix; y = iy; z = iz; };
	void set( int ix, int iy, int iz ) { x = ix; y = iy; z = iz; };
	int x, y, z;
};

struct fCoords
{
	fCoords() {};
	fCoords( float fw, float fh, float fd ) { fWidth = fw; fHeight = fh; fDepth = fd; };
	float fWidth, fHeight, fDepth;
};

class billBoardInfo
{
public:
	bool operator<( billBoardInfo &other ) { return other.fDistance < fDistance; };
	
	iCoords cube;
	float fDistance;
};

class CMineField
{
	//WHAT encapsulation?  I don't see any...  ~Mike, 04/09/2003
	friend LRESULT CALLBACK WndProc(HWND wpHWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	CMineField(void);
	~CMineField(void);

	// Game Logic functions
	// This function initializes the logic of the mine field.  Thus, the term "new game."
	BOOL NewGame( IDirect3DDevice8 * device, iCoords cords, int numMines );
	void NewGame(IDirect3DDevice8* device); // restarts the current size game..


	// interaction and window maintnance
	void OnMouseMove( int x, int y, WPARAM ButtonState );
	void OnMouseClick( int x, int y, WPARAM ButtonState, UINT mouseButton );
	void Redraw(IDirect3DDevice8 *device);
	void MoveScene();
	BOOL RebuildVertexBuffer(IDirect3DDevice8 *device);
	void Release();
	BOOL isGameWon();
	float getCameraDistance();

private:
	void AdjustMineCount();  //Clears & resets #adj property for each datapoint ~Mike 4/9/2003
	void GetRayFromMouse(int x, int y, D3DXVECTOR3 &vPickRayDir, D3DXVECTOR3 &vPickRayOrig);
	D3DVECTOR GetTriangleNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);
	iCoords FinderIntersect( int x, int y);

	void ChainClear(int x, int y, int z);  /*Does the rapid clearing of blank squares.
										     ~Mike, March 28th*/

	void ApplyAdjcent( void (CMineField::*pm_fn)(CDataPoint*, iCoords), iCoords center );

	IDirect3DVertexBuffer8* m_pVertexBuffer;
	IDirect3DVertexBuffer8* m_pNumberVertexBuffer;
	LPDIRECT3DTEXTURE8 m_CubeTexture; // cube texture
	LPDIRECT3DTEXTURE8 m_CubeBombTexture; // cube texture
	LPDIRECT3DTEXTURE8 m_BombTexture; // cube texture
	LPDIRECT3DTEXTURE8 m_CubeNotBombTexture; // cube texture
	LPDIRECT3DTEXTURE8 m_NumberTextures[5];
	D3DMATERIAL8 m_matMaterial;
	LPD3DXFONT m_Font;

	// sprite used for explosion..
	struct explosion_Stuff
	{
		long m_Time;
		int m_Step;
		D3DXVECTOR3 m_Center;
		LPDIRECT3DTEXTURE8 m_ExplosionTexture;
        LPD3DXSPRITE m_ExplosionSprite; 
	}m_Explosion;

	CDataPoint*** m_pDataField;

	iCoords m_Current; //where the mouse pointer is
	iCoords m_Size;
	fCoords m_CubeSize;
	
	int m_MineCount;

	float m_fDistanceToCamera;

	int m_iRotationDegreesX;
	int m_iRotationDegreesY;

};

#endif