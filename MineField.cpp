//#include "Includes.h"
#include "MineField.h"
#include "DataPoint.h"


CMineField::CMineField(void)
{
	memset(this,0,sizeof(CMineField));
}

CMineField::~CMineField(void)
{
	// clean out field..
	if( m_pDataField )
	{
		for( int x = 0; x < m_Size.x; x++)
		{
			for( int y = 0; y < m_Size.y; y++)
				delete [] m_pDataField[x][y];

			delete [] m_pDataField[x];
		}
		delete [] m_pDataField;
	}
}

float CMineField::getCameraDistance()
{
	return m_fDistanceToCamera;
}

// way to use this:  make a function with the following prototype as a member of CMineField
// void MyFuncitonName( CDataPoint* dataPoint );
// to use it, simply call ApplyAdjcent( MyFunctionName, placeToStart );
void CMineField::ApplyAdjcent( void (CMineField::*pm_fn)( CDataPoint*, iCoords), iCoords XYZ )
{
	if(XYZ.x > 0 )
		(*this.*pm_fn)( &m_pDataField[XYZ.x-1][XYZ.y][XYZ.z], iCoords(XYZ.x-1,XYZ.y,XYZ.z) ); // apply to right side
	if(XYZ.x < m_Size.x-1 )
		(*this.*pm_fn)( &m_pDataField[XYZ.x+1][XYZ.y][XYZ.z], iCoords(XYZ.x+1,XYZ.y,XYZ.z) ); // apply to left side
	if(XYZ.y > 0 )
		(*this.*pm_fn)( &m_pDataField[XYZ.x][XYZ.y-1][XYZ.z], iCoords(XYZ.x,XYZ.y-1,XYZ.z) ); // apply to bottom
	if(XYZ.y < m_Size.y-1 )
		(*this.*pm_fn)( &m_pDataField[XYZ.x][XYZ.y+1][XYZ.z], iCoords(XYZ.x,XYZ.y+1,XYZ.z) ); // apply to top
	if(XYZ.z > 0 )					
		(*this.*pm_fn)( &m_pDataField[XYZ.x][XYZ.y][XYZ.z-1], iCoords(XYZ.x,XYZ.y,XYZ.z-1) );	// apply to back
	if(XYZ.z < m_Size.z-1 ) 
		(*this.*pm_fn)( &m_pDataField[XYZ.x][XYZ.y][XYZ.z+1], iCoords(XYZ.x,XYZ.y,XYZ.z+1) );	// apply to front
}

BOOL CMineField::isGameWon()
{
	long totalMines = 0;
	long flaggedMines = 0;
	BOOL bGameWon = TRUE;

	for( int x = 0; x < m_Size.x; x++ )
	{
		for( int y = 0; y < m_Size.y; y++ )
		{
			for( int z = 0; z < m_Size.z; z++ )
			{
				if( !m_pDataField[x][y][z].isMine() && !m_pDataField[x][y][z].isUncovered() )
					bGameWon = FALSE;
				if( m_pDataField[x][y][z].isMine() )
					totalMines++;
				if( m_pDataField[x][y][z].isFlagged() )
					flaggedMines++;
			}
		}
	}

	m_MineCount = totalMines-flaggedMines;

	if( bGameWon )
	{
		g_Game.SetState( gameWon );
		m_MineCount = 0;
	}

	return bGameWon;
}

void CMineField::OnMouseMove( int x, int y, WPARAM ButtonState )
{
	if( GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_CONTROL))
	{
	 	FinderIntersect( x, y );
		m_pDataField[m_Current.x][m_Current.y][m_Current.z].setUncovered();
	}
}

void CMineField::AdjustMineCount()
{
	CDataPoint* current;
	int x, y, z;

	//Clears current settings for number of mines adjacent
	for( x = 0; x < m_Size.x; x++ )
	{
		for( y = 0; y < m_Size.y; y++ )
		{
			for( z = 0; z < m_Size.z; z++ )
			{
				while( m_pDataField[x][y][z].getAdj() > 0 )
					m_pDataField[x][y][z].decrAdj();
			}
		}
		}

	//Sets the number of adjacent mines for each cube
	for( x = 0; x < m_Size.x; x++)
	{
		for( y = 0; y < m_Size.y; y++)
		{
			for( z = 0; z < m_Size.z; z++)
			{
				current = &m_pDataField[x][y][z];

				//First check the x-direction
				if( x > 0 )
				{
					if(m_pDataField[x-1][y][z].isMine())
						current->incrAdj();
				}
				if( x < m_Size.x - 1 )
				{
					if(m_pDataField[x+1][y][z].isMine())
						current->incrAdj();
				}
				//Then check the y-direction
				if( y > 0 )
				{
					if(m_pDataField[x][y-1][z].isMine())
						current->incrAdj();
				}
				if( y < m_Size.y - 1 )
				{
					if(m_pDataField[x][y+1][z].isMine())
						current->incrAdj();
				}
				//Finally, check the z-direction
				if( z > 0 )
				{
					if(m_pDataField[x][y][z-1].isMine())
						current->incrAdj();
				}
				if( z < m_Size.z - 1 )
				{
					if(m_pDataField[x][y][z+1].isMine())
						current->incrAdj();
				}
			}//end z
		}//end y
	} //end x
}

