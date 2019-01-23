
// wx 
#include "wx/wxprec.h"
#include <wx/app.h>

// local headers
#include "../unimap.h"
#include "../sound/sound_engine.h"
#include "../image/astroimage.h"

// main header
#include "image_note.h"

//extern CUnimap* g_pUnimap;
DECLARE_APP(CUnimap)

// constructor/destructor
////////////////////////////////////////////////////////////////////
CImageNote::CImageNote( CAstroImage* pAstroImage )
{
	m_pAstroImage = pAstroImage;

	m_nId = 0;
	m_nType = 0;
	m_rDateTime.SetToCurrent();
	m_strTitle = wxT("");
	m_strMessage = wxT("");
	m_strSoundFile = wxT("");
	x = -1.0;
	y = -1.0;

	m_pSoundEngine = NULL;
}

////////////////////////////////////////////////////////////////////
CImageNote::~CImageNote( )
{
	if( m_pSoundEngine ) wxGetApp().StopSoundEngine();
}

// some notes handlers
////////////////////////////////////////////////////////////////////
void CImageNote::CopyFrom( CImageNote& note )
{
	m_nId = note.m_nId;
	m_nType = note.m_nType;
	m_rDateTime = note.m_rDateTime;
	m_strTitle = note.m_strTitle;
	m_strMessage = note.m_strMessage;
	m_strSoundFile = note.m_strSoundFile;
	x = note.x;
	y = note.y;
}

// audio notes handlers
////////////////////////////////////////////////////////////////////
int CImageNote::RecordAudioNote_Start()
{
	if( !m_pAstroImage ) return( 0 );

	// build sound file name/path
	int nNextNoteId = m_pAstroImage->m_vectNotes.size();

	m_strSoundPath.sprintf( wxT("%s/%s.nts"), m_pAstroImage->m_strImagePath, m_pAstroImage->m_strImageName );
	m_strSoundFile.sprintf( wxT("%s/%s_%d.wav"), m_strSoundPath, m_pAstroImage->m_strImageName, nNextNoteId );
	// first we check if folder exists
	if( !wxDirExists( m_strSoundPath ) ) wxMkdir( m_strSoundPath );

	m_pSoundEngine = wxGetApp().StartSoundEngine( m_strSoundFile );
	if( m_pSoundEngine ) 
		m_pSoundEngine->StartRecording();
	else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CImageNote::RecordAudioNote_Stop( wxEvtHandler* handler )
{
	if( !m_pSoundEngine ) return( 0 );

	m_pSoundEngine->StopRecording( handler );
//	wxGetApp().StopSoundEngine( );
	m_pSoundEngine = NULL;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CImageNote::PlayAudioNote_Start( wxEvtHandler* handler )
{
	m_pSoundEngine = wxGetApp().StartSoundEngine( m_strSoundFile );
	if( m_pSoundEngine ) 
		m_pSoundEngine->StartPlaying( handler );
	else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CImageNote::PlayAudioNote_Stop( wxEvtHandler* handler )
{
	if( !m_pSoundEngine ) return( 0 );
	m_pSoundEngine->StopPlaying( handler );

	wxGetApp().StopSoundEngine( );
	m_pSoundEngine = NULL;

	return( 1 );
}
