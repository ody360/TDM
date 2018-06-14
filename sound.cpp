#include "Includes.h"
#include "Sound.h"

CSound::CSound(void)
{
	g_pLoader		= NULL;
	g_pPerformance	= NULL;
	g_pSegment		= NULL;
}

CSound::~CSound(void)
{
}

void CSound::PlaySound()
{

    // Initialize COM
    CoInitialize(NULL);
    
    // Create loader object
    CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicLoader8, (void**)&g_pLoader );

    // Create performance object
    CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicPerformance8, (void**)&g_pPerformance );

    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    g_pPerformance->InitAudio( NULL, NULL, NULL, 
                               DMUS_APATH_DYNAMIC_STEREO, 64,
                               DMUS_AUDIOF_ALL, NULL );

    CHAR strPath[MAX_PATH];
    GetWindowsDirectory( strPath, MAX_PATH );
    strcat( strPath, "\\media" );

    // Tell DirectMusic where the default search path is
    WCHAR wstrSearchPath[MAX_PATH];
    MultiByteToWideChar( CP_ACP, 0, strPath, -1, 
                         wstrSearchPath, MAX_PATH );

	g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               wstrSearchPath, FALSE );
    
    // Load the segment from the file
    WCHAR wstrFileName[MAX_PATH] = L"theSound.wav";   
    if( FAILED( g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8,
                                               wstrFileName,
                                               (LPVOID*) &g_pSegment ) ) )

	 // Download the segment's instruments to the synthesizer
    g_pSegment->Download( g_pPerformance );

    // Tell DirectMusic to repeat this segment forever
    g_pSegment->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

	    // Create an 3D audiopath with a 3d buffer.
    // We can then play all segments into this buffer and directly control its
    // 3D parameters.
    IDirectMusicAudioPath8* p3DAudioPath = NULL;
    g_pPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 
                                             64, TRUE, &p3DAudioPath );

    // Play segment on the 3D audiopath
    g_pPerformance->PlaySegmentEx( g_pSegment, NULL, NULL, 0, 
                                   0, NULL, NULL, p3DAudioPath );
}
void CSound::StopSound()
{
	    // Stop the music
    g_pPerformance->Stop( NULL, NULL, 0, 0 );

    // Cleanup all interfaces
//    pDSB->Release();
//    p3DAudioPath->Release();
    g_pLoader->Release(); 
    g_pSegment->Release();

    // Close down DirectMusic after releasing the DirectSound buffers
    g_pPerformance->CloseDown();
    g_pPerformance->Release();
    
    // Close down COM
    CoUninitialize();
}