void CMineField::OnMouseClick( int x, int y, WPARAM ButtonState, UINT mouseButton )
{
	if( g_Game.getState() == playing )
	{
 		FinderIntersect( x, y );
		if( (MK_SHIFT & ButtonState) || mouseButton == WM_RBUTTONDOWN )
			m_pDataField[m_Current.x][m_Current.y][m_Current.z].ToggleFlagged();

		// "Defuse"  ~Mike 4/9/2003
		else if( (m_pDataField[m_Current.x][m_Current.y][m_Current.z].isFlagged())
			&& (GetAsyncKeyState(VK_CONTROL)) && (mouseButton == WM_LBUTTONDOWN) )
		{
			if( !m_pDataField[m_Current.x][m_Current.y][m_Current.z].isMine() )
				g_Game.SetState( gameOver ); //Can't diffuse a non-bomb
			else
			{
				m_pDataField[m_Current.x][m_Current.y][m_Current.z].unSetMine();
				m_pDataField[m_Current.x][m_Current.y][m_Current.z].setUncovered();

				//fixes the new number scheme
				AdjustMineCount();
				//this next part decrements each datapoint; the above function doesn't
				//  quite allow for the defuse option.  Not quite.
				for( int i = 0; i < m_Size.x; i++ )
					for( int j = 0; j < m_Size.y; j++ )
						for( int k = 0; k < m_Size.z; k++ )
						{
							m_pDataField[i][j][k].decrAdj();
							/*if( (m_pDataField[i][j][k].getAdj() < 0) && (m_pDataField[i][j][k].isUncovered()) )
								ChainClear(i, j, k);*/
						}
			}
		} //End "defuse"

		else if( !m_pDataField[m_Current.x][m_Current.y][m_Current.z].isFlagged()
					&& !GetAsyncKeyState(VK_CONTROL) )
		{
				if( m_pDataField[m_Current.x][m_Current.y][m_Current.z].isMine() )
				{
					m_Explosion.m_Center.x = (float)x;
					m_Explosion.m_Center.y = (float)y;
					g_Game.SetState( gameOver );
				}
				else
				{
                    m_pDataField[m_Current.x][m_Current.y][m_Current.z].setUncovered();

					/*This does "chain clearing" - if the block just cleared has no adjacent
					  mines, then it'll clear all its adjascent squares. ~Mike, March 28th*/
					if( m_pDataField[m_Current.x][m_Current.y][m_Current.z].getAdj() < 0 )
						ChainClear( m_Current.x, m_Current.y, m_Current.z );
				}
		}

		//Sweeps through the minefield and makes sure any open blocks which aren't
		//  adjacent to any mines go through chain clearing
		//  ~Mike 4/9/2003
		for( int i = 0; i < m_Size.x; i++ )
			for( int j = 0; j < m_Size.y; j++ )
				for( int k = 0; k < m_Size.z; k++ )
					if( (m_pDataField[i][j][k].getAdj() < 0) && (m_pDataField[i][j][k].isUncovered()) )
						ChainClear(i, j, k);

		isGameWon();
	}
}

void CMineField::MoveScene()
{
	// We have a static integer for how many degrees to generate the rotation matrix for
	D3DLIGHT8 Light;
	
	D3DXMATRIX matRotationX;
	D3DXMATRIX matRotationY;
	D3DXMATRIX matTrans;
	D3DXMATRIX matRotate;
	D3DXMATRIX matFinal;

	if( GetAsyncKeyState(VK_LEFT) )
		m_iRotationDegreesX++;
	if( GetAsyncKeyState(VK_RIGHT) )
		m_iRotationDegreesX--;
	if( GetAsyncKeyState(VK_UP) )
		m_iRotationDegreesY++;
	if( GetAsyncKeyState(VK_DOWN) )
		m_iRotationDegreesY--;

	// Setup a matrix to rotate iRotationDegrees degrees around the Y axis
	D3DXMatrixRotationY(&matRotationX, D3DXToRadian(m_iRotationDegreesX));
	D3DXMatrixRotationX(&matRotationY, D3DXToRadian(m_iRotationDegreesY));

	// Increment the degree of rotation. 
	if (abs(m_iRotationDegreesY)==360 || g_Game.getState() == inMenu)
		m_iRotationDegreesY=0;
	if (abs(m_iRotationDegreesX)==360 || g_Game.getState() == inMenu)
		m_iRotationDegreesX=0;

	D3DXMatrixTranslation(&matTrans, 0, 0, m_fDistanceToCamera );
	D3DXMatrixMultiply(&matRotate, &matRotationX, &matRotationY);
	D3DXMatrixMultiply(&matFinal, &matRotate, &matTrans);
	
	// following keeps the light position the same as the camera
	if( SUCCEEDED( g_Game.m_pDevice->GetLight( 0, &Light ) ) )
	{
		D3DXMATRIX matInvsFinal;
		float det;

		// turns out the invers of the translation matrix's bottom row
		// is the position of the camera..
		D3DXMatrixInverse( &matInvsFinal, &det, &matFinal );

		Light.Position.x = matInvsFinal._41;
		Light.Position.y = matInvsFinal._42;
		Light.Position.z = matInvsFinal._43;

		g_Game.m_pDevice->SetLight(0, &Light );
	}

	// Set that matrix to the world transformation
	g_Game.m_pDevice->SetTransform(D3DTS_VIEW, &matFinal );

}

