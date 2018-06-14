// Includes for Windows and DirectX
#pragma once

#ifndef INCLUDES

#include <windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <D3dx8math.h>
#include <list>
//Sound include
#include <dmusicc.h>
#include <dmusici.h>
#include <cguid.h>
//Sound define
#include "Sound.h"
//#define INITGUID

// helper for releasing objects
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

// dialog stuff
#include "resource.h"

// My files
#include "CVertex.h"

#include "DataPoint.h"
#include "MineField.h"
//#include "GameMenu.h"
#include "GameManager.h"

// The CVertex FVF
#define D3DFVF_CVertex D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

// Billboard, number vertices
#define D3DFVF_BILLVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

//extern LPD3DXFONT g_Font;
extern CGameManager g_Game;

// used as return values for functions
#define FN_FAILED 0
#define FN_SUCCESS 1

// Constants to make changing stuff in initialization easier
#define WINDOW_TITLE "3D Minesweeper"
#define WINDOW_CLASS_NAME "3D Minesweeper"

#define INCLUDES

#endif