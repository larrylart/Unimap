
// use msi package
#define USE_MSI
#ifdef USE_MSI
// system headers
#include <Windows.h> 
#include <Msi.h> 
#include <tchar.h>
// msi system libs
#pragma comment(lib,"msi")
#endif

//wx
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/msw/registry.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/utils.h>

// main header
#include "apps_manager.h"

////////////////////////////////////////////////////////////////////
CAppsManager::CAppsManager( )
{
	m_bAppsDefLoaded = 0;
	m_bAppsProcessed = 0;
	m_vectApps.clear();

	////////////////////////////
	// init apps
	// :: image processing - id=0
	AddApp( wxT("photoshop"), wxT("Photoshop.exe"), wxT("http://www.adobe.com/products/photoshop"), 0 );
	AddApp( wxT("gimp"), wxT("gimp.exe"), wxT("http://www.gimp.org/downloads"), 0 );
	AddApp( wxT("pixinsight"), wxT("pixinsight.exe"), wxT(""), 0 );
	AddApp( wxT("maximdl"), wxT("maximdl.exe"), wxT(""), 0 );
	AddApp( wxT("imageplus"), wxT("imageplus.exe"), wxT(""), 0 );
	// :: image staking - id=1
	AddApp( wxT("registax"), wxT("Registax.exe"), wxT("http://www.astronomie.be/registax/download.html"), 1 );
	AddApp( wxT("deepskystacker"), wxT("DeepSkyStacker.exe"), wxT("http://deepskystacker.free.fr"), 1 );
	AddApp( wxT("iris"), wxT("iris.exe"), wxT(""), 1 );
	AddApp( wxT("registar"), wxT("registar.exe"), wxT(""), 1 );
	AddApp( wxT("k3ccdtools"), wxT("k3ccdtools.exe"), wxT(""), 1 );
	// :: other apps - id=2
	AddApp( wxT("google earth"), wxT("googleearth.exe"), wxT("http://earth.google.com/download-earth.html"), 2 );
	AddApp( wxT("world wind"), wxT("WorldWind.exe"), wxT("http://worldwind.arc.nasa.gov/download.html"), 2 );
}

////////////////////////////////////////////////////////////////////
CAppsManager::~CAppsManager( )
{
	SaveAppsDef();
	m_vectApps.clear();
}

////////////////////////////////////////////////////////////////////
void CAppsManager::AddApp( wxString name, wxString exe, wxString web, int type, wxString desc )
{
	DefAppInstall app;

	app.name = name;
	app.system_name = wxT("");
	app.path = wxT("");
	app.exe = exe;
	app.desc = desc;
	app.cmd_format = wxT("");
	app.webpage = web;
	app.found = 0;
	app.type = type;

	m_vectApps.push_back( app );
}

////////////////////////////////////////////////////////////////////
void CAppsManager::Init( )
{
	// try first to load
	if( !LoadAppsDef() ) SearchApps();
}

////////////////////////////////////////////////////////////////////
void CAppsManager::SaveAppsDef( )
{
	FILE* pFile = NULL;
	int i=0;
/*
	pFile = wxFopen( wxT("./cfg/apps.ini"), wxT("w") );
	// check if there is any configuration to load
	if( !pFile ) return;

	for( i=0; i<m_vectTemporary.size(); i++ )
	{
		wxFprintf( pFile, wxT("\[%s\]\n"), m_vectTemporary[i].name );
		wxFprintf( pFile, wxT("path=%s\n"), m_vectTemporary[i].path );
		wxFprintf( pFile, wxT("quota=%ul\n"), m_vectTemporary[i].quota );
		wxFprintf( pFile, wxT("\n") );
	}
	// close file
	fclose( pFile );
*/
}