void CMineField::Redraw(IDirect3DDevice8 *device)
{
	int size = m_Size.x * m_Size.y * m_Size.z * 36; // 36 verts per cube
	int cubeCount = 0;
	float det;

	//Sweeps through the minefield and makes sure any open blocks which aren't
	//  adjacent to any mines go through chain clearing
	//  ~Mike 4/9/2003
/*	for( int i = 0; i < m_Size.x; i++ )
		for( int j = 0; j < m_Size.y; j++ )
			for( int k = 0; k < m_Size.z; k++ )
				if( (m_pDataField[i][j][k].getAdj() < 0) && (m_pDataField[i][j][k].isUncovered()) )
					ChainClear(i, j, k);
*/
	D3DXMATRIX  matTransNumberWorld;
	D3DXMATRIX  matNumberWorld;
    D3DXMATRIX  matWorld;
    D3DXMATRIX  matInvsView;
    D3DXMATRIX  matXWorld;
    D3DXMATRIX  matYWorld;

	D3DXVECTOR3 vCameraPosition;
	D3DXVECTOR3 vBillPosition;
	std::list<billBoardInfo>  billBoardList;
	billBoardInfo tempBillInfo;

	if( !size ) // we have no minefield!
		return; 

	device->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
    //device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	// Tell it what kind of vertices we're using
	device->SetVertexShader(D3DFVF_CVertex );


	// tell it what texture

	// set the material
	device->SetMaterial(&m_matMaterial);
	device->SetTexture(0, m_CubeTexture );
	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	//device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE  );

	// what vertices to render
	device->SetStreamSource(0,m_pVertexBuffer,sizeof(CVertex));

	for( int x = 0; x < m_Size.x; x++ )
	{
		for( int y = 0; y < m_Size.y; y++ )
		{
			for( int z = 0; z < m_Size.z; z++ )
			{
				if( !m_pDataField[x][y][z].isUncovered() || m_pDataField[x][y][z].isFlagged() )
				{
					if( g_Game.getState() == gameOver || g_Game.getState() == gameWon )
					{
						if( m_pDataField[x][y][z].isMine() )
						{
							if( !m_pDataField[x][y][z].isFlagged() )
								device->SetTexture(0, m_BombTexture );
							else /*Change this next one to "Flagged" texture
								   when we have it.  ~Mike, March 28th*/
								device->SetTexture(0, m_BombTexture );
						}
						else
						{
							if( !m_pDataField[x][y][z].isFlagged() )
								m_pDataField[x][y][z].setUncovered();
							else  /*Make this one the "Flagged incorrectly" texture
								    when we have it.  ~Mike, March 28th*/
								device->SetTexture(0, m_CubeNotBombTexture );
						}
					}
					else if( m_pDataField[x][y][z].isFlagged() )
						device->SetTexture(0, m_CubeBombTexture );
					else
						device->SetTexture(0, m_CubeTexture );

					// do some other checking to see what we need to draw..
					// for example we might only need to draw one side..
					if( (x > 0 && m_pDataField[x-1][y][z].isUncovered()) || x == 0 ) // need to draw right side
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 12+cubeCount, 2);
					
					if( (x < m_Size.x-1 && m_pDataField[x+1][y][z].isUncovered()) || x == m_Size.x-1 ) // need to draw left side
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 6+cubeCount, 2);

					if( (y > 0 && m_pDataField[x][y-1][z].isUncovered()) || y == 0 ) // need to draw bottom
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 24+cubeCount, 2);

					if( (y < m_Size.y-1 && m_pDataField[x][y+1][z].isUncovered()) || m_Size.y-1 == y ) // need to draw top
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 18+cubeCount, 2);

					if( (z > 0 && m_pDataField[x][y][z-1].isUncovered()) || z == 0 ) // need to draw back
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 0+cubeCount, 2);

					if( (z < m_Size.z-1 && m_pDataField[x][y][z+1].isUncovered()) || m_Size.z-1 == z ) // need to draw front
						device->DrawPrimitive(D3DPT_TRIANGLELIST, 30+cubeCount, 2);

				}
				/*else
				{
					device->SetTexture(0, m_ClearCubeTexture );
					device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
					device->DrawPrimitive(D3DPT_TRIANGLELIST, 0+cubeCount, 12); //Top
				}*/
				cubeCount += 36;
			}
		}
	}

	// *****  Draw the numbers ****** //
	device->SetVertexShader(D3DFVF_BILLVERTEX);
	device->SetStreamSource(0,m_pNumberVertexBuffer,sizeof(BillVertex));
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	// Tell it to use our texture
	// kinda works, hides ones behind it //device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1  );

	/* WORKS but is cheating */
	D3DXMatrixRotationY(&matXWorld, D3DXToRadian(m_iRotationDegreesX));
	D3DXMatrixRotationX(&matYWorld, D3DXToRadian(m_iRotationDegreesY));

	D3DXMatrixMultiply(&matWorld, &matXWorld, &matYWorld);
	D3DXMatrixInverse( &matInvsView, &det, &matWorld );


	/* Below, we cycle throught the minefield checking each cube
	if the cube is uncovered and has mines adjacent to it we go ahead
	and move the billboard into place ( by resetting the world translation ),
	set the correct texture and then draw the billboard..*/
	D3DVIEWPORT8 viewPort;
	device->GetViewport( &viewPort );

	GetRayFromMouse( viewPort.Width/2, viewPort.Height/2, D3DXVECTOR3(), vCameraPosition);

	for( int x = 0; x < m_Size.x; x++ )
	{
		for( int y = 0; y < m_Size.y; y++ )
		{
			for( int z = 0; z < m_Size.z; z++ )
			{
				if( m_pDataField[x][y][z].isUncovered() && m_pDataField[x][y][z].getAdj() > -1
					&& m_pDataField[x][y][z].getAdj() < 5 )
				{
					// checks to see if all of the surrounding cubes are uncovered..
					if(!(((x > 0 && m_pDataField[x-1][y][z].isUncovered()) || x == 0 ) &&
						 ((x < m_Size.x-1 && m_pDataField[x+1][y][z].isUncovered()) || x == m_Size.x-1 ) && // need to draw left side
						 ((y > 0 && m_pDataField[x][y-1][z].isUncovered()) || y == 0 ) &&// need to draw bottom
						 ((y < m_Size.y-1 && m_pDataField[x][y+1][z].isUncovered()) || m_Size.y-1 == y ) && // need to draw top
						 ((z > 0 && m_pDataField[x][y][z-1].isUncovered()) || z == 0 ) && // need to draw back
						 ((z < m_Size.z-1 && m_pDataField[x][y][z+1].isUncovered()) || m_Size.z-1 == z ) )) // need to draw front
					{
						tempBillInfo.cube.set( x, y, z );
						vBillPosition.x = x*m_CubeSize.fWidth*2-m_Size.x*m_CubeSize.fWidth;
						vBillPosition.y = y*m_CubeSize.fHeight*2-m_Size.y*m_CubeSize.fHeight;
						vBillPosition.z = z*m_CubeSize.fDepth*2-m_Size.z*m_CubeSize.fDepth;

						tempBillInfo.fDistance = (float)sqrt(  (vCameraPosition.z-vBillPosition.z)*(vCameraPosition.z-vBillPosition.z) +
														(vCameraPosition.x-vBillPosition.x)*(vCameraPosition.x-vBillPosition.x) +
														(vCameraPosition.y-vBillPosition.y)*(vCameraPosition.y-vBillPosition.y) );

						billBoardList.push_back( tempBillInfo );
					}
				}
			}
		}
	}

	billBoardList.sort();

	while( billBoardList.size() )
	{
		tempBillInfo = billBoardList.front();

		if( m_pDataField[tempBillInfo.cube.x][tempBillInfo.cube.y][tempBillInfo.cube.z].getAdj() < 0 )
			tempBillInfo = billBoardList.front();
		// case when game is over, all numbers are drawn including 6's, which we dont have a texutre for,
		// this was causing an overrun of our texture array, which has only textures for 1-5, not six,
		// you will never see six ---> so dont even bother drawing it..
		device->SetTexture(0, m_NumberTextures[m_pDataField[tempBillInfo.cube.x][tempBillInfo.cube.y][tempBillInfo.cube.z].getAdj()]);

		D3DXMatrixTranslation(&matTransNumberWorld, 
								(tempBillInfo.cube.x*m_CubeSize.fWidth*2-m_Size.x*m_CubeSize.fWidth),
								(tempBillInfo.cube.y*m_CubeSize.fHeight*2-m_Size.y*m_CubeSize.fHeight),
								(tempBillInfo.cube.z*m_CubeSize.fDepth*2-m_Size.z*m_CubeSize.fDepth) );

		D3DXMatrixMultiply(&matNumberWorld, &matInvsView, &matTransNumberWorld);

		device->SetTransform( D3DTS_WORLD, &matNumberWorld );

		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); //draw bill..

		billBoardList.pop_front();
	}

    // Restore device state
    D3DXMatrixIdentity( &matWorld );
    device->SetTransform( D3DTS_WORLD, &matWorld );

	// Static text is added here ///
	RECT FontPosition = {0,50,800,600}; //postion of text on screen
    char bufferFirst[255];
		
	sprintf(bufferFirst, "Size of Cube:  %i, Number of mines left:%i, GameState: %i, Time: %.2f",
		(m_Size.x * m_Size.y * m_Size.z), m_MineCount, g_Game.getState(), g_Game.GetGameTime());
	
	m_Font->DrawText(bufferFirst, -1, &FontPosition, DT_CENTER, 0xffff0000);

	// draw the explosions if the game is over..
	if( g_Game.getState() == gameOver && m_Explosion.m_Step < 8)
	{
			D3DXVECTOR2 pos;
			D3DXVECTOR2 scale;
			RECT imageRect;

			imageRect.left = 128*m_Explosion.m_Step;
			imageRect.right = 128*(m_Explosion.m_Step+1);
			imageRect.top = 0;
			imageRect.bottom = 128;

			//						(m_Current.x*m_CubeSize.fWidth*2-m_Size.x*m_CubeSize.fWidth),
			//						(m_Current.y*m_CubeSize.fHeight*2-m_Size.y*m_CubeSize.fHeight),
			//						(m_Current.z*m_CubeSize.fDepth*2-m_Size.z*m_CubeSize.fDepth) );

			pos.x = m_Explosion.m_Center.x-128.0f*2.5f;
			pos.y = m_Explosion.m_Center.y-128.0f*2.5f;

			scale.x = 5;
			scale.y = 5;

			HRESULT hr = m_Explosion.m_ExplosionSprite->Draw( m_Explosion.m_ExplosionTexture,
															&imageRect,
															&scale, 0, 0, &pos,  D3DCOLOR_XRGB(255,255,255) );

			if( GetTickCount() - m_Explosion.m_Time > 100 ) // used to keep it from drawing too fast
			{
				m_Explosion.m_Step++;
				m_Explosion.m_Time = GetTickCount();
			}
	}

	// release texture & vertex buffer
	device->SetStreamSource(0,0,0);
	device->SetTexture(0, NULL);

    // Restore device state
    D3DXMatrixIdentity( &matWorld );
    device->SetTransform( D3DTS_WORLD, &matWorld );

}

