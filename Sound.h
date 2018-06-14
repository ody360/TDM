#pragma once
#include "Includes.h"

class CSound
{
public:
	CSound(void);
	~CSound(void);
	void PlaySound();
	void StopSound();

private:
	IDirectMusicLoader8*      g_pLoader;//         = NULL;
IDirectMusicPerformance8* g_pPerformance;//    = NULL;
IDirectMusicSegment8*    g_pSegment;//        = NULL;



};
