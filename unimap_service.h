////////////////////////////////////////////////////////////////////
// Package:		unimap service definition
// File:		unimap_service.h
// Purpose:		manage service call from external parties
//
// Created by:	Larry Lart on 10/04/2013
// Updated by:	
//
// Copyright:	(c) 2006-2013 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _UNIMAP_SERVICE_H
#define _UNIMAP_SERVICE_H

// include other local headers
#include "worker.h"
#include "service/soapUnimapSoapXmlBindingService.h"

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_ACTION_DETECT_CHECK_SERVICE		1

///////////
// external classes
class CUnimap;
class CUnimapWorker;
class CAstroImage;
class CConfigMain;
class CConfigDetect;
class CImageDb;
class CUnimapSoapService;
class CGUIFrame;
class CLogger;

//////////////////////////////////////////////////
// class:	CUnimapServiceBase
/////////////////////////////////////////////////
class CUnimapServiceBase :  public wxThread
{
// methods
public:
	CUnimapServiceBase( CLogger *pLogger );
	~CUnimapServiceBase();

	void Log( const wxString& strMsg, int nGUILoggingStyle = GUI_LOGGER_DEFAULT_STYLE );

	virtual bool SetConfig( CConfigMain* pMainConfig );

	// thread entry point
	virtual void *Entry(){ return(NULL); };
	virtual void OnExit();

// data
public:

	CLogger*		m_pLogger;	
	CUnimap*		m_pUnimap;
    CGUIFrame*		m_pFrame;

	CConfigMain*	m_pMainConfig;
	CConfigDetect*	m_pDetectConfig;
	CUnimapWorker* m_pUnimapWorker;

	// process syncronizer/locker
	wxMutex *m_pMutex;

	bool	m_bLife;

	// object i need to manage
//	CStarViewDlg*	m_pStarDlg;

//	CAstroImage*	m_pAstroImage;
//	CImageDb* m_pImageDb;

	CUnimapSoapService* m_pSoapService;

// private data
private:

//	SOAP_SOCKET m_pSoapMasterSocket;
//	SOAP_SOCKET m_pSoapSlaveSocket;
//	struct soap structSoap;
};

//////////////////////////////////////////////////
// class:	CUnimapService
/////////////////////////////////////////////////
class CUnimapService :  public CUnimapServiceBase
{
// methods
public:
	CUnimapService( CLogger *pLogger );
	~CUnimapService();

	void StopService();

	bool SetConfig( CConfigMain* pMainConfig );

	// thread entry point
	void *Entry();
	void OnExit();

	// runtime initialization
	void Entry_Init( );

	int ProcessClientRequest();

// data
public:

	int	m_nServicePort;
	bool m_bSoapServiceConnected;

// private data
private:

};

//////////////////////////////////////////////////
// class:	CUnimapServiceResponse
/////////////////////////////////////////////////
class CUnimapServiceResponse :  public CUnimapServiceBase
{
// methods
public:
	CUnimapServiceResponse( CUnimapSoapService *pSoapService, CLogger *pLogger );
	//~CUnimapServiceResponse();

	// thread entry point
	void *Entry();
	//void OnExit();

// data
public:

};


//////////////////////////////////////////////////
// class:	CUnimapSoapService
/////////////////////////////////////////////////
class CUnimapSoapService : public UnimapSoapXmlBindingService
{
// methods
public:
	CUnimapSoapService( CUnimapService *pUnimapService );
	CUnimapSoapService( CUnimapService *pUnimapService, const struct soap &_soap);
	//~CUnimapSoapService();

	CUnimapService *m_pUnimapService;

	 int ImageSolve(ns__ImageSolveRequestType *ns__ImageSolveRequest, ns__ImageSolveResponseType *ns__ImageSolveResponse);
	 int ScopePositionSet(ns__ScopePositionSetRequestType *input, ns__ScopePositionSetResponseType *result);
	 int ScopePositionGet(ns__ScopePositionGetRequestType *input, ns__ScopePositionGetResponseType *result);
	 int CameraImageTake(ns__CameraImageTakeRequestType *input, ns__CameraImageTakeResponseType *result);
	 int ScopeFocusAdjust(ns__ScopeFocusAdjustRequestType *input, ns__ScopeFocusAdjustResponseType *result);
	
};

#endif