// x and y are the mouse coordinates in the client window
void CMineField::GetRayFromMouse(int x, int y, D3DXVECTOR3 &vPickRayDir, D3DXVECTOR3 &vPickRayOrig)
{
	D3DXMATRIX matProj;

	g_Game.m_pDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	POINT ptCursor = { x, y };

	D3DVIEWPORT8 viewPort;
	D3DXVECTOR3 v;
	UINT width, height;

	g_Game.m_pDevice->GetViewport( &viewPort );

	width = viewPort.Width;
	height = viewPort.Height;

	v.x =  ( ( ( 2.0f * ptCursor.x ) / width  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / height ) - 1 ) / matProj._22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIX matView, m;
	g_Game.m_pDevice->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixInverse( &m, NULL, &matView );

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
}

iCoords CMineField::FinderIntersect( int x, int y)
{
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vert1;
	D3DXVECTOR3 vert2;
	D3DXVECTOR3 vert3;

	CVertex* pVerts = 0;
	float textCordsx, textCordsy;
	float closest = 1000000;
	float testClosest = 0;
	int m_NumVerts = m_Size.x * m_Size.y * m_Size.z * 36; // 36 verts per cube

	// get the ray from the mouse
	GetRayFromMouse( x, y, vPickRayDir, vPickRayOrig );

	// get actual buffer..
	m_pVertexBuffer->Lock( 0, m_NumVerts*sizeof(CVertex), (BYTE**)&pVerts, 0 );

	for( int x = 0; x < m_Size.x; x++ )
	{
		for( int y = 0; y < m_Size.y; y++ )
		{
			for( int z = 0; z < m_Size.z; z++ )
			{
				if( !m_pDataField[x][y][z].isUncovered() )
				{
					for( int vertex = 0; vertex < 36; vertex += 3 )
					{
						vert1 = (D3DXVECTOR3)pVerts[vertex];
						vert2 = (D3DXVECTOR3)pVerts[vertex+1];
						vert3 = (D3DXVECTOR3)pVerts[vertex+2];

						if( D3DXIntersectTri( &vert1, &vert2, &vert3,
												&vPickRayOrig, &vPickRayDir,
												&textCordsx, &textCordsy, &testClosest ) )
						{
							if( testClosest < closest )
							{
								m_Current.x = x;
								m_Current.y = y;
								m_Current.z = z;

								closest = testClosest;
							}
						}

					}

				}
				pVerts += 36; // next cube..
			}
		}
	}

	m_pVertexBuffer->Unlock();
	
	return m_Current;
}


