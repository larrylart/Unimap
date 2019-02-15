////////////////////////////////////////////////////////////////////
// Package:		unimap service implementation
// File:		unimap_service.cpp
// Purpose:		manage service call from external parties
//
// Created by:	Larry Lart on 10/04/2013
// Updated by:	
//
// Copyright:	(c) 2004-2013 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

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
#include <wx/process.h>
#include <wx/socket.h>
#include <wx/utils.h>
#include <wx/app.h>
#include <wx/filename.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#ifdef WIN32
#include "util/func.h"
#endif

//#include "logger/logger.h"
#include "unimap.h"
#include "gui/frame.h"
#include "unimap_worker.h"
#include "image/astroimage.h"
#include "image/imagegroup.h"
#include "image/imagedb.h"
#include "config/config.h"
#include "config/mainconfig.h"
#include "config/detectconfig.h"
#define NOCTYPES
#include "proc/sextractor.h"
#undef NOCTYPES
#include "sky/sky.h"
// soap service
//#include "service/unimap_soap_service.h"
#include "service/soapH.h"
#include "service/UnimapSoapXmlBinding.nsmap"

// main header include
#include "unimap_service.h"

//		***** CLASS CUnimapServiceBase *****
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimapServiceBase
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapServiceBase::CUnimapServiceBase( CLogger *pLogger ) : wxThread( wxTHREAD_DETACHED )
{ 
	// set my processing object to null
	m_pUnimap =  NULL;
	m_pFrame = NULL;

	m_pSoapService = NULL;

	// reset action flags
	m_bLife = false;
	m_pMutex = new wxMutex( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CUnimapServiceBase
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapServiceBase::~CUnimapServiceBase( )
{
	m_pLogger = NULL;
	if( m_pMutex ) delete( m_pMutex );

	if( m_pSoapService != NULL ) OnExit();
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CUnimapServiceBase
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CUnimapServiceBase::OnExit( )
{
	// destroy - clean my place
	if( m_pMutex ) delete( m_pMutex );
	m_pMutex = NULL;

	if( m_pSoapService != NULL ) 
	{
		// cleanup class instances
		soap_destroy((struct soap*)m_pSoapService);
		// cleanup
		soap_end((struct soap*)m_pSoapService);
		// detach and free thread's copy of soap environment
		soap_free((struct soap*)m_pSoapService);

		//delete( m_pSoapService );
		m_pSoapService = NULL;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Log
// Class:		CUnimapServiceBase
// Purpose:		log event by sening it using a command message
// Input:		message to log
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUnimapServiceBase::Log( const wxString& strMsg, int nGUILoggingStyle )
{
//	return;

	if( m_pFrame != NULL )
	{
		wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_REMOTE_LOGGER );
		event.SetEventObject( NULL );
		// Give it some contents
		event.SetString( strMsg );
		// set the loggin style
		event.SetInt( nGUILoggingStyle );
		// Send it
		m_pFrame->GetEventHandler()->ProcessEvent( event );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CUnimapServiceBase
// Purpose:	set my action configuration
// Input:	pointe config object
// Output:	nothing	
////////////////////////////////////////////////////////////////////
bool CUnimapServiceBase::SetConfig( CConfigMain* pMainConfig )
{
	// check if valid reference
	if( !pMainConfig ) return(false);
	m_pMainConfig = pMainConfig;

	return(true);
}

//		***** CLASS CUnimapService *****
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimapService
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapService::CUnimapService( CLogger *pLogger ) : CUnimapServiceBase( pLogger )
{ 
	// gsoap init
	m_nServicePort = 11011;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
////////////////////////////////////////////////////////////////////
CUnimapService::~CUnimapService( )
{
	// clean up here
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
////////////////////////////////////////////////////////////////////
void CUnimapService::OnExit( )
{
	// clean up here
	return;
}

////////////////////////////////////////////////////////////////////
void CUnimapService::StopService()
{
	m_bLife = 0;

	wxSocketClient* pLocalClient = new wxSocketClient( wxSOCKET_BLOCK );

	// set address
	//wxString strAddr = m_strRemoteServerAddr;
	wxIPV4address addr;
	// larry's desktop
	addr.Hostname( wxT("127.0.0.1") );
	addr.Service( 11011 );

	if( pLocalClient->Connect(addr, true) ) 
	{

	} else
	{

	}

	delete(pLocalClient);
	pLocalClient = NULL;

	//soap_closesock((struct soap*)m_pSoapService);
  // cleanup class instances
//  soap_destroy((struct soap*)m_pSoapService);
  // cleanup
//  soap_end((struct soap*)m_pSoapService);
  // detach and free thread's copy of soap environment
//  soap_free((struct soap*)m_pSoapService);
//  m_pSoapService = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CUnimapService
// Purpose:	set my action configuration
// Input:	pointe config object
// Output:	nothing	
////////////////////////////////////////////////////////////////////
bool CUnimapService::SetConfig( CConfigMain* pMainConfig )
{
	// check if valid reference
	if( !CUnimapServiceBase::SetConfig( pMainConfig ) ) return(false);

	int nVarInt = 0;
	int bChange = 0;

	// SECTION :: ACTION
	//////////////////////////////////
	if( m_pMainConfig->GetIntVar( SECTION_ID_SERVICES, CONF_SERVICES_SERVER_PORT, m_nServicePort, 1 ) == 0 ) bChange = 1;

	// check if default updates then save config
	if( bChange > 0 ) m_pMainConfig->Save( );

	return(true);
}

////////////////////////////////////////////////////////////////////
void CUnimapService::Entry_Init()
{
	m_pSoapService = new CUnimapSoapService( this );

	if (soap_valid_socket(m_pSoapService->bind(NULL, m_nServicePort, 100)))
		m_bSoapServiceConnected = true;
	else
		m_bSoapServiceConnected = false;

}

int CUnimapService::ProcessClientRequest()
{
	// make a copy of soap data
	struct soap* tcopy = soap_copy( (struct soap*) m_pSoapService);

	CUnimapSoapService* pSoapService =  new CUnimapSoapService( this, *tcopy );

	CUnimapServiceResponse* pUnimapServiceResponse = new CUnimapServiceResponse( pSoapService, m_pLogger );

	if ( pUnimapServiceResponse->Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
		return(0);

	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );

	}

	// start the thread
	if ( pUnimapServiceResponse->Run() != wxTHREAD_NO_ERROR )
	{
		return(0);
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	//delete(pSoapService);

	return(1);
}

////////////////////////////////////////////////////////////////////
// Method:		Entry
// Class:		CWorker
// Purpose:		the main executable body of my thread
// Input:		nothing
// Output:		void pointer
////////////////////////////////////////////////////////////////////
void *CUnimapService::Entry( )
{
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	m_bLife = true;

    // run iterative server on port until fatal error 
    /*if( m_pSoapService->run(m_nServicePort) )
    { 
		m_pSoapService->soap_stream_fault(std::cerr);
		return(0);
		//exit(-1);
    }*/

	// interface to init in entry from derived class
	Entry_Init();

	// port from run - to do just one step
	if( m_bSoapServiceConnected )
	{	
		while( m_bLife )
		{
			// accept connection
			if (!soap_valid_socket(m_pSoapService->accept()))
			{
				// here to do if socket not valid - reconnect

				if( m_pSoapService != NULL )
					return( NULL );// m_pSoapService->error;
				else
					return( NULL ); //return(1);
			}
			// if thread still to run
			if( m_bLife )
			{
				// copy to Client Thread response
				ProcessClientRequest();

			} else
				return( NULL ); //return(0);
		}
	}
	else
		return( NULL ); //return m_pSoapService->error;

	//return SOAP_OK;


	// actions
	//if( m_nActionCmd == THREAD_ACTION_IMAGE_LOAD )
	{

	}

	CoUninitialize();

	return( NULL );
}

//		***** CLASS CUnimapServiceResponse *****
////////////////////////////////////////////////////////////////////
CUnimapServiceResponse::CUnimapServiceResponse( CUnimapSoapService *pSoapService, CLogger *pLogger ) : CUnimapServiceBase(pLogger)
{ 
	m_pSoapService = pSoapService;
}

////////////////////////////////////////////////////////////////////
void *CUnimapServiceResponse::Entry( )
{
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	m_bLife = true;

	(void) m_pSoapService->serve();

	CoUninitialize();

	return( NULL );
}

//		***** CLASS CUnimapSoapService *****
////////////////////////////////////////////////////////////////////
int UnimapSoapXmlBindingService::ImageSolve(ns__ImageSolveRequestType *ns__ImageSolveRequest, ns__ImageSolveResponseType *ns__ImageSolveResponse){ return(0); };
int UnimapSoapXmlBindingService::ScopePositionSet(ns__ScopePositionSetRequestType *input, ns__ScopePositionSetResponseType *result){ return(0); };
int UnimapSoapXmlBindingService::ScopePositionGet(ns__ScopePositionGetRequestType *input, ns__ScopePositionGetResponseType *result){ return(0); };
int UnimapSoapXmlBindingService::CameraImageTake(ns__CameraImageTakeRequestType *input, ns__CameraImageTakeResponseType *result){ return(0); };
int UnimapSoapXmlBindingService::ScopeFocusAdjust(ns__ScopeFocusAdjustRequestType *input, ns__ScopeFocusAdjustResponseType *result){ return(0); };

////////////////////////////////////////////////////////////////////
CUnimapSoapService::CUnimapSoapService( CUnimapService *pUnimapService ) : UnimapSoapXmlBindingService()
{ 
	// set my processing object
	m_pUnimapService =  pUnimapService;
	soap_omode( (soap*) pUnimapService, SOAP_XML_DEFAULTNS);
}

////////////////////////////////////////////////////////////////////
CUnimapSoapService::CUnimapSoapService( CUnimapService *pUnimapService, const struct soap &_soap) : UnimapSoapXmlBindingService(_soap)
{ 
	// set my processing object
	m_pUnimapService =  pUnimapService;
	soap_omode( (soap*) pUnimapService, SOAP_XML_DEFAULTNS);
}

////////////////////////////////////////////////////////////////////
int CUnimapSoapService::ImageSolve(ns__ImageSolveRequestType *ns__ImageSolveRequest, ns__ImageSolveResponseType *ns__ImageSolveResponse)
{
	int i = 0;
	//char str_tmp[255];

	//*result = 99.991345;
	/*ns1__ResponseStatusType res;
	res.ResponseCode = 0;
	res.ResponseMessage[0] = "OK";
	res.ResponseResult = "RESULT OK";
	*/

	//soap_malloc
	//ns1__ImageSolveResponseType ppp;
	//ns__ImageSolveResponse = soap_new_ns__ImageSolveResponseType(this,-1);

	wxString strImagePath = wxString(ns__ImageSolveRequest->ImagePath.c_str(),wxConvUTF8);
	wxString strImageName = wxString(ns__ImageSolveRequest->ImageName.c_str(),wxConvUTF8); 
	wxString strSearchCatalog = wxString(ns__ImageSolveRequest->SearchCatalog.c_str(),wxConvUTF8); 
	//strImagePath.Printf( wxT("%s"), ns__ImageSolveRequest->ImagePath.c_str() );
	//strImageName.Printf( wxT("%s"), ns__ImageSolveRequest->ImageName.c_str() );

	wxFileName rFileName( strImagePath, strImageName ); 
	CAstroImage* pAstroImage = new CAstroImage( NULL, rFileName.GetPath(), rFileName.GetName(), rFileName.GetExt() );

	// load image data
//	pAstroImage->Load(0);

	/////////
	// SET WORKER ACTION
	DefCmdAction act;
    wxMutex mutex;
    wxCondition condition(mutex);
	act.id = THREAD_ACTION_SERVICES_IMAGE_PLATESOLVE;
	act.vectObj[0] = (void*) &mutex;
	act.vectObj[1] = (void*) &condition;
	act.vectObj[2] = (void*) pAstroImage;
	act.vectStr[0] = strSearchCatalog;
	//act.vectStr[1] = strImagePath;

	// add to the main thread queue
	m_pUnimapService->m_pUnimapWorker->SetAction( act );

	// and wait to finish
	condition.Wait();


	/////////////////////////////////////////////
	// BUILD RESPONSE
	// :: STATUS MESSAGE/LOGS
	ns__ImageSolveResponse->ResponseStatus = soap_new_ns__ResponseStatusType(this,-1);
	ns__ImageSolveResponse->ResponseStatus->ResponseCode = 0;
	ns__ImageSolveResponse->ResponseStatus->ResponseMessage.push_back( "OK" );
	ns__ImageSolveResponse->ResponseStatus->ResponseResult = "RESULT OK";
	
	// IMAGE DATA HEADER
	ns__ImageSolveResponse->ImgDataHeader = soap_new_ns__ImgDataHeaderType(this,-1);
	ns__ImageSolveResponse->ImgDataHeader->ObjDetected = std::to_string( (_Longlong) pAstroImage->m_nStar);
	ns__ImageSolveResponse->ImgDataHeader->ObjMatched = std::to_string( (_Longlong) pAstroImage->m_nMatchedStars);
	ns__ImageSolveResponse->ImgDataHeader->TargetName = std::string(pAstroImage->m_strTargetName.mb_str());;
	ns__ImageSolveResponse->ImgDataHeader->OrigRa = pAstroImage->m_nOrigRa;
	ns__ImageSolveResponse->ImgDataHeader->OrigDec = pAstroImage->m_nOrigDec;
	ns__ImageSolveResponse->ImgDataHeader->FieldWidth = pAstroImage->m_nFieldWidth;
	ns__ImageSolveResponse->ImgDataHeader->FieldHeight = pAstroImage->m_nFieldHeight;
	ns__ImageSolveResponse->ImgDataHeader->MinMag = pAstroImage->m_nMinMag;
	ns__ImageSolveResponse->ImgDataHeader->MaxMag = pAstroImage->m_nMaxMag;

	///////////////////////////////
	// write detected stars
	for( i=0; i<pAstroImage->m_nStar; i++ )
	{
		if( pAstroImage->m_vectStar[i].match <= 0 ) continue;

		ns__ImgObjDetectedType* elem = soap_new_ns__ImgObjDetectedType(this,-1);
		elem->id = std::to_string( (_Longlong) i );
		if( pAstroImage->m_vectStar[i].cat_name )
			elem->name = pAstroImage->m_vectStar[i].cat_name;
		else
			elem->name = "";
		elem->x = pAstroImage->m_vectStar[i].x;
		elem->y = pAstroImage->m_vectStar[i].y;
		elem->ra = pAstroImage->m_vectStar[i].ra;
		elem->dec = pAstroImage->m_vectStar[i].dec;
		elem->mag = pAstroImage->m_vectStar[i].mag;

		ns__ImageSolveResponse->vectImgObjDetected.push_back( elem );
		
		if( i == 1 ) break;
	}
	
	// cleanup
	delete( pAstroImage );		

	return(0);
}

////////////////////////////////////////////////////////////////////
int CUnimapSoapService::ScopePositionSet(ns__ScopePositionSetRequestType *input, ns__ScopePositionSetResponseType *result)
{
	return(0);
}

////////////////////////////////////////////////////////////////////
int CUnimapSoapService::ScopePositionGet(ns__ScopePositionGetRequestType *input, ns__ScopePositionGetResponseType *result)
{
	return(0);
}

////////////////////////////////////////////////////////////////////
int CUnimapSoapService::CameraImageTake(ns__CameraImageTakeRequestType *input, ns__CameraImageTakeResponseType *result)
{
	return(0);
}

////////////////////////////////////////////////////////////////////
int CUnimapSoapService::ScopeFocusAdjust(ns__ScopeFocusAdjustRequestType *input, ns__ScopeFocusAdjustResponseType *result)
{
	return(0);
}
