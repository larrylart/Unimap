
#ifndef _FILES_MANAGER_H
#define _FILES_MANAGER_H

// system libs
#include <stdio.h>
#include <vector>

// wx includes
#include "wx/wxprec.h"

// temporary folders structure
typedef struct
{
	wxString name;
	wxString path;
	wxString desc;
	wxULongLong size;
	unsigned long no;
	unsigned long quota;

} DefTemporaryFolder;

// class:	CFilesManager
///////////////////////////////////////////////////////
class CFilesManager 
{

// public methods
public:
	CFilesManager( );
	~CFilesManager( );

	// process temporary files/folders
	void LoadTemporaryDef( );
	void SaveTemporaryDef( );
	int ProcessTemporary( int bForce=0 );

	std::vector<DefTemporaryFolder> m_vectTemporary;

	int m_bTemporaryDefLoaded;
	int m_bTemporaryProcessed;
};

#endif