void CMineField::ChainClear(int x, int y, int z)
{
	m_pDataField[x][y][z].decrAdj();

	if( x > 0 && !m_pDataField[x-1][y][z].isFlagged() )
	{
		m_pDataField[x-1][y][z].setUncovered();
		if( m_pDataField[x-1][y][z].getAdj() == -1 )
			ChainClear(x-1, y, z);
	}
	if( x < (m_Size.x-1) && !m_pDataField[x+1][y][z].isFlagged() )
	{
		m_pDataField[x+1][y][z].setUncovered();
		if( m_pDataField[x+1][y][z].getAdj() == -1 )
			ChainClear(x+1, y, z);
	}
	if( y > 0 && !m_pDataField[x][y-1][z].isFlagged() )
	{
		m_pDataField[x][y-1][z].setUncovered();
		if( m_pDataField[x][y-1][z].getAdj() == -1 )
			ChainClear(x, y-1, z);
	}
	if( y < (m_Size.y-1) && !m_pDataField[x][y+1][z].isFlagged() )
	{
		m_pDataField[x][y+1][z].setUncovered();
		if( m_pDataField[x][y+1][z].getAdj() == -1 )
			ChainClear(x, y+1, z);
	}
	if( z > 0 && !m_pDataField[x][y][z-1].isFlagged() )
	{
		m_pDataField[x][y][z-1].setUncovered();
		if( m_pDataField[x][y][z-1].getAdj() == -1 )
			ChainClear(x, y, z-1);
	}
	if( z < (m_Size.z-1) && !m_pDataField[x][y][z+1].isFlagged() )
	{
		m_pDataField[x][y][z+1].setUncovered();
		if( m_pDataField[x][y][z+1].getAdj() == -1 )
			ChainClear(x, y, z+1);
	}
	m_pDataField[x][y][z].incrAdj();
}

void CMineField::NewGame(IDirect3DDevice8* device)
{
 	NewGame( device, m_Size, m_MineCount );
}