////////////////////////////////////////////////////////////////////
int CAppsManager::LoadAppsDef()
{
	if( m_bAppsDefLoaded ) return(1);

	FILE* pFile = NULL;
	wxChar strLine[500];
	wxChar strTmp[500];
	int nLineSize=0, bFoundSection=0, nId=0;
	unsigned long nTmp;

	pFile = wxFopen( wxT("./cfg/apps.ini"), wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return(0);

/*	// Reading lines from cfg file
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
		}
	}
*/
	// close file
	fclose( pFile );
	m_bAppsDefLoaded = 1;

	return(1);
}

////////////////////////////////////////////////////////////////////
int CAppsManager::SearchApps( int bForce )
{
/*	if( m_bTemporaryProcessed && !bForce ) return(0);
	if( !m_bTemporaryDefLoaded ) LoadTemporaryDef();
	if( !m_bTemporaryDefLoaded ) return(0);
*/
	int i=0;

	//////////////
	// check registry - local
	SearchRegistry( wxT("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall") );
	// check registry - user
	SearchRegistry( wxT("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall") );
	
#ifdef USE_MSI
	////////////////
	// LOOK WITH MSI - AT LAST
	SearchMsiApps( );
#endif

	/////////////////
	// EXCEPTION :: GOOGLE EARTH - HKEY_CURRENT_USER\\Software\\Google\\Google Earth Plus
//	if( !CheckRegistryApp( wxT("HKEY_CURRENT_USER\\Software\\Google"), wxT("Google Earth Plus"), MGR_APPS_NAMEID_GOOGLEEARTH ) )
//		CheckRegistryApp( wxT("HKEY_LOCAL_MACHINE\\Software\\Google"), wxT("Google Earth Plus"), MGR_APPS_NAMEID_GOOGLEEARTH );

	/////////////////
	// EXCEPTION :: Deep Sky Stacker
	// HKEY_CLASSES_ROOT\DeepSkyStacker.FileList\shell\open\command
	CheckRegistryAppExt( wxT("HKEY_CLASSES_ROOT\\DeepSkyStacker.FileList\\shell\\open\\command"),
							wxT("Deek Sky Stacker"), MGR_APPS_NAMEID_DEEPSKYSTACKER );

	return(1);
}

#ifdef USE_MSI
////////////////////////////////////////////////////////////////////
void CAppsManager::SearchMsiApps( )
{
	int prod_id=0;
	// with msi
	UINT code = ERROR_SUCCESS; 
	TCHAR prodCode[40]; 
	TCHAR name[1024], manufacturer[1024], version[100], path[1024]; 

	for( DWORD idx = 0; code == ERROR_SUCCESS; idx++ ) 
	{ 
		code =MsiEnumProducts( idx, prodCode ); 
		if (ERROR_SUCCESS != code) break; 
		DWORD sz = sizeof(name) / sizeof(*name); 
		MsiGetProductInfo( prodCode, INSTALLPROPERTY_PRODUCTNAME, name, &sz ); 
		wxString strName = name;

		if( CheckProductName( strName.Lower(), prod_id ) ) 
		{
			// also get install path
			sz = sizeof(path) / sizeof(*path);
			MsiGetProductInfo( prodCode, INSTALLPROPERTY_INSTALLLOCATION, path, &sz ); 
			wxString strPath = path;

			// set values
			m_vectApps[prod_id].system_name = name;
			m_vectApps[prod_id].path = path;
			m_vectApps[prod_id].found = 1;

			// debug
			//wxMessageBox( strPath, wxT("MSI::") + strName );
		}	
	}

	//return(1);
}
#endif

////////////////////////////////////////////////////////////////////
void CAppsManager::SearchRegistry( wxString base_key )
{
	wxString strName;
	wxString strPath;
	long index=1;
	int prod_id=0;

	// also look in HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall HKEY_LOCAL_MACHINE
	//Retrive the number of SubKeys and enumerate them
	wxRegKey* pRegKey = new wxRegKey( base_key );
	if( pRegKey->Exists() )
	{
		bool ok = pRegKey->GetFirstKey( strName, index );
		//for( i=0; i<nSubKeys; i++ )
		while( ok )
		{
			if( CheckProductName( strName.Lower(), prod_id ) ) 
			{
				wxRegKey* pRegKeySub = new wxRegKey( *pRegKey, strName );

				// exceptions
				if( prod_id == MGR_APPS_NAMEID_REGISTAX || 
					prod_id == MGR_APPS_NAMEID_WORLDWIND )
				{
					if( pRegKeySub->HasValue( wxT("UninstallString") ) )
					{
						wxString strTmp;
						pRegKeySub->QueryValue( wxT("UninstallString"), strTmp );
						strPath = wxPathOnly( strTmp ) + wxT("\\");
					}

				} else
				{
					if( pRegKeySub->HasValue( wxT("InstallLocation") ) )
						pRegKeySub->QueryValue( wxT("InstallLocation"), strPath );
				}
				delete( pRegKeySub );
				
				if( !strName.IsEmpty() && !strPath.IsEmpty() )
				{
					// set my record
					m_vectApps[prod_id].system_name = strName;
					m_vectApps[prod_id].path = strPath;
					m_vectApps[prod_id].found = 1;
					/////////
					// find the exe if excetions
					if( prod_id == MGR_APPS_NAMEID_GIMP )
						m_vectApps[prod_id].exe = GetGimpExe( strPath );
					else if( prod_id == MGR_APPS_NAMEID_REGISTAX )
						m_vectApps[prod_id].exe = GetRegistaxExe( strPath );
		
					// debug :: display
					//wxMessageBox( strPath, strName );
				}
			}

			ok = pRegKey->GetNextKey( strName, index );
		}
	}
	delete( pRegKey );
}

////////////////////////////////////////////////////////////////////
int CAppsManager::CheckRegistryApp( wxString base_key, wxString app_key, int app_id )
{
	int bFound=0;
	wxString strPath;
	wxString strKey = base_key + wxT("\\") + app_key;

	wxRegKey* pRegKey = new wxRegKey( strKey );
	if( pRegKey->Exists() )
	{
		if( pRegKey->HasValue( wxT("InstallLocation") ) )
		{
			wxString strTmp;
			pRegKey->QueryValue( wxT("InstallLocation"), strTmp );
			strPath = wxPathOnly( strTmp ) + wxT("\\");

			m_vectApps[app_id].system_name = app_key;
			m_vectApps[app_id].path = strPath;
			m_vectApps[app_id].found = 1;

			bFound = 1;

			// debug :: display
			//wxMessageBox( strPath, app_key );
		}
	}
	delete( pRegKey );

	return( bFound );
}

////////////////////////////////////////////////////////////////////
int CAppsManager::CheckRegistryAppExt( wxString base_key, wxString app_name, int app_id )
{
	int bFound=0;
	wxString strPath;

	wxRegKey* pRegKey = new wxRegKey( base_key );
	if( pRegKey->Exists() )
	{
		if( pRegKey->HasValues( ) )
		{
			wxString strTmp = pRegKey->QueryDefaultValue();
			strPath = wxPathOnly( strTmp ) + wxT("\\");

			if( m_vectApps[app_id].system_name.IsEmpty() && !app_name.IsEmpty() ) m_vectApps[app_id].system_name = app_name;
			m_vectApps[app_id].path = strPath;
			m_vectApps[app_id].found = 1;

			bFound = 1;

			// debug :: display
			//wxMessageBox( strPath, app_name );
		}
	}
	delete( pRegKey );

	return( bFound );
}

////////////////////////////////////////////////////////////////////
wxString CAppsManager::GetGimpExe( wxString path )
{
	wxString strFile, strTFile=wxT("");
	wxString strFileSpec = wxT("*.exe");
	wxString strDir = path + wxT("bin\\");
	wxDir mydir( strDir );
	unsigned long max_fsize = 0;
	wxRegEx reGimp( wxT("gimp\\-[0-9\\.\\-]+\\.exe"), wxRE_EXTENDED|wxRE_ICASE );

	bool ok = mydir.GetFirst( &strFile, strFileSpec );
	while( ok )
	{
		if( reGimp.Matches( strFile ) )
		{
			unsigned long fsize = (unsigned long) wxFileName::GetSize( strFile ).GetValue();
			if( fsize > max_fsize )
			{
				max_fsize = fsize;
				strTFile = strFile;
			}
		}

		ok = mydir.GetNext( &strFile );
	}

	return( wxT("bin\\") + strTFile );
}

////////////////////////////////////////////////////////////////////
wxString CAppsManager::GetRegistaxExe( wxString path )
{
	// check if directory exists
	if( !wxDir::Exists(path) ) return( wxT("") );

	wxString strFile, strTFile=wxT("");
	wxString strFileSpec = wxT("*.exe");
	unsigned long max_fsize = 0;
	wxRegEx reGimp( wxT("registax.*\\.exe"), wxRE_EXTENDED|wxRE_ICASE );
	wxDir mydir( path );

	bool ok = mydir.GetFirst( &strFile, strFileSpec );
	while( ok )
	{
		if( reGimp.Matches( strFile ) )
		{
			unsigned long fsize = (unsigned long) wxFileName::GetSize( strFile ).GetValue();
			if( fsize > max_fsize )
			{
				max_fsize = fsize;
				strTFile = strFile;
			}
		}

		ok = mydir.GetNext( &strFile );
	}

	return( strTFile );
}

////////////////////////////////////////////////////////////////////
int CAppsManager::CheckProductName( wxString name, int& id )
{
	int i=0;

	for( i=0; i<m_vectApps.size(); i++ )
	{
		// skip if already found
		if( m_vectApps[i].found ) continue;
		// check substring
		if( name.Find( m_vectApps[i].name ) != wxNOT_FOUND )
		{
			id = i;
			return(1);
		}
	}

	return(0);
}

////////////////////////////////////////////////////////////////////
int CAppsManager::RunApp( int id, wxString strArg )
{
	wxString strCmd = m_vectApps[id].path + wxT("\\") + m_vectApps[id].exe;

	if( id == MGR_APPS_NAMEID_PHOTOSHOP )
	{
		strCmd += wxT(" ") + strArg;

	} else if( id == MGR_APPS_NAMEID_GIMP )
	{
		strCmd += wxT(" ") + strArg;
	}

	//////////////
	// run application
	// todo: here probably i should use derived class from wxProcess like in power button
	wxExecute( strCmd, wxEXEC_ASYNC );

	return(1);
}
