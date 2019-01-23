
// system headers
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/file.h>

// main header
#include "sound_player.h"

static volatile int     waveFreeBlockCount; 
static wxCriticalSection waveCriticalSection;  

////////////////////////////////////////////////////////////////////
static void CALLBACK waveOutProc( HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance,
									DWORD dwParam1, DWORD dwParam2 )   
{   
    int* freeBlockCounter = (int*) dwInstance;   

    // ignore calls that occur due to openining and closing the device.  
    if( uMsg != WOM_DONE ) return;   
   
	waveCriticalSection.Enter();   
    (*freeBlockCounter)++;   
	waveCriticalSection.Leave();   
}  

////////////////////////////////////////////////////////////////////
CSoundPlayer::CSoundPlayer( wxString strSoundFile, int iHertz ) : wxThread( wxTHREAD_DETACHED )
{
	m_strSoundFile =  strSoundFile;

	waveBlocks = NULL;
	hWaveOut = NULL;
	hFile = NULL;

	memset(&m_WaveFormatEx,0x00,sizeof(m_WaveFormatEx));
	m_WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormatEx.nChannels = 1;
	m_WaveFormatEx.wBitsPerSample = 8;
	m_WaveFormatEx.cbSize = 0;
	m_WaveFormatEx.nSamplesPerSec = iHertz; // 20050;
	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec*(m_WaveFormatEx.wBitsPerSample/8);
	m_WaveFormatEx.nBlockAlign = (m_WaveFormatEx.wBitsPerSample/8)*m_WaveFormatEx.nChannels;

	m_bIsPlaying = 0;

	InitThread( );
}

////////////////////////////////////////////////////////////////////
CSoundPlayer::~CSoundPlayer( )
{
//	if( m_SoundBuffer ) delete m_SoundBuffer;
	CloseAudio( );
}

////////////////////////////////////////////////////////////////////
void CSoundPlayer::OnExit( )
{
}

////////////////////////////////////////////////////////////////////
int CSoundPlayer::InitThread( )
{	
	// create thread
	if( Create() != wxTHREAD_NO_ERROR )
		return( 0 );
	else
		return( 1 );
}

////////////////////////////////////////////////////////////////////
int CSoundPlayer::Play( )
{
	m_bIsPlaying = 1;

	// start the thread
	if( Run() != wxTHREAD_NO_ERROR )
		return( 0 );
	else
		return( 1 );
}

////////////////////////////////////////////////////////////////////
void *CSoundPlayer::Entry( )
{
	// intermediate buffer for reading 
    char buffer[1024];  

    //initialise the module variables  
    waveBlocks         = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);   
    waveFreeBlockCount = BLOCK_COUNT;   
    waveCurrentBlock   = 0;   

	// first we set format by file
	SetFormatByFile( m_strSoundFile );
  
	// try and open the file  
    if( (hFile = CreateFile( m_strSoundFile, GENERIC_READ,   
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL )) == INVALID_HANDLE_VALUE )
	{   
        //fprintf(stderr, "%s: unable to open file '%s'\n", argv[0], argv[1]);   
        //ExitProcess(1);   
    }   

	// open audio out device
   if( waveOutOpen( &hWaveOut, WAVE_MAPPER, &m_WaveFormatEx, (DWORD_PTR)waveOutProc,
        (DWORD_PTR)&waveFreeBlockCount, CALLBACK_FUNCTION ) != MMSYSERR_NOERROR )
   {   
        //fprintf(stderr, "%s: unable to open wave mapper device\n", argv[0]);   
        //ExitProcess(1);   
    }   

	/////////////////////////
	// MAIN LOOP
	while( m_bContinuePlaying )
	{
		if( TestDestroy( ) == 1 ) break;

        DWORD readBytes;   
   
        if(!ReadFile(hFile, buffer, sizeof(buffer), &readBytes, NULL))   
            break;   
        if(readBytes == 0)   
            break;   
           
        if(readBytes < sizeof(buffer)) 
		{   
            //printf("at end of buffer\n");   
            memset(buffer + readBytes, 0, sizeof(buffer) - readBytes);   
            //printf("after memcpy\n");   
   
        }   
        
		// send audio to the device
        writeAudio(hWaveOut, buffer, sizeof(buffer));   
    }   
   
    // wait for all blocks to complete  
    while(waveFreeBlockCount < BLOCK_COUNT && m_bContinuePlaying)
	{
		if( TestDestroy( ) == 1 ) break;
		Sleep(10);   
	}   
       
    // close audio device   
	if( m_bContinuePlaying ) CloseAudio( );

	// reset flags
	m_bIsPlaying = 0;

	// wait to be stopped
	while( 1 )
	{
		if( TestDestroy( ) == 1 ) break;
		wxThread::Sleep(20);
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
bool CSoundPlayer::SetFormatByFile( wxString file )
{
	#pragma pack(push, 1)
	struct FileHeader
	{
		char cFlagFiff[4];
		unsigned __int32 iFileLen;
		char cFlagWave[4];
		char cFlagFmt[4];
		char cResove[4];
		unsigned __int16 cFormat;
		unsigned __int16 cChannel;
		unsigned __int32 cSample;
		unsigned __int32 cBytePerSec;
		unsigned __int16 cByteprocess;
		unsigned __int16 cBit;
		char cFlagDat[4];
		unsigned __int32 iLenData;
	};
	#pragma pack(pop)

	wxFile fi;
	if( !fi.Open(file) ) return( FALSE );
	
	struct FileHeader head;
	fi.Read ((void *)&head,sizeof (head));
	fi.Close ();

	m_WaveFormatEx.nChannels = head.cChannel;
	m_WaveFormatEx.nSamplesPerSec = head.cSample;
	m_WaveFormatEx.wBitsPerSample = head.cBit;

	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec*(m_WaveFormatEx.wBitsPerSample/8);
	m_WaveFormatEx.nBlockAlign = (m_WaveFormatEx.wBitsPerSample/8)*m_WaveFormatEx.nChannels;


	return( TRUE );
}

////////////////////////////////////////////////////////////////////
// SOUND PLAY METHODS
////////////////////////////////////////////////////////////////////
void CSoundPlayer::CloseAudio( )
{
	int i = 0;

	if( hWaveOut ) waveOutReset(hWaveOut);

	if( waveBlocks && hWaveOut )
	{
		// unprepare any blocks that are still prepared  
		for(i = 0; i < waveFreeBlockCount; i++)    
			if(waveBlocks[i].dwFlags & WHDR_PREPARED)   
				waveOutUnprepareHeader(hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));   
	}

    //DeleteCriticalSection(&waveCriticalSection);   
    if( waveBlocks ) freeBlocks(waveBlocks);   
    if( hWaveOut ) waveOutClose(hWaveOut);   
    if( hFile ) CloseHandle(hFile); 

	waveBlocks = NULL;
	hWaveOut = NULL;
	hFile = NULL;
}

