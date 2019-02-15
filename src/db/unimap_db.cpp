////////////////////////////////////////////////////////////////////
// Name:		Unimap Db Class Implementation
// Purpose:		Handle different db storage options
// Created by:	Larry Lart on 21/04/2013
// Updated by:	Larry Lart on 11/08/2014
////////////////////////////////////////////////////////////////////

//system headers
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>

// math libs
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tokenzr.h>
#include <wx/filename.h>

// SQLite
#include "../../lib/sqlite/sqlite3.h"

// main header
#include "unimap_db.h"

// class:		CAstroImage
////////////////////////////////////////////////////////////////////
// Method:		Constructor
////////////////////////////////////////////////////////////////////
CUnimapDb::CUnimapDb( wxString strDbFileName )
{
	m_strDbFileName = strDbFileName;
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
////////////////////////////////////////////////////////////////////
CUnimapDb::~CUnimapDb()
{
}

////////////////////////////////////////////////////////////////////
// Method:		ExecSql
////////////////////////////////////////////////////////////////////
int CUnimapDb::ExecSql( wxString strSql )
{
	int nRet = 0;
	int rc = 0;
	sqlite3 *db;
	char *pErrMsg = 0;

	// open Db
	rc = sqlite3_open( m_strDbFileName.mb_str(), &db );

	if( rc )
	{
		//fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg(db) );
		sqlite3_close( db );
		return(0);
	}

	rc = sqlite3_exec( db, strSql.mb_str(), CUnimapDb::ExecCallback, 0, &pErrMsg );
	if( rc!=SQLITE_OK )
	{
		//fprintf( stderr, "SQL error: %s\n", pErrMsg );
		sqlite3_free( pErrMsg );
	}

	sqlite3_close( db );

	return(nRet);
}

////////////////////////////////////////////////////////////////////
// Method:		ExecCallback
////////////////////////////////////////////////////////////////////
int CUnimapDb::ExecCallback( void *NotUsed, int argc, char **argv, char **azColName )
{
	int i;

	for( i=0; i<argc; i++ )
	{
		//printf( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
	}
	//printf("\n");

	return(0);
}
