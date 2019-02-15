////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _WEBFUNC_IMAGE
#define _WEBFUNC_IMAGE

#include <wx/xml/xml.h>

class CUnimapWorker;

// download an web file from url to local file
long LoadWebFileT( const wxString& strUrlGetData, const wxString& strFile, CUnimapWorker* m_pUnimapWorker=NULL, unsigned long nFileSize=0, int bBinary=0 );
int LoadWebFile( const wxString& strUrlGetData, const wxString& strFile );
int GetWebPage( const wxString& strUrlGetData, wxString& strPage );
int GetWebXmlDoc( const wxString& strUrlGetData, wxXmlDocument& xml );
int cGetWebXmlDoc( const wxString& strUrlGetData, wxXmlDocument& xml );

// curl methods
int GetHttpsPage( const char* strUrlGetData );

#endif
