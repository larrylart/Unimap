////////////////////////////////////////////////////////////////////
// Package:		Config implementation
// File:		config.cpp
// Purpose:		manage software configuration from to file/argv
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	Larry Lart on 31-Jul-2006
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <atlbase.h>
#include <tchar.h>
#include <windows.h>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "wx/defs.h"

// custom
#ifdef _WINDOWS
#include "../util/func.h"
#endif

// wxwindows includes
#include "wx/wxprec.h"
#include <wx/string.h>
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
//#include "../unimap.h"

// main header
#include "config.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CConfig
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfig::CConfig( const wxString& strPath, const wxString& strName )
{
	if( !strPath.IsEmpty() ) 
		m_strConfigFile.Printf( wxT("%s%s%s"), strPath, wxT("\\"), strName );
	else
		m_strConfigFile = strName;

	// call init
//	Init( );
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CConfig
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfig::~CConfig( )
{
	//  save my configuration on exit
	this->Save( );
	// clear stuff left around
	for( int i=0; i<m_vectSectionDef.GetCount(); i++ )
	{
		m_arrConfigVar[i].Clear( );
	}
	m_vectSectionDef.Clear( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CConfig
// Purpose:	inittialize my  object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CConfig::Init( )
{
	int i =0, j = 0;
	// set the init vector
	for( i=0; i<MAX_NO_CONF_SECTION; i++ )
	{
		for( j=0; j<MAX_NO_CONF_VARS; j++ )
		{
			m_vectConfigVarInit[i][j] = 0;
		}
	}

	// initialize my configuration
	if( !Load() )
	{
		// TODO: default initialization ?
	}
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	AddVar
// Class:	CConfig
// Purpose:	add a new variable
// Input:	section id, var id, var name, var type
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CConfig::AddVar( int nSection, int nVarId, const char* strVarName,
						int nVarType )
{
	wxString strWxVarName = wxString(strVarName,wxConvUTF8);
	m_arrConfigVar[nSection].Insert( strWxVarName, nVarId );
	m_vectConfigVarType[nSection][nVarId] = nVarType;

	// init var by type
	if( nVarType == VAR_TYPE_INTEGER || VAR_TYPE_FLOAT )
	{
		m_vectConfigVar[nSection][nVarId] = 0.0;
		m_vectConfigVarInit[nSection][nVarId] = 0;
	} else if( nVarType == VAR_TYPE_STRING )
	{
		m_vectConfigStrVar[nSection][nVarId] = wxT("");
		m_vectConfigVarInit[nSection][nVarId] = 0;
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Int Var
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value 
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetIntVar( int nSection, int nVarId, int nVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( -1 );
	// check if same return 0
	if( m_vectConfigVar[nSection][nVarId] == (double) nVarValue ) return( 0 );
	// set my variable
	m_vectConfigVar[nSection][nVarId] = (double) nVarValue;
	m_vectConfigVarInit[nSection][nVarId] = 1;
//	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_INTEGER;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Float Var
// Class:	CConfig
// Purpose:	set float var of section/var id to a value 
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetFloatVar( int nSection, int nVarId, double nVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( -1 );
	// set my variable
	m_vectConfigVar[nSection][nVarId] = nVarValue;
	m_vectConfigVarInit[nSection][nVarId] = 1;
//	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_FLOAT;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Str Var
// Class:	CConfig
// Purpose:	set string var of section/var id to a value 
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVar( int nSection, int nVarId, char* strVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( -1 );
	// set my variable
	m_vectConfigStrVar[nSection][nVarId] = wxString(strVarValue,wxConvUTF8);
	m_vectConfigVarInit[nSection][nVarId] = 1;
//	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_STRING;

	return( 1 );
}

// use wxString for unicode - todo: convert all places where this call
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVar( int nSection, int nVarId, wxString& strVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( -1 );
	// set my variable
	m_vectConfigStrVar[nSection][nVarId] = strVarValue;
	m_vectConfigVarInit[nSection][nVarId] = 1;
//	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_STRING;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Int Var Update
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value and call remote 
//			objects to update
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetNumberVarUpdate( int nSection, int nVarId, double nVarValue )
{
	int nStatus = -1;
	if( m_vectConfigVarType[nSection][nVarId] == VAR_TYPE_INTEGER )
		nStatus = SetIntVar( nSection, nVarId, (int) nVarValue );
	else if( m_vectConfigVarType[nSection][nVarId] == VAR_TYPE_FLOAT )
		nStatus = SetFloatVar( nSection, nVarId, nVarValue );
		
	if( nStatus == -1 ) return( -1 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Str Var Update
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value and call remote 
//			objects to update
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVarUpdate( int nSection, int nVarId, char* strVarValue )
{

	if( SetStrVar( nSection, nVarId, strVarValue ) == -1 ) return( -1 );

	return( 1 );
}

// with wstring for unicode
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVarUpdate( int nSection, int nVarId, wxString& strVarValue )
{

	if( SetStrVar( nSection, nVarId, strVarValue ) == -1 ) return( -1 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get Integer Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetIntVar( int nSection, int nVarId, int& nVarValue, int bDefault )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return( 0 );

	// set the default - i pass to this
	if( m_vectConfigVarInit[nSection][nVarId] == 0  ) 
	{
		if( bDefault ) SetIntVar( nSection, nVarId, nVarValue );
		return ( 0 );
	}
	
	// set my var 
	nVarValue = m_vectConfigVar[nSection][nVarId];

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get Float Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetFloatVar( int nSection, int nVarId, float& nVarValue, int bDefault )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 )
	{
		// set the default - i pass to this
		if( bDefault ) SetFloatVar( nSection, nVarId, (double) nVarValue );
		return ( 0 );
	}
	
	// set my var 
	nVarValue = (float) m_vectConfigVar[nSection][nVarId];

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get double Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetDoubleVar( int nSection, int nVarId, double& nVarValue, int bDefault )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 )
	{
		// set the default - i pass to this
		if( bDefault ) SetFloatVar( nSection, nVarId, nVarValue );
		return ( 0 );
	}

	// set my var 
	nVarValue = m_vectConfigVar[nSection][nVarId];

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get String Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetStrVar( int nSection, int nVarId, char* strVarValue, int bDefault )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 )
	{
		// set the default - i pass to this
		if( bDefault ) SetStrVar( nSection, nVarId, strVarValue );
		return ( 0 );
	}
	
	// set my var 
	sprintf( strVarValue, "%s", m_vectConfigStrVar[nSection][nVarId].ToAscii() );

	// return success = 1
	return( 1 );
}

// use wxtString for unicode
/////////////////////////////////////////////////////////////////////
int CConfig::GetStrVar( int nSection, int nVarId, wxString& strVarValue, int bDefault )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 )
	{
		// set the default - i pass to this
		if( bDefault ) SetStrVar( nSection, nVarId, strVarValue );
		return ( 0 );
	}
	
	// set my var 
	strVarValue = m_vectConfigStrVar[nSection][nVarId];

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load 
// Class:	CConfig
// Purpose:	load my software configuration from file
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::Load( )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
//	wxChar strVarName[256];
//	wxChar strVarValue[256];
	double nVarFloatValue = 0;
	long nVarIntValue = 0;
	int nSection = -1;
	// define my regex 
	wxRegEx reSection( wxT("\\[([a-zA-Z0-9_\\-]+)\\]") );
	wxRegEx reVariable( wxT( "([a-zA-Z0-9_\\-]+)\\ *=\\ *([^\n^\r^\t]+)\\ *" ) );

	// Read the index
	pFile = wxFopen( m_strConfigFile, wxT("r") );

	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
		nVarIntValue = 0;
		nVarFloatValue = 0;
		bzero( strLine, 2000 );
//		bzero( strVarName, 256 );
//		bzero( strVarValue, 256 );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );

//		wxString strWxLine(strLine,wxConvUTF8);

		// check if section start
		if( reSection.Matches( strLine ) )
		{
			wxString strSection = reSection.GetMatch(strLine, 1 );
			int nTmp = m_vectSectionDef.Index( strSection );
			if( nTmp >= 0 ) 
				nSection = nTmp;
			else
				nSection = m_vectSectionDef.Add( strSection );

		} else if( nSection >=0 && reVariable.Matches( strLine ) )
		{
			wxString strVarName = reVariable.GetMatch(strLine, 1 );
			wxString strVarValue = reVariable.GetMatch(strLine, 2 );
			
			int nVarId = m_arrConfigVar[nSection].Index( strVarName );
			// check if variable is number or string
			if( m_vectConfigVarType[nSection][nVarId] == VAR_TYPE_FLOAT && 
				strVarValue.ToDouble( &nVarFloatValue ) )
			{				
				SetNumberVarUpdate( nSection, nVarId, nVarFloatValue );

			} else if( m_vectConfigVarType[nSection][nVarId] == VAR_TYPE_INTEGER &&
						strVarValue.ToLong( &nVarIntValue ) )
			{
				SetNumberVarUpdate( nSection, nVarId, (double) nVarIntValue );
			} else if( m_vectConfigVarType[nSection][nVarId] == VAR_TYPE_STRING )
			{
				//char strTmp[255];
				//sprintf( strTmp, "%s", strVarValue.ToAscii() );

				SetStrVarUpdate( nSection, nVarId, strVarValue );
			}
		}
	}

	// close file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Save 
// Class:	CConfig
// Purpose:	save my configuration to a file/db ?
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::Save( )
{
	FILE* pFile = NULL;
	unsigned int i = 0, j = 0;
	int nVarIntValue = 0;
	double nVarFloatValue = 0;
	wxString strVarValue;


	// open config file to write
	pFile = wxFopen( m_strConfigFile, wxT("w") );
	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// for each section
	for( i=0; i<m_vectSectionDef.GetCount() ; i++ )
	{
		// check if defined
		//??if( !m_vectSectionDef.Item( i ) ) continue;
		// print header
		wxFprintf( pFile, wxT("\n\n%s%s%s\n"), "[", m_vectSectionDef[i], "]" );
		// for each possibile var id
		for( j=0; j<m_arrConfigVar[i].GetCount(); j++ )
		{
			if( m_vectConfigVarType[i][j] == VAR_TYPE_INTEGER )
			{
				if( GetIntVar( i, j, nVarIntValue ) != 0 )
				{
					wxFprintf( pFile, wxT("%s=%d\n"), m_arrConfigVar[i].Item( j ),  
							 nVarIntValue );
				}

			} else if( m_vectConfigVarType[i][j] == VAR_TYPE_FLOAT )
			{
				if( GetDoubleVar( i, j, nVarFloatValue ) != 0 )
				{
					wxFprintf( pFile, wxT("%s=%.20f\n"), m_arrConfigVar[i].Item( j ),  
							 nVarFloatValue );
				}

			} else if( m_vectConfigVarType[i][j] == VAR_TYPE_STRING )
			{
				if( GetStrVar( i, j, strVarValue ) != 0 )
				{
					wxFprintf( pFile, wxT("%s=%s\n"), m_arrConfigVar[i].Item( j ), strVarValue );
				}
			}
		}
	}
	// close my file handler
	fclose( pFile );

	return( 1 );
}

