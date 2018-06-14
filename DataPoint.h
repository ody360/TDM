#pragma once
#include "Includes.h"

enum CubeState { covered = 0, uncovered = 1, marked = 2 };

class CDataPoint
{
public:
	CDataPoint(void);
	~CDataPoint(void);
	BOOL isUncovered();
	void setUncovered();
	void ToggleFlagged();
	BOOL isMine();
	BOOL isFlagged();
	int getAdj();

	void incrAdj();
	void decrAdj();
	void setMine();
	void unSetMine(); //Added by Mike Smith, 9th of April 2003
	
private:
	CubeState status;
	BOOL mine;
	int numMineAdj;

};
