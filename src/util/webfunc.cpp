////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/url.h>
#include <wx/tokenzr.h>
// curl
#include <curl/curl.h>
#include "../httpengine/wxcurl_http.h"

// local headers
#include "../util/func.h"
#include "../unimap.h"
#include "../unimap_worker.h"

// main header
#include "webfunc.h"

////////////////////////////////////////////////////////////////////
// Method:		LoadWebFileT
// Class:		CGUIFrame
// Purpose:		download web file
// Input:		url, local file name
// Output:		no of bytes read
////////////////////////////////////////////////////////////////////
long LoadWebFileT( const wxString& strUrlGetData, const wxString& strFile, 
				  CUnimapWorker* m_pUnimapWorker, unsigned long nFileSize, int bBinary )
{
	int bError=0;
	FILE* pLocalFile = NULL;

    wxURL url(strUrlGetData);  
    wxInputStream* m_in = url.GetInputStream();
	// check if I got a stream
	if( !m_in ) return( 0 );

    size_t nRead;
	long nPCount=0, nPCountStep=0, nTotal=0;
	int nPStep=0;
	// get size - if any
	long nStreamSize = m_in->GetSize();
	// calculate sizes
	if( m_pUnimapWorker && nFileSize ) nPCountStep = (long) nFileSize/100;

    if(m_in)
    {
		// open local file to write
		if( bBinary )
			pLocalFile = wxFopen( strFile, wxT("wb") );
		else
			pLocalFile = wxFopen( strFile, wxT("w") );
		// if failure - exit
		if( !pLocalFile ) return( 0 );

		// read remote data
		char buf[100];
		do
		{
			nRead = m_in->Read(buf, 100).LastRead();
			if( nRead > 0 ) fwrite( buf, 1, nRead, pLocalFile );
			nTotal++;

			// if unimap pointer and size set the I should do progress bar
			if( m_pUnimapWorker && nFileSize )
			{
				// check if break
				if( m_pUnimapWorker->m_bCurrentTaskSetToBreak )
				{
					bError = 1;
					break;
				}

				if( nPCount > nPCountStep )
				{
					nPCount=0;
					nPStep++;
					m_pUnimapWorker->SetWaitDialogProgress( nPStep );
					continue;

				} else
					nPCount += nRead;
			}

		} while( nRead == 100 && !m_in->Eof() && m_in->GetLastError() == wxSTREAM_NO_ERROR );

		// close file for writing
		fclose( pLocalFile );
		// delete file downloaded if error
		if( bError ) wxRemoveFile( strFile );
	}

	// check for errors
	if( !bError )
		return( nTotal );
	else
		return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadWebFile
// Class:		CGUIFrame
// Purpose:		download web file
// Input:		url, local file name
// Output:		nothing
////////////////////////////////////////////////////////////////////
int LoadWebFile( const wxString& strUrlGetData, const wxString& strFile )
{
	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;

	////////////////////////////
	// here we initialize the reading process
	wxFileSystem *pUrl = new wxFileSystem();
	pUrl->AddHandler( new wxInternetFSHandler );

	// then open data
	pFile = pUrl->OpenFile( strUrlGetData );

	if( !pFile || pFile == NULL ) return( 0 );
	
	pStream = pFile->GetStream( );

	// open local file to write
	pLocalFile = wxFopen( strFile, wxT("w") );
	if( !pLocalFile )
	{
		return( 0 );
	}

	// read the entire bufer
	do
	{
		int nChar = pStream->GetC();
		if( pStream->LastRead() ) fputc( nChar, pLocalFile );  

	} while( pStream->LastRead() );

	// close file for writing
	fclose( pLocalFile );

	delete( pFile );
	delete( pUrl );


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetWebPage
// Class:		CGUIFrame
// Purpose:		download web page into a string
// Input:		url, local file name
// Output:		nothing
////////////////////////////////////////////////////////////////////
int GetWebPage( const wxString& strUrlGetData, wxString& strPage )
{
	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;

	////////////////////////////
	// here we initialize the reading process
	wxFileSystem *pUrl = new wxFileSystem();
	pUrl->AddHandler( new wxInternetFSHandler );

	// then open data
	pFile = pUrl->OpenFile( strUrlGetData );

	if( !pFile || pFile == NULL ) return( 0 );
	
	pStream = pFile->GetStream( );

	// read the entire bufer
	do
	{
		wxChar nChar = pStream->GetC();
		if( pStream->LastRead() ) strPage.Append( nChar );

	} while( pStream->LastRead() );

	delete( pFile );
	delete( pUrl );


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetWebXmlDoc
// Class:		CGUIFrame
// Purpose:		download web page and load it into an xml document
// Input:		url as string, reference to xml document to populate
// Output:		1 = ok, 0 = failed
////////////////////////////////////////////////////////////////////
int GetWebXmlDoc( const wxString& strUrlGetData, wxXmlDocument& xml )
{
	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;
	int nState = 0;

	////////////////////////////
	// here we initialize the reading process
	wxFileSystem *pUrl = new wxFileSystem();
	pUrl->AddHandler( new wxInternetFSHandler );

	// then open data
	pFile = pUrl->OpenFile( strUrlGetData );
	// check if success
	if( !pFile || pFile == NULL )
		 nState = 0;
	else
	{
		// get stream
		pStream = pFile->GetStream( );
		// load into xml doc
		if( xml.Load( *pStream ) ) nState = 1;
	}

	delete( pFile );
	delete( pUrl );

	return( nState );
}

////////////////////////////////////////////////////////////////////
int cGetWebXmlDoc( const wxString& strUrlGetData, wxXmlDocument& xml )
{
	int nState = 0;

	wxCurlHTTP* pHttp = new wxCurlHTTP();
	pHttp->SetBaseURL( strUrlGetData );

	// then get data
	if( pHttp->Get( wxT("./ww.xml") ) )
	{
		// load into xml doc
		if( xml.Load( wxT("./ww.xml") ) ) nState = 1;
	}

	delete( pHttp );

	return( nState );
}


////////////////////////////////////////////////////////////////////
int GetHttpsPage( const wxString& strUrlGetData )
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if( curl )
  {
		curl_easy_setopt(curl, CURLOPT_URL, "https://sourceforge.net/");

#ifdef SKIP_PEER_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0); 

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  return( 0 );
}

