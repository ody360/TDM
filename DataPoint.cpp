#include "DataPoint.h"

CDataPoint::CDataPoint(void)
{
	status = covered;
	mine = FALSE;
	numMineAdj = -1;
	
}

CDataPoint::~CDataPoint(void)
{
}

BOOL CDataPoint::isUncovered() {return status == uncovered;}
void CDataPoint::setUncovered() { status = uncovered; }
void CDataPoint::ToggleFlagged()
{
	if( status != uncovered )
	{
		if( status == marked )
			status = covered;
		else
			status = marked;
	}
}
BOOL CDataPoint::isMine() {return mine;}
BOOL CDataPoint::isFlagged() { return status == marked; };
int CDataPoint::getAdj() {return numMineAdj;}

void CDataPoint::setMine() {mine = TRUE;}
void CDataPoint::unSetMine() {mine = FALSE;} //~Mike, 9th of April 2003
void CDataPoint::incrAdj() { numMineAdj++; }
void CDataPoint::decrAdj() { numMineAdj--; }
