////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

//wx
#include "wx/wxprec.h"
#include <wx/dir.h>

// main header
#include "files_manager.h"

////////////////////////////////////////////////////////////////////
CFilesManager::CFilesManager( )
{
	m_bTemporaryDefLoaded = 0;
	m_bTemporaryProcessed = 0;
	m_vectTemporary.clear();
}

////////////////////////////////////////////////////////////////////
CFilesManager::~CFilesManager( )
{
	SaveTemporaryDef();
	m_vectTemporary.clear();
}

////////////////////////////////////////////////////////////////////
void CFilesManager::SaveTemporaryDef( )
{
	if( m_vectTemporary.size() <= 0 || m_bTemporaryProcessed == 0 ) return;

	FILE* pFile = NULL;
	int i=0;

	pFile = wxFopen( wxT("./cfg/temporary.ini"), wxT("w") );
	// check if there is any configuration to load
	if( !pFile ) return;

	for( i=0; i<m_vectTemporary.size(); i++ )
	{
		wxFprintf( pFile, wxT("\[%s\]\n"), m_vectTemporary[i].name );
		wxFprintf( pFile, wxT("path=%s\n"), m_vectTemporary[i].path );
		wxFprintf( pFile, wxT("quota=%u\n"), m_vectTemporary[i].quota );
		wxFprintf( pFile, wxT("desc=%s\n"), m_vectTemporary[i].desc );
		wxFprintf( pFile, wxT("\n") );
	}
	// close file
	fclose( pFile );
}

////////////////////////////////////////////////////////////////////
void CFilesManager::LoadTemporaryDef( )
{
	if( m_bTemporaryDefLoaded ) return;

	FILE* pFile = NULL;
	wxChar strLine[500];
	wxChar strTmp[500];
	int nLineSize=0, bFoundSection=0, nId=0;
	unsigned long nTmp;

	m_vectTemporary.clear();

	pFile = wxFopen( wxT("./cfg/temporary.ini"), wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return;

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		// check section
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
			wxChar strName[255];
			if( wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strName ) )
			{
				DefTemporaryFolder rec;
				rec.name = strName;
				rec.path = wxT("");
				rec.desc = wxT("");
				rec.no = 0;
				rec.size = 0;
				rec.quota = 0;
				m_vectTemporary.push_back( rec );
				nId = m_vectTemporary.size()-1;
			}

		} else if( bFoundSection && wxStrncmp( strLine, wxT("path="), 5 ) == 0 )
		{
			if( wxSscanf( strLine, wxT("path=%[^\n]\n"), strTmp ) )
				m_vectTemporary[nId].path = strTmp;

		} else if( bFoundSection && wxStrncmp( strLine, wxT("quota="), 6 ) == 0 )
		{
			if( wxSscanf( strLine, wxT("quota=%ul\n"), &nTmp ) )
				m_vectTemporary[nId].quota = nTmp;
		
		} else if( bFoundSection && wxStrncmp( strLine, wxT("desc="), 5 ) == 0 )
		{
			if( wxSscanf( strLine, wxT("desc=%[^\n]\n"), strTmp ) )
				m_vectTemporary[nId].desc = strTmp;
		}

	}
	// close file
	fclose( pFile );
	m_bTemporaryDefLoaded = 1;
}

////////////////////////////////////////////////////////////////////
int CFilesManager::ProcessTemporary( int bForce )
{
	if( m_bTemporaryProcessed && !bForce ) return(0);
	if( !m_bTemporaryDefLoaded ) LoadTemporaryDef();
	if( !m_bTemporaryDefLoaded ) return(0);
	int i=0;

	// process size, number of files
	for( i=0; i<m_vectTemporary.size(); i++ )
	{
		wxDir folder( m_vectTemporary[i].path ); 
		m_vectTemporary[i].size = wxDir::GetTotalSize(m_vectTemporary[i].path);
		wxArrayString files;
		m_vectTemporary[i].no = (unsigned long) wxDir::GetAllFiles(m_vectTemporary[i].path, &files);

	}
	m_bTemporaryProcessed = 1;

	return(1);
}

