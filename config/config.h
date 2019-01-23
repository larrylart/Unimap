////////////////////////////////////////////////////////////////////
// Package:		config definition
// File:		config.h
// Purpose:		manage software/hardware configuration
//
// Created by:	Larry Lart on 22/04/2006
// Updated by:	Larry Lart on 16/01/2009
//
// Copyright:	(c) 2006-2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_H
#define _CONFIG_H

#define CONFIG_FILE_NAME		"./default.ini"
#define MAX_NO_CONF_SECTION		100
#define MAX_NO_CONF_VARS		100

// define var types;
#define VAR_TYPE_INTEGER				0
#define VAR_TYPE_FLOAT					1
#define VAR_TYPE_STRING					2

#include <wx/arrstr.h>

// CLASS :: CCONFIG
////////////////////////////////////
class CConfig
{
// methods
public:
	CConfig( const wxString& strPath, const wxString& strName );
	~CConfig( );

	void Init( );
	void AddVar( int nSection, int nVarId, const char* strVarName,
						int nVarType );
	int Load( );
	int Save( );

	int SetIntVar( int nSection, int nVarId, int nVarValue );
	int GetIntVar( int nSection, int nVarId, int& nVarValue, int bDefault=0 );

	int GetFloatVar( int nSection, int nVarId, float& nVarValue, int bDefault=0 );
	int GetDoubleVar( int nSection, int nVarId, double& nVarValue, int bDefault=0 );
	int SetFloatVar( int nSection, int nVarId, double nVarValue );

	int SetNumberVarUpdate( int nSection, int nVarId, double nVarValue );

	int SetStrVar( int nSection, int nVarId, char* strVarValue );
	int SetStrVar( int nSection, int nVarId, wxString& strVarValue );
	int SetStrVarUpdate( int nSection, int nVarId, char* strVarValue );
	int SetStrVarUpdate( int nSection, int nVarId, wxString& strVarValue );
	int GetStrVar( int nSection, int nVarId, char* strVarValue, int bDefault=0 );
	int GetStrVar( int nSection, int nVarId, wxString& strVarValue, int bDefault=0 );

// data
public:
	// my data vectors
	double m_vectConfigVar[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];
	wxString m_vectConfigStrVar[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];
	int m_vectConfigVarInit[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];
	int m_vectConfigVarType[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];

	// section/var label definition
	wxArrayString m_vectSectionDef;
	wxArrayString m_arrConfigVar[MAX_NO_CONF_SECTION];

	// config file with path
	wxString m_strConfigFile;
};

#endif
