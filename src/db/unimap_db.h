////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _UNIMAP_DB
#define _UNIMAP_DB

// system headers
#include <time.h>
#include <vector>
#include <cmath>

// class: CUnimapDb
class CUnimapDb 
{

// public methods
public:
	// constructor/destructor
	CUnimapDb( wxString strDbFileName );
	~CUnimapDb();

	int ExecSql( wxString strSql );

	static int ExecCallback( void *NotUsed, int argc, char **argv, char **azColName );

// public data
public:
	wxString m_strDbFileName;
};

#endif