BOOL CMineField::NewGame( IDirect3DDevice8 * device, iCoords cords, int numMines )
{
	CDataPoint* current;
	int x, y, z;
	BOOL sameSize = FALSE;

	// clean out old field..
	if( m_pDataField )
	{
		for( x = 0; x < m_Size.x; x++)
		{
			for( y = 0; y < m_Size.y; y++)
				delete [] m_pDataField[x][y];

			delete [] m_pDataField[x];
		}
		delete [] m_pDataField;
	}
	
	if( m_Size.x != cords.x || m_Size.y != cords.y || m_Size.z != cords.z )
	{
		m_Size.x = cords.x;
		m_Size.y = cords.y;
		m_Size.z = cords.z;
	}
	else
		sameSize = TRUE;

	// create array of pointers to pointers to int of size NumPlanes 
	m_pDataField = new CDataPoint** [m_Size.x]; 
	// for each plane create array of pointers to int of size NumRow 
	for ( x = 0; x < m_Size.x; x++ ) 
	{ 
		m_pDataField[x]=new CDataPoint* [m_Size.y]; 
		// for each row create array of int's of size NumCol 
		
		for ( y = 0; y < m_Size.y; y++ ) 
		{ 
			m_pDataField[x][y] = new CDataPoint[m_Size.z]; 
		} 
	} 


	m_MineCount = numMines;

	if (!m_pDataField) //If there was a problem with memory allocation
		return FALSE;
	else if (numMines > cords.x * cords.y * cords.z) //If there are supposedly more mines than cubes
		return FALSE;
	//Otherwise, randomly spread the mines through the field
	else
	{
		// seed the number generator
		srand((unsigned int)GetTickCount());

		// place the mines
		for(int i = 0; i < numMines;)
		{
			x = rand() % m_Size.x;
			y = rand() % m_Size.y;
			z = rand() % m_Size.z;

			if (!(m_pDataField[x][y][z].isMine()))
			{
				m_pDataField[x][y][z].setMine();
				i++;
			}
		}


		//Finally, set the number of adjacent mines for each cube
		AdjustMineCount();

		/****  Dead code.  Erase if the above line works.  ****
		for( x = 0; x < m_Size.x; x++)
		{
			for( y = 0; y < m_Size.y; y++)
			{
				for( z = 0; z < m_Size.z; z++)
				{
					current = &m_pDataField[x][y][z];

					//First check the x-direction
					if( x > 0 )
					{
						if(m_pDataField[x-1][y][z].isMine())
							current->incrAdj();
					}
					if( x < m_Size.x - 1 )
					{
						if(m_pDataField[x+1][y][z].isMine())
							current->incrAdj();
					}
					//Then check the y-direction
					if( y > 0 )
					{
						if(m_pDataField[x][y-1][z].isMine())
							current->incrAdj();
					}
					if( y < m_Size.y - 1 )
					{
						if(m_pDataField[x][y+1][z].isMine())
							current->incrAdj();
					}
					//Finally, check the z-direction
					if( z > 0 )
					{
						if(m_pDataField[x][y][z-1].isMine())
							current->incrAdj();
					}
					if( z < m_Size.z - 1 )
					{
						if(m_pDataField[x][y][z+1].isMine())
							current->incrAdj();
					}
				}
			}
		} //end triple-FOR loop*/

		if( !sameSize || !m_pVertexBuffer )
		{
			Release();
			if( !RebuildVertexBuffer( device ))
				return FALSE;
		}

		return TRUE;
	}
}

