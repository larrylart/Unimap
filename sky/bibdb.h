
#ifndef _BIB_DB_H
#define _BIB_DB_H

// wx
#include "wx/wxprec.h"

// external classes
class CUnimapWorker;

// class:	CBibDb
///////////////////////////////////////////////////////
class CBibDb
{
public:

	CBibDb( CUnimapWorker* pUnimapWorker );
	~CBibDb();

	void GetBibcodesData();

	// handlers
	wxString ProcessPathFromBibCode( wxString& strBibcode );

	int FindBibIdByName( const wxString& strBibName );

//////////////
// DATA
public:

	CUnimapWorker* m_pUnimapWorker;

	// bibcodes
	int			m_nBibCode;
	wxString	m_vectBibCode[4000];

};

#endif