////////////////////////////////////////////////////////////////////
void CSoundPlayer::writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)   
{   
    WAVEHDR* current;   
    int remain;   
   
    current = &waveBlocks[waveCurrentBlock];   
       
    while(size > 0) 
	{   
		// first make sure the header we're going to use is unprepared  
        if(current->dwFlags & WHDR_PREPARED)    
            waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));   
   
        if(size < (int)(BLOCK_SIZE - current->dwUser)) {   
            memcpy(current->lpData + current->dwUser, data, size);   
            current->dwUser += size;   
            break;   
        }   
   
        remain = BLOCK_SIZE - current->dwUser;   
        memcpy(current->lpData + current->dwUser, data, remain);   
        size -= remain;   
        data += remain;   
        current->dwBufferLength = BLOCK_SIZE;   
          
        waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));   
        waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));   
           
		waveCriticalSection.Enter();   
        waveFreeBlockCount--;   
		waveCriticalSection.Leave();   
           
        // wait for a block to become free   
        while(!waveFreeBlockCount) Sleep(10);   
   
        // point to the next block  
        waveCurrentBlock++;   
        waveCurrentBlock %= BLOCK_COUNT;   
   
        current = &waveBlocks[waveCurrentBlock];   
        current->dwUser = 0;   
    }   
}   

////////////////////////////////////////////////////////////////////
WAVEHDR* CSoundPlayer::allocateBlocks(int size, int count)   
{   
    unsigned char* buffer;   
    int i;   
    WAVEHDR* blocks;   
    DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;   
       
    // allocate memory for the entire set in one go  
    if((buffer = (unsigned char *) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize )) == NULL) 
	{   
        //fprintf(stderr, "Memory allocation error\n");   
        //ExitProcess(1);   
    }   
   
    // and set up the pointers to each bit  
    blocks = (WAVEHDR*)buffer;   
    buffer += sizeof(WAVEHDR) * count;   
    for(i = 0; i < count; i++)
	{   
        blocks[i].dwBufferLength = size;   
        blocks[i].lpData = (LPSTR) buffer;   
        buffer += size;   
    }   
       
    return blocks;   
}   

////////////////////////////////////////////////////////////////////s
void CSoundPlayer::freeBlocks(WAVEHDR* blockArray)   
{   
	// and this is why allocateBlocks works the way it does  
	HeapFree(GetProcessHeap(), 0, blockArray);   
}   
   