BOOL CMineField::RebuildVertexBuffer( IDirect3DDevice8 * device)
{
	HRESULT g_hr;
	D3DXVECTOR3 vNormal;

	m_CubeSize.fWidth = 1.0f;
	m_CubeSize.fHeight = 1.0f;
	m_CubeSize.fDepth = 1.0f;

	CVertex corners[8];

	int NumVerts = m_Size.x * m_Size.y * m_Size.z * 36; // 36 verts per cube

	if( !NumVerts )
	{
		NewGame( device, iCoords(3,3,3), 7 );
		NumVerts = m_Size.x * m_Size.y * m_Size.z * 36;
	}

	CVertex* pVerts = 0;
	BillVertex* pBillVerts = 0;

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

	// be sure this hasnt already been done!!
	if( m_CubeTexture != 0 )
		Release();

	// now load cube texture
    if( FAILED( D3DXCreateTextureFromFile( device, "ClearCube.png", &m_CubeTexture ) ) )
       return FALSE;
	// load bomb texture
	if( FAILED( D3DXCreateTextureFromFile( device, "FlagCube.png", &m_CubeBombTexture ) ) )
		return FALSE;
	if( FAILED( D3DXCreateTextureFromFile( device, "BombCube1.png", &m_BombTexture ) ) )
       return FALSE;
	if( FAILED( D3DXCreateTextureFromFile( device, "NotBomb1.png", &m_CubeNotBombTexture ) ) )
		return FALSE;
	// now load cube texture
    if( FAILED( D3DXCreateTextureFromFile( device, "number1.tga", &m_NumberTextures[0] ) ) )
       return FALSE;
    if( FAILED( D3DXCreateTextureFromFile( device, "number2.tga", &m_NumberTextures[1] ) ) )
       return FALSE;
    if( FAILED( D3DXCreateTextureFromFile( device, "number3.tga", &m_NumberTextures[2] ) ) )
       return FALSE;
    if( FAILED( D3DXCreateTextureFromFile( device, "number4.tga", &m_NumberTextures[3] ) ) )
       return FALSE;
    if( FAILED( D3DXCreateTextureFromFile( device, "number5.tga", &m_NumberTextures[4] ) ) )
       return FALSE;

	// setup the explosion sprite
	if( FAILED( D3DXCreateSprite( device, &m_Explosion.m_ExplosionSprite) ) )
		return FALSE;
	if( FAILED( D3DXCreateTextureFromFile( device, "explosion.tga", &m_Explosion.m_ExplosionTexture) ) )
		return FALSE;

	m_Explosion.m_Step = 0;

    // adjust camera distance from cube dynamically per cubesize..
	m_fDistanceToCamera = m_Size.x*m_CubeSize.fWidth*7;

	if( m_Size.y*m_CubeSize.fHeight*7 > m_fDistanceToCamera )
		m_fDistanceToCamera = m_Size.y*m_CubeSize.fHeight*7;
	if( m_Size.z*m_CubeSize.fDepth*7 > m_fDistanceToCamera )
		m_fDistanceToCamera = m_Size.z*m_CubeSize.fDepth*7;


	//create font  // COMMENTED FOR PLUGGING MEMORY LEAKING PURPOSES
	LOGFONT LogFont = {24,0,0,0,FW_NORMAL,false,false,false,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Arial"};
	
	if(FAILED(D3DXCreateFontIndirect(device,&LogFont,&m_Font)))		
	{
		//MessageBox(g_hWnd,"D3DXCreateFontIndirect() failed!","WinMain()",MB_OK);
		return false;
	}
		
	// ****** first build the numberfield ******** //

	// create the vertex buffer..
	g_hr = device->CreateVertexBuffer( 4*sizeof(BillVertex),
										D3DUSAGE_POINTS,
										D3DFVF_BILLVERTEX,
										D3DPOOL_DEFAULT,
										&m_pNumberVertexBuffer );

	if( FAILED(g_hr))
		return FN_FAILED;

	g_hr = m_pNumberVertexBuffer->Lock( 0, 4*sizeof(BillVertex), (BYTE**)&pBillVerts, 0 );

	if( FAILED(g_hr))
		return FN_FAILED;

	pBillVerts[0].SetXYZ( m_CubeSize.fWidth/-2, m_CubeSize.fHeight/-2, 0 );
	pBillVerts[1].SetXYZ( m_CubeSize.fWidth/-2, m_CubeSize.fHeight/2, 0 );
	pBillVerts[2].SetXYZ( m_CubeSize.fWidth/2, m_CubeSize.fHeight/-2, 0 );
	pBillVerts[3].SetXYZ( m_CubeSize.fWidth/2, m_CubeSize.fHeight/2, 0 );

	pBillVerts[0].SetUV( 0, 1 );        
	pBillVerts[1].SetUV( 0, 0 );
	pBillVerts[2].SetUV( 1, 1 );
	pBillVerts[3].SetUV( 1, 0 );

	m_pNumberVertexBuffer->Unlock();

	// ****** done building the numberfield ******** //

	// ****** Now build the minefield ******** //

	// create the vertex buffer..
	g_hr = device->CreateVertexBuffer( NumVerts*sizeof(CVertex),
										D3DUSAGE_POINTS,
										D3DFVF_CVertex,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer );

	if( FAILED(g_hr))
		return FN_FAILED;

	g_hr = m_pVertexBuffer->Lock( 0, NumVerts*sizeof(CVertex), (BYTE**)&pVerts, 0 );

	if( FAILED(g_hr))
		return FN_FAILED;

	corners[0].Create( -m_CubeSize.fWidth,  m_CubeSize.fHeight, -m_CubeSize.fDepth, -1.0f,  1.0f, -1.0f);    // Back Top Left Vertex
	corners[1].Create(  m_CubeSize.fWidth,  m_CubeSize.fHeight, -m_CubeSize.fDepth,  1.0f,  1.0f, -1.0f);    // Back Top Right Vertex
	corners[2].Create( -m_CubeSize.fWidth, -m_CubeSize.fHeight, -m_CubeSize.fDepth, -1.0f, -1.0f, -1.0f);    // Back Bottom Left Vertex
	corners[3].Create(  m_CubeSize.fWidth, -m_CubeSize.fHeight, -m_CubeSize.fDepth,  1.0f, -1.0f, -1.0f);    // Back Bottom Right Vertex
	corners[4].Create( -m_CubeSize.fWidth,  m_CubeSize.fHeight,  m_CubeSize.fDepth, -1.0f,  1.0f,  1.0f);    // Front Top Left Vertex
	corners[5].Create(  m_CubeSize.fWidth,  m_CubeSize.fHeight,  m_CubeSize.fDepth,  1.0f,  1.0f,  1.0f);    // Front Top Right Vertex
	corners[6].Create( -m_CubeSize.fWidth, -m_CubeSize.fHeight,  m_CubeSize.fDepth, -1.0f, -1.0f,  1.0f);    // Front Bottom Left Vertex
	corners[7].Create(  m_CubeSize.fWidth, -m_CubeSize.fHeight,  m_CubeSize.fDepth,  1.0f, -1.0f,  1.0f);    // Front Bottom Right Vertex

	//D3DPT_TRIANGLELIST
	for( int xcount = 0; xcount < m_Size.x; xcount++ )
	{
		for( int ycount = 0; ycount < m_Size.y; ycount++ )
		{
			for( int zcount = 0; zcount < m_Size.z; zcount++ )
			{
				pVerts[0]  = corners[1].SetUV( 1,0);
				pVerts[1]  = corners[3].SetUV( 1,1);
				pVerts[2]  = corners[2].SetUV( 0,1); // Back a --->  I think this is the front
				pVerts[3]  = corners[1].SetUV( 1,0);
				pVerts[4]  = corners[2].SetUV( 0,1);
				pVerts[5]  = corners[0].SetUV( 0,0); // Back b --->  I think this is the front
				pVerts[6]  = corners[5].SetUV( 1,0);
				pVerts[7]  = corners[7].SetUV( 1,1);
				pVerts[8]  = corners[3].SetUV( 0,1); // Right a
				pVerts[9]  = corners[5].SetUV( 1,0);
				pVerts[10]  = corners[3].SetUV( 0,1);
				pVerts[11]  = corners[1].SetUV( 0,0); // Right b
				pVerts[12]  = corners[0].SetUV( 1,0);
				pVerts[13]  = corners[2].SetUV( 1,1);
				pVerts[14]  = corners[6].SetUV( 0,1); // Left a
				pVerts[15]  = corners[0].SetUV( 1,0);
				pVerts[16]  = corners[6].SetUV( 0,1);
				pVerts[17]  = corners[4].SetUV( 0,0); // Left b
				
				//For normal top
					pVerts[18]  = corners[0].SetUV( 0,1);
					pVerts[19]  = corners[4].SetUV( 0,0);
					pVerts[20]  = corners[5].SetUV( 1,0); // Top a
					pVerts[21]  = corners[0].SetUV( 0,1);
					pVerts[22]  = corners[5].SetUV( 1,0);
					pVerts[23]  = corners[1].SetUV( 1,1); // Top b

				//Experimented with diaganol top
				// Uncomment to see diaganol top 1
				/*	pVerts[18]  = corners[0].SetUV( 0,.5);
					pVerts[19]  = corners[4].SetUV( .5,0);
					pVerts[20]  = corners[5].SetUV( 1,.5); // Top a
					pVerts[21]  = corners[0].SetUV( 0,.5);
					pVerts[22]  = corners[5].SetUV( 1,.5);
					pVerts[23]  = corners[1].SetUV( .5,1); // Top b*/
				pVerts[24]  = corners[6].SetUV( 0,1);
				pVerts[25]  = corners[2].SetUV( 0,0);
				pVerts[26]  = corners[3].SetUV( 1,0); // Bottom a
				pVerts[27]  = corners[6].SetUV( 0,1);
				pVerts[28]  = corners[3].SetUV( 1,0);
				pVerts[29]  = corners[7].SetUV( 1,1); // Bottom b
				pVerts[30]  = corners[4].SetUV( 1,0);
				pVerts[31]  = corners[6].SetUV( 1,1);
				pVerts[32]  = corners[7].SetUV( 0,1); // Front a --->  I think this is the back
				pVerts[33]  = corners[4].SetUV( 1,0);
				pVerts[34]  = corners[7].SetUV( 0,1);
				pVerts[35]  = corners[5].SetUV( 0,0); // Front b --->  I think this is the back

				//This part adjusted to shift minefield to world center.  ~Mike (March 8th)
				for( int i = 0; i < 36; i++)
					pVerts[i].Move( xcount*m_CubeSize.fWidth*2-m_Size.x*m_CubeSize.fWidth,
									ycount*m_CubeSize.fHeight*2-m_Size.y*m_CubeSize.fHeight,
									zcount*m_CubeSize.fDepth*2-m_Size.z*m_CubeSize.fDepth );

				// recalculating the normals to correct values..
				for( int j = 0; j < 36; j += 6 )
				{
					vNormal = GetTriangleNormal( &(D3DXVECTOR3)pVerts[j],
												 &(D3DXVECTOR3)pVerts[j+1],
												 &(D3DXVECTOR3)pVerts[j+2] );
					pVerts[j].SetNormal( &vNormal );
					pVerts[j+1].SetNormal( &vNormal );
					pVerts[j+2].SetNormal( &vNormal );
					pVerts[j+3].SetNormal( &vNormal );
					pVerts[j+4].SetNormal( &vNormal );
					pVerts[j+5].SetNormal( &vNormal );
				}

				pVerts = pVerts+36;
			}
		}
	}


	m_pVertexBuffer->Unlock();

	return TRUE;
}

void CMineField::Release()
{
	while( m_pVertexBuffer && m_pVertexBuffer->Release() );
	while( m_pNumberVertexBuffer && m_pNumberVertexBuffer->Release() );
	while( m_CubeTexture && m_CubeTexture->Release() );
	while( m_CubeBombTexture && m_CubeBombTexture->Release() );
	while( m_BombTexture && m_BombTexture->Release() );
	while( m_CubeNotBombTexture && m_CubeNotBombTexture->Release() );
	while( m_Explosion.m_ExplosionTexture && m_Explosion.m_ExplosionTexture->Release() );
    while( m_Explosion.m_ExplosionSprite && m_Explosion.m_ExplosionSprite->Release() );
	while( m_Font && m_Font->Release() );

    m_Font = 0;
	m_pVertexBuffer = 0;
	m_pNumberVertexBuffer = 0;
	m_CubeTexture = 0;
	m_CubeBombTexture = 0;
	m_BombTexture = 0;
	m_CubeNotBombTexture = 0;
	m_Explosion.m_ExplosionTexture = 0;
    m_Explosion.m_ExplosionSprite = 0;

	for( int i = 0; i < 5; i++ )
	{
		while( m_NumberTextures[i] && m_NumberTextures[i]->Release() );
		m_NumberTextures[i] = 0;
	}
}

D3DVECTOR CMineField::GetTriangleNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3)
{
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;

    D3DXVec3Subtract(&v1, vVertex2, vVertex1);
    D3DXVec3Subtract(&v2, vVertex3, vVertex1);

    D3DXVec3Cross(&vNormal, &v1, &v2);

    D3DXVec3Normalize(&vNormal, &vNormal);

    return vNormal;
}