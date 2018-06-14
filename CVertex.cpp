#include "Includes.h"


void CVertex::Create(float fp_fX,float fp_fY,float fp_fZ, float fp_nX, float fp_nY, float fp_nZ)
{
	fX=fp_fX;
	fY=fp_fY;
	fZ=fp_fZ;
	fXnor = fp_fX;
	fYnor = fp_fY;
	fZnor = fp_fZ;
	//dwColor=D3DCOLOR_XRGB((BYTE)(fp_fR*255), (BYTE)(fp_fG*255), (BYTE)(fp_fB*255));
}

CVertex CVertex::operator =( CVertex &other )
{
	fX = other.fX;
	fY = other.fY;
	fZ = other.fZ;

	fXnor = other.fXnor;
	fYnor = other.fYnor;
	fZnor = other.fZnor;

	tu = other.tu;
	tv = other.tv;
	//dwColor = other.dwColor;

	return (*this);
}

CVertex::operator D3DXVECTOR3()
{
	return D3DXVECTOR3( fX, fY, fZ );
}

CVertex CVertex::Move( float x, float y, float z )
{
	fX += x;
	fY += y;
	fZ += z;

	return (*this);
}

CVertex CVertex::SetUV( float u, float v)
{
	tu = u;
	tv = v;

	return (*this);
}

void CVertex::SetNormal( D3DXVECTOR3* pNormal )
{
	fXnor = pNormal->x;
	fYnor = pNormal->y;
	fZnor = pNormal->z;
}
