#include "Includes.h"

// Our vertex class
class CVertex
{
public:
	// Its coordinates
	float fX, fY, fZ;
	
	// the normal
	float fXnor, fYnor, fZnor;
	// Its colour
	//DWORD dwColor;

	//
    FLOAT  tu, tv; // Vertex texture coordinates


	// A function to simplify initialization
	void Create(float fp_fX,float fp_fY,float fp_fZ, float fp_nX, float fp_nY, float fp_nZ);
	CVertex SetUV( float u, float v);
	void SetNormal( D3DXVECTOR3* pNormal );
	CVertex operator=( CVertex &other );
	operator D3DXVECTOR3 ();
	CVertex Move( float x, float y, float z );
};


// our billboard vertex class
// Custom vertex type for the trees
class BillVertex
{
public:
    // Its coordinates
	float fX, fY, fZ;      // Vertex position
    //DWORD       color;  // Vertex color
    FLOAT       tu, tv; // Vertex texture coordinates

	operator D3DXVECTOR3 () { return D3DXVECTOR3( fX, fY, fZ ); };

	void SetXYZ(float x, float y, float z ) { fX = x; fY = y; fZ = z; };
	void SetUV(float u, float v) { tu = u; tv = v; };
	//void SetColor( int r, int g, int b ) { color=D3DCOLOR_XRGB((BYTE)(r*255), (BYTE)(g*255), (BYTE)(b*255)); };
};
