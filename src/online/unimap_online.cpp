////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

// include local headers
#include "../util/func.h"
#include "../util/folders.h"
#include "../util/crypt-md5.h"
#include "../httpengine/wxcurl_http.h"
#include "../httpengine/base64.h"
#include "../image/astroimage.h"
#include "../image/imagegroup.h"
#include "../unimap_worker.h"
#include "../gui/frame.h"
#include "../config/mainconfig.h"

// main header
#include "unimap_online.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CUnimapOnline::CUnimapOnline( )
{
	int i=0;
	// init vars
	m_bIsLogin = 0;
	m_nLastAppUpdateTime = 0;
	m_bUseCookies = 1;
	m_vectRemoteFileList.clear();
//	m_vectOtherAccounts.clear();

#ifdef _DEBUG
	// test
//	m_strUrl = wxT("http://localhost/unimap/");
	m_strUrl = wxT("http://uni-map.net/");
#else
	// life
	m_strUrl = wxT("http://uni-map.net/");
#endif

	m_pHttp = new wxCurlHTTP();

	// set my handlers vars
	m_pHttp->SetBaseURL( m_strUrl );
	m_pHttp->UseCookies( m_bUseCookies );

	// init profile vars
	m_strUsername = wxT("");
	memset( &m_strPassword, 0, 255 );
	m_strEmail = wxT("");
	m_strNickname = wxT("");
	m_nUseSecure = 0;

	// updates
	m_bAppUpdates = 0;
	m_vectAppUpdates.clear();

	/////////////////
	// initialize service accounts - todo: move this to iservices.ini file
	m_nServiceAccounts = 2;
	// meteoblue
	m_vectServiceAccounts[0].id = 0;
	m_vectServiceAccounts[0].name = wxT("MeteoBlue");
	m_vectServiceAccounts[0].reg_page = wxT("http://my.meteoblue.com/my/");
	m_vectServiceAccounts[0].username = wxT("");
	strcpy( m_vectServiceAccounts[0].password, "" );
	m_vectServiceAccounts[0].note = wxT("Note: If you don't have an account please press \"Register\" to get one.");
	// satat
	m_vectServiceAccounts[1].id = 1;
	m_vectServiceAccounts[1].name = wxT("Space Track");
	m_vectServiceAccounts[1].reg_page = wxT("http://www.space-track.org/perl/new_account.pl");
	m_vectServiceAccounts[1].username = wxT("");
	strcpy( m_vectServiceAccounts[1].password, "" );
	m_vectServiceAccounts[1].note = wxT("Note: If you don't have an account please press \"Register\" to get one. Registration might take serveral days.");

	// load configuration
	LoadCfg( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
////////////////////////////////////////////////////////////////////
CUnimapOnline::~CUnimapOnline( ) 
{
	SaveCfg( );
	if( m_pHttp ) delete( m_pHttp ); 
	m_vectRemoteFileList.clear();
//	m_vectOtherAccounts.clear();
}

// Method:	SetConfig
////////////////////////////////////////////////////////////////////
void CUnimapOnline::SetConfig( CGUIFrame* pFrame, CUnimapWorker* pUnimapWorker, CConfigMain* pMainConfig )
{
	m_pFrame = pFrame;
	m_pUnimapWorker = pUnimapWorker;
	m_pMainConfig = pMainConfig;
}

// Method:	Connect
////////////////////////////////////////////////////////////////////
int CUnimapOnline::Connect( ) 
{
	return( 1 );
}

// Method:	CheckUsername
////////////////////////////////////////////////////////////////////
int CUnimapOnline::CheckAppUpdates( ) 
{
	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxRegEx reAppUpdate( wxT("^TYPE\\{(.*?)\\}DTIME\\{(.*?)\\}TITLE\\{(.*?)\\}DESC\\{(.*?)\\}"), wxRE_ADVANCED );
	wxString strPostData = wxT("");
	wxString strResponse=wxT("");
	strPostData.sprintf( wxT("username=%s&appid=%s"), m_strUsername, m_pMainConfig->m_strAppId );
	int nPostData = strPostData.Len()*sizeof(char);

	// set proper url call
	wxString strUrl = wxT("");
	strUrl.Printf( wxT("%s%s"), m_strUrl, wxT("bin/check_updates.php") );
	m_pHttp->SetBaseURL( strUrl );

	// reset updates to zero
	m_bAppUpdates = 0;
	m_vectAppUpdates.clear();

	// post and check result
	if( m_pHttp->Post( strPostData.ToAscii(), nPostData ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	///////////////////
	// here we go to process the response
	if( bStatus )
	{
		// set last time check
		m_nLastAppUpdateTime = time(NULL);

		// look for token | then check for key 
		wxStringTokenizer tkz( strResponse, wxT("|") );
		while ( tkz.HasMoreTokens() )
		{
			wxString strElem = tkz.GetNextToken();
			// now check if right 
			if( reAppUpdate.Matches( strElem ) )
			{
				DefAppUpdate elem;
				// ids
				elem.type = wxAtoi( reAppUpdate.GetMatch( strElem, 1 ) );
				elem.title = reAppUpdate.GetMatch( strElem, 3 );
				elem.desc = reAppUpdate.GetMatch( strElem, 4 );
				// date/time
				elem.datetime = (time_t) wxAtof( reAppUpdate.GetMatch( strElem, 2 ) );

				m_vectAppUpdates.push_back( elem );
			}
		}
		// check if updates
		if( m_vectAppUpdates.size() ) m_bAppUpdates = 1;
	}

	return( bStatus );
}

// Method:	CheckUsername
////////////////////////////////////////////////////////////////////
int CUnimapOnline::CheckUsername( const wxString& strUsername ) 
{
	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxString strPostData = wxT("");
	strPostData.sprintf( wxT("username=%s"), strUsername );
	int nPostData = strPostData.Len()*sizeof(char);

	// post and check result
	if( m_pHttp->Post( strPostData.ToAscii(), nPostData ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

// Method:	Login
////////////////////////////////////////////////////////////////////
int CUnimapOnline::Login( )
{
	return( LoginT( m_strUsername, wxString(m_strPassword,wxConvUTF8) ) );
}

// Method:	Login with arguments
////////////////////////////////////////////////////////////////////
int CUnimapOnline::LoginT( wxString strUsername, wxString strPassword )
{
	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxString strPostData = wxT("");
	strPostData.sprintf( wxT("username=%s&password=%s"), strUsername, strPassword );
	int nPostData = strPostData.Len()*sizeof(char);

	// set proper url call
	wxString strUrl = wxT("");
	strUrl.Printf( wxT("%s%s"), m_strUrl, wxT("bin/login.php") );
	m_pHttp->SetBaseURL( strUrl );

//	m_pHttp->SetBaseURL( m_strUrl );

	// post and check result
	if( m_pHttp->Post( strPostData.ToAscii(), nPostData ) )
//	if( m_pHttp->PostDF( vectFields,  wxT("bin/login.php") ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}
	// set satus of login
	m_bIsLogin = bStatus;

	return( bStatus );
}

// Method:	CreateAccount
////////////////////////////////////////////////////////////////////
int CUnimapOnline::CreateAccount( const wxString& strUsername, const wxString& strPassword, 
								 const wxString& strEmail, const wxString& strNickname )
{
	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxString strPostData = wxT("");
	if( strNickname != wxT("") )
		strPostData.sprintf( wxT("username=%s&password=%s&email=%s&nickname=%s"), strUsername, strPassword, strEmail, strNickname );
	else
		strPostData.sprintf( wxT("username=%s&password=%s&email=%s"), strUsername, strPassword, strEmail );
	int nPostData = strPostData.Len()*sizeof(char);

	// set proper url call
	wxString strUrl = wxT("");
	strUrl.sprintf( wxT("%s%s"), m_strUrl, wxT("bin/register.php") );
	m_pHttp->SetBaseURL( strUrl );
	// post and check result
	if( m_pHttp->Post( strPostData.ToAscii(), nPostData ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

// Method:	SaveCfg
////////////////////////////////////////////////////////////////////
int CUnimapOnline::SaveCfg( ) 
{
	FILE* pFile = NULL;
	char strTmp[255];
	int i=0;

	// open local file to write
	pFile = wxFopen( UNIMAP_ONLINE_CONFIG_FILE, wxT("w") );
	if( !pFile ) return( 0 );

	fprintf( pFile, "\n" );

	/////////////
	// SAVE :: PROFILE
	fprintf( pFile, "[Profile]\n" );
	wxFprintf( pFile, wxT("Username=%s\n"), m_strUsername );

	//wxString strWxPassword(m_strPassword,wxConvUTF8);
	TEncryptString( m_strPassword, "j01Wqt3", strTmp );
	fprintf( pFile, "Password=%s\n", strTmp );

	fprintf( pFile, "UseSecure=%d\n", m_nUseSecure );
	wxFprintf( pFile, wxT("Email=%s\n"), m_strEmail );
	wxFprintf( pFile, wxT("Nickname=%s\n"), m_strNickname );
	fprintf( pFile, "\n" );

	/////////////
	// SAVE :: Updates
	fprintf( pFile, "[Updates]\n" );
	wxFprintf( pFile, wxT("LastCheck=%lu\n"), (unsigned long) m_nLastAppUpdateTime );
	fprintf( pFile, "\n" );

	//////////////////
	// SAVE OTHER PROFILES/ACCOUNTS - meteo blue, catsat, etc
	for( i=0; i<m_nServiceAccounts; i++ )
	{
		wxFprintf( pFile, wxT("{%s}\n"), m_vectServiceAccounts[i].name );
		wxFprintf( pFile, wxT("Username=%s\n"), m_vectServiceAccounts[i].username );
		TEncryptString( m_vectServiceAccounts[i].password, "j01Wqt3", strTmp );
		fprintf( pFile, "Password=%s\n", strTmp );
		fprintf( pFile, "\n" );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}

// Method:	LoadCfg
////////////////////////////////////////////////////////////////////
int CUnimapOnline::LoadCfg( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	char str_tmp[255];
	int nSection=0, nLineSize=0, nServiceId=-1;

	// open local file to read from
	pFile = wxFopen( UNIMAP_ONLINE_CONFIG_FILE, wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		memset( strTmp, 0, 2000*sizeof(wxChar) );
		memset( strLine, 0, 2000*sizeof(wxChar) );
		memset( str_tmp, 0, 255 );

		// get one line out of the config file
		wxFgets( strLine, 1000, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

//		wxString strWxLine(strLine,wxConvUTF8);

		// check section
		if( wxStrncmp( strLine, wxT("[Profile]"), 9 ) == 0 )
		{
			nSection = UNIMAP_ONLINE_CFG_FILE_SECTION_PROFILE;
			continue;
	
		/////////////////////
		// check if updates section
		} else if( wxStrncmp( strLine, wxT("[Updates]"), 9 ) == 0 )
		{
			nSection = UNIMAP_ONLINE_CFG_FILE_SECTION_UPDATES;
			continue;

		/////////////////////
		// check if service account
		} else if( strLine[0] == '{' )
		{
			// check fi name set
			if( wxSscanf( strLine, wxT("\{%[^\}]\}\n"), strTmp ) )
			{
				wxString strName = strTmp;
				nServiceId = GetServiceId( strName );
				if( nServiceId >= 0 ) nSection = UNIMAP_ONLINE_CFG_FILE_SECTION_SERVICE;
			}

		////////////////////
		// SECTION :: PROFILE
		} else if( nSection && nSection == UNIMAP_ONLINE_CFG_FILE_SECTION_PROFILE )
		{
			if( wxStrncmp( strLine, wxT("Username="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("Username=%[^\n]\n"), strTmp );
				m_strUsername = strTmp;

			} else if( wxStrncmp( strLine, wxT("Password="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("Password=%[^\n]\n"), strTmp );

				wxString strWxTmp = strTmp;
				TDecryptString( strWxTmp.ToAscii(), "j01Wqt3", m_strPassword );

			} else if( wxStrncmp( strLine, wxT("Email="), 6 ) == 0 )
			{
				wxSscanf( strLine, wxT("Email=%[^\n]\n"), strTmp );
				m_strEmail = strTmp;

			} else if( wxStrncmp( strLine, wxT("Nickname="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("Nickname=%[^\n]\n"), strTmp );
				m_strNickname = strTmp;

			} else if( wxStrncmp( strLine, wxT("UseSecure="), 10 ) == 0 )
			{
				wxSscanf( strLine, wxT("UseSecure=%d\n"), &m_nUseSecure );
			}

		////////////////////
		// SECTION :: SERVICE
		} else if( nSection && nSection == UNIMAP_ONLINE_CFG_FILE_SECTION_UPDATES )
		{
			if( wxStrncmp( strLine, wxT("LastCheck="), 10 ) == 0 )
			{
				unsigned long last_time;
				wxSscanf( strLine, wxT("LastCheck=%lu\n"), &last_time );
				m_nLastAppUpdateTime = (time_t) last_time;

			}

		////////////////////
		// SECTION :: SERVICE
		} else if( nSection && nSection == UNIMAP_ONLINE_CFG_FILE_SECTION_SERVICE && nServiceId >= 0 )
		{
			if( wxStrncmp( strLine, wxT("Username="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("Username=%[^\n]\n"), strTmp );
				m_vectServiceAccounts[nServiceId].username = strTmp;

			} else if( wxStrncmp( strLine, wxT("Password="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("Password=%[^\n]\n"), strTmp );
				wxString strWxTmp = strTmp;
				TDecryptString( strWxTmp.ToAscii(), "j01Wqt3", m_vectServiceAccounts[nServiceId].password );
			}
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

// Method:	UploadGroupImageFile
////////////////////////////////////////////////////////////////////
int CUnimapOnline::UploadGroupImageFile( const wxString& strFile, const wxString& strGroupName,
									const wxString& strBaseImageName, const wxString& strFileName, int nType )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();
	// define element
	DefFormField field;
	// :: set group
	field.field_name = wxT("group");
	field.field_value = strGroupName;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: baseimage
	field.field_name = wxT("baseimage");
	field.field_value = strBaseImageName;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set group
	field.field_name = wxT("ftype");
	field.field_value.Printf( wxT("%d"), nType );
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set file
	field.field_name = wxT("upfile");
	field.field_value = strFile;
	field.field_type = CURLFORM_FILE;
	vectFields.push_back( field );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/upload.php") ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

// Method:	SetGroupImageDesc
////////////////////////////////////////////////////////////////////
int CUnimapOnline::SetGroupImageDesc( const wxString& strGroupName, const wxString& strBaseImageName, 
									const wxString& strTitle, const wxString& strDesc )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();
	// define element
	DefFormField field;
	// :: set group
	field.field_name = wxT("group");
	field.field_value = strGroupName;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: baseimage
	field.field_name = wxT("baseimage");
	field.field_value = strBaseImageName;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set title
	field.field_name = wxT("ititle");
	field.field_value = strTitle;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set description
	field.field_name = wxT("idesc");
	field.field_value = strDesc;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/setimgdesc.php") ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	UploadGroupFile
////////////////////////////////////////////////////////////////////
int CUnimapOnline::UploadGroupFile( const wxString& strFile, const wxString& strGroupName,
									const wxString& strFileName, int nType )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();
	// define element
	DefFormField field;
	// :: set group
	field.field_name = wxT("group");
	field.field_value = strGroupName;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set group
	field.field_name = wxT("ftype");
	field.field_value.Printf( wxT("%d"), nType );
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set file
	field.field_name = wxT("upfile");
	field.field_value = strFile;
	field.field_type = CURLFORM_FILE;
	vectFields.push_back( field );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/gupload.php") ) )
	{
		// get result
		wxString strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	GetFileList
////////////////////////////////////////////////////////////////////
int CUnimapOnline::GetFileList( int nMaxNo, int nSortCol, int bSortCol, 
							   const wxString& strKey, int nGui, wxEvtHandler* pEvtHandler )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) )
	{
		m_pUnimapWorker->DisplayNotification( wxT("UniMap Online: Cannot login! \nCheck your account in options!"), 0 );
		return( 0 );
	}

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxRegEx reListElem( wxT("^LELM\\{([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( pEvtHandler, 1 );
	else
		SetHandler( NULL, 0 );

	// we empty file list at the begining 
	m_vectRemoteFileList.clear();

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();
	// define element
	DefFormField field;
	// :: set fetch max number
	field.field_name = wxT("maxno");
	field.field_value.Printf( wxT("%d"), nMaxNo );
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set sort column
	field.field_name = wxT("scol");
	field.field_value.Printf( wxT("%d"), nSortCol );
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set sort column order
	field.field_name = wxT("socol");
	field.field_value.Printf( wxT("%d"), bSortCol );
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );
	// :: set search key
	field.field_name = wxT("key");
	field.field_value = strKey;
	field.field_type = CURLFORM_COPYCONTENTS;
	vectFields.push_back( field );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Fetching image list...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/getimglist.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}

	} else
		bStatus = 0;

	///////////////////
	// here we go to process the response
	if( bStatus )
	{
		// look for token | then check for key LELM()
		wxStringTokenizer tkz( strResponse, wxT("|") );
		while ( tkz.HasMoreTokens() )
		{
			wxString strElem = tkz.GetNextToken();
			// now check if right 
			if( reListElem.Matches( strElem ) )
			{
				DefRemoteFileList elem;
				// ids
				elem.nUserId = (long) atol( reListElem.GetMatch( strElem, 1 ).ToAscii() );
				elem.nGroupId = (long) atol( reListElem.GetMatch( strElem, 2 ).ToAscii() );
				elem.nImageId = (long) atol( reListElem.GetMatch( strElem, 3 ).ToAscii() );
				// other
				elem.strUser = reListElem.GetMatch( strElem, 4 );
				elem.strGroup = reListElem.GetMatch( strElem, 5 );
				elem.strImage = reListElem.GetMatch( strElem, 6 );
				elem.strIni = reListElem.GetMatch( strElem, 7 );
				elem.strPreview = reListElem.GetMatch( strElem, 8 );
				// score
				elem.score_q = (unsigned char) atoi( reListElem.GetMatch( strElem, 9 ).ToAscii() );
				elem.score_a = (unsigned char) atoi( reListElem.GetMatch( strElem, 10 ).ToAscii() );
				elem.score_s = (unsigned char) atoi( reListElem.GetMatch( strElem, 11 ).ToAscii() );
				// date/time
				elem.datetime = (time_t) atof( reListElem.GetMatch( strElem, 12 ).ToAscii() );
				elem.nComments = atoi( reListElem.GetMatch( strElem, 13 ).ToAscii() );

				m_vectRemoteFileList.push_back( elem );
			}
		}

	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	GetPreviewIcon
////////////////////////////////////////////////////////////////////
int CUnimapOnline::GetPreviewIconFile( const wxString& strUser, const wxString& strGroup, 
										const wxString& strFile, int nGui, wxEvtHandler* pEvtHandler )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strLocalFolder;
	wxString strLocalFile;
	wxString strOnlineFile;
	wxString strMsg;

	// set handler for progress bar
	if( nGui ) 
		SetHandler( pEvtHandler, 1 );
	else
		SetHandler( NULL, 0 );

	// file local
	strLocalFolder.Printf( unMakeAppPath(wxT("data/temp/uonline/preview_icons/%s")), strGroup );
	if( !wxDirExists( strLocalFolder ) ) wxMkdir( strLocalFolder );

	// set url file
	if( nGui )
	{
		strMsg.Printf( wxT("Downloading preview for %s ..."), strFile );
		m_pUnimapWorker->SetWaitDialogMsg( strMsg );
	}
	strLocalFile.Printf( wxT("%s/%s"), strLocalFolder, strFile );
	//strOnlineFile.Printf( wxT("uploads/previews/%s/%s/%s"), URLEncode(strUser), URLEncode(strGroup), URLEncode(strFile) );
	strOnlineFile.Printf( wxT("bin/getimg.php?type=preview&user=%s&group=%s&img=%s"), URLEncode(strUser), URLEncode(strGroup), URLEncode(strFile) );
	m_pHttp->SetBaseURL( m_strUrl );
	// get file
	if( m_pHttp->Get( strLocalFile, strOnlineFile ) )
	{
		bStatus = 1;
	} else
		bStatus = 0;

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	GetImageData
////////////////////////////////////////////////////////////////////
int CUnimapOnline::GetImageData( const wxString& strPath, const wxString& strUser, 
								const wxString& strGroup, const wxString& strBaseFile, 
								const wxString& strIniFile, int nGui, wxEvtHandler* pEvtHandler )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strLocalFolder;
	wxString strLocalFile;
	wxString strOnlineFile;
	wxString strMsg;

	// set handler for progress bar
	if( nGui ) 
		SetHandler( pEvtHandler, 1 );
	else
		SetHandler( NULL, 0 );

	// set refer url
	m_pHttp->SetBaseURL( m_strUrl );

	// set file local & check if folder exist or create
	strLocalFolder.Printf( wxT("%s/%s"), strPath, strGroup );
	if( !wxDirExists( strLocalFolder ) ) wxMkdir( strLocalFolder );

	///////////////////
	// set base
	if( nGui )
	{
		strMsg.Printf( wxT("Downloading image %s ..."), strBaseFile );
		m_pUnimapWorker->SetWaitDialogMsg( strMsg );
	}
	strLocalFile.Printf( wxT("%s/%s"), strLocalFolder, strBaseFile );
	strOnlineFile.Printf( wxT("uploads/base/%s/%s/%s"), strUser, strGroup, strBaseFile );
	// get base file
	bStatus = m_pHttp->Get( strLocalFile, strOnlineFile );

	///////////////////
	// set ini - if set
	if( bStatus && !strIniFile.IsEmpty() )
	{
		// do the gui thing
		if( nGui )
		{
			strMsg.Printf( wxT("Downloading data %s ..."), strIniFile );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
		}

		// set local/remote file
		strLocalFile.Printf( wxT("%s/%s"), strLocalFolder, strIniFile );
		//strOnlineFile.Printf( wxT("uploads/base/%s/%s/%s"), strUser, strGroup, strIniFile );
		strOnlineFile.Printf( wxT("bin/getimg?type=base&user=%s&group=%s&img=%s"), strUser, strGroup, strIniFile );
		// get ini file
		bStatus = m_pHttp->Get( strLocalFile, strOnlineFile );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	UploadAstroImage
////////////////////////////////////////////////////////////////////
int CUnimapOnline::UploadAstroImage( CAstroImage* pAstroImage, int nGui, wxEvtHandler* pEvtHandler )
{
	if( !pAstroImage ) return( 0 );

	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) )
	{
		m_pUnimapWorker->DisplayNotification( wxT("UniMap Online: Cannot login! \nCheck your account in options!"), 0 );
		return( 0 );
	}

	int bStatus=0;
	wxString strMsg=wxT("");
	wxString strFilePath=wxT("");
	wxString strFile=wxT("");
	CImageGroup* pGroup = pAstroImage->m_pImageGroup;

	// set handler for progress bar
	if( nGui ) 
		SetHandler( pEvtHandler, 1 );
	else
		SetHandler( NULL, 0 );

	//////////////
	// call to upload current image/with current group
	wxString strImgFile = pAstroImage->GetImageName();
	// if unimap then log
	if( nGui )
	{
		strMsg.Printf( wxT("Uploading Image %s ..."), strFile );
		m_pUnimapWorker->SetWaitDialogMsg( strMsg );
	}
	// upload file
	bStatus = UploadGroupImageFile( pAstroImage->GetFullName(), pGroup->m_strGroupNameId, strImgFile, strImgFile, UONLINE_UPLOADED_FILE_TYPE_IMAGE );
	// set image description
	if( !pAstroImage->m_strTargetName.IsEmpty() || !pAstroImage->m_strTargetDesc.IsEmpty() )
		SetGroupImageDesc( pGroup->m_strGroupNameId, strImgFile, pAstroImage->m_strTargetName, pAstroImage->m_strTargetDesc );

	///////////////
	// check upload ini file if exists
	strFilePath = pAstroImage->GetIniFilePath();
	if( wxFileExists( strFilePath ) )
	{
		strFile = pAstroImage->GetIniFileName();
		if( nGui )
		{
			strMsg.Printf( wxT("Uploading Profile %s ..."), strFile );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
		}
		// upload file
		bStatus = UploadGroupImageFile( strFilePath, pGroup->m_strGroupNameId, strImgFile, strFile, UONLINE_UPLOADED_FILE_TYPE_IMAGE_INI );
	}

	////////////////
	// THUMBNAIL ICON :: generate and upload 
	if( CreateOnlineThumbIcon( pAstroImage, strFilePath, strFile, 1 ) )
	{
		if( nGui )
		{
			strMsg.Printf( wxT("Uploading thumbnail image %s ..."), strFile );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
		}
		// upload file
		bStatus = UploadGroupImageFile( strFilePath, pGroup->m_strGroupNameId, strImgFile, strFile, UONLINE_UPLOADED_FILE_TYPE_IMAGE_ICON );
	}

	////////////////
	// PREVIEW ICON :: generate and upload 
	if( CreateOnlinePreviewIcon( pAstroImage, strFilePath, strFile, 1 ) )
	{
		if( nGui )
		{
			strMsg.Printf( wxT("Uploading preview image %s ..."), strFile );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
		}
		// upload file
		bStatus = UploadGroupImageFile( strFilePath, pGroup->m_strGroupNameId, 
										strImgFile, strFile, UONLINE_UPLOADED_FILE_TYPE_IMAGE_PREVIEW );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	UploadImageGroup
////////////////////////////////////////////////////////////////////
int CUnimapOnline::UploadImageGroup( CImageGroup* pGroup, int nGui, wxEvtHandler* pEvtHandler )
{
	if( !pGroup ) return( 0 );

	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) )
	{
		m_pUnimapWorker->DisplayNotification( wxT("UniMap Online: Cannot login! \nCheck your account in options!"), 0 );
		return( 0 );
	}

	int bStatus=0, i=0;
	wxString strMsg;

	// set handler for progress bar
	if( nGui ) 
		SetHandler( pEvtHandler, 1 );
	else
		SetHandler( NULL, 0 );

	// for all images in the group do upload ... one by one
	for( i=0; i<pGroup->m_nImageNode; i++ )
	{
		// upload my image 
		bStatus = UploadAstroImage( pGroup->m_vectImageNode[i], nGui );
	}

	//////////////////////
	// NOW HERE WE UPLOAD THE GROUP INI FILE
	// TODO :: regenerate this with the files i have and/or use different to store files ?
	wxString strIniFile = pGroup->GetIniFilePath();
	if( wxFileExists( strIniFile ) )
	{
		wxString strFile = pGroup->GetIniFileName();
		if( nGui )
		{
			strMsg.Printf( wxT("Uploading Profile %s ..."), strFile );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
		}
		// upload file - todo ... here use a different funtion ? or param
		bStatus = UploadGroupFile( strIniFile, pGroup->m_strGroupNameId, strFile );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	CreateOnlineThumbIcon
////////////////////////////////////////////////////////////////////
int CUnimapOnline::CreateOnlineThumbIcon( CAstroImage* pAstroImage, wxString& strFullPath, 
										 wxString& strFile, int bForce )
{
	wxString strPathToImgGroup;

	// :: first set the folder with the group
	strPathToImgGroup.Printf( wxT("%s\/%s"), UNIMAP_OLNINE_TMP_FOLDER_THUMB_ICONS, pAstroImage->m_pImageGroup->m_strGroupNameId );
	// :: check if exists
	if( !wxDirExists( strPathToImgGroup ) ) wxMkdir( strPathToImgGroup );

	strFile.Printf( wxT("%s_icon.jpg"), pAstroImage->m_strImageName );
	strFullPath.Printf( wxT("%s\/%s"), strPathToImgGroup, strFile );

	// if not force - try to see if already exists
	if( !bForce && wxFileExists( strFullPath ) ) return( 1 );

	// if not create
	pAstroImage->CreateThumbnailCustom( strFullPath, UNIMAP_OLNINE_ICON_IMAGE_WIDTH, UNIMAP_OLNINE_ICON_IMAGE_HEIGHT, 1 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	CreateOnlinePreviewIcon
////////////////////////////////////////////////////////////////////
int CUnimapOnline::CreateOnlinePreviewIcon( CAstroImage* pAstroImage, wxString& strFullPath, 
											wxString& strFile, int bForce )
{
	wxString strPathToImgGroup;

	// :: first set the folder with the group
	strPathToImgGroup.Printf( wxT("%s\/%s"), UNIMAP_OLNINE_TMP_FOLDER_PREVIEW_ICONS, pAstroImage->m_pImageGroup->m_strGroupNameId );
	// :: check if exists
	if( !wxDirExists( strPathToImgGroup ) ) wxMkdir( strPathToImgGroup );

	strFile.sprintf( wxT("%s_preview.jpg"), pAstroImage->m_strImageName );
	strFullPath.sprintf( wxT("%s\/%s"), strPathToImgGroup, strFile );

	// if not force - try to see if already exists
	if( !bForce && wxFileExists( strFullPath ) ) return( 1 );

	// if not create
	pAstroImage->CreateThumbnailCustom( strFullPath, UNIMAP_OLNINE_PREVIEW_IMAGE_WIDTH, UNIMAP_OLNINE_PREVIEW_IMAGE_HEIGHT, 1 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	RateAstroImage
////////////////////////////////////////////////////////////////////
int CUnimapOnline::RateAstroImage( long imid, int scoreq, int scorea, int scores, int nGui )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( m_pFrame, 1 );
	else
		SetHandler( NULL, 0 );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();

	// populate my post form
	vectFields.push_back( _MakewxCurlFormField( "imid", imid, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "scoreq", (long) scoreq, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "scorea", (long) scorea, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "scores", (long) scores, CURLFORM_COPYCONTENTS  )  );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Post image score ...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/scoreimg.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	PostComment
////////////////////////////////////////////////////////////////////
int CUnimapOnline::PostComment( long imid, wxString& strTitle, wxString& strMsg, int nGui )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( m_pFrame, 1 );
	else
		SetHandler( NULL, 0 );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();

	// populate my post form
	vectFields.push_back( _MakewxCurlFormField( "imid", imid, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "title", strTitle.ToAscii(), CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "msg", strMsg.ToAscii(), CURLFORM_COPYCONTENTS  )  );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Post image comment ...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/postcmnt.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	GetImageComments
////////////////////////////////////////////////////////////////////
int CUnimapOnline::GetImageComments( int list_id, long imid, int nGui )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );
	wxRegEx reListElem( wxT("^CMNT\\{([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\:([^\\:]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( m_pFrame, 1 );
	else
		SetHandler( NULL, 0 );

	// we empty the image comment list
	m_vectRemoteFileList[list_id].vectComments.clear();

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();

	// populate my post form
	vectFields.push_back( _MakewxCurlFormField( "imid", imid, CURLFORM_COPYCONTENTS  )  );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Get image comments ...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/getcmnt.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	///////////////////
	// here we go to process the response
	if( bStatus )
	{
		// reset back the flag to check if any items
		bStatus = 0;
		// look for token | then check for key CMNT{}
		wxStringTokenizer tkz( strResponse, wxT("|") );
		while ( tkz.HasMoreTokens() )
		{
			bStatus = 1;
			wxString strElem = tkz.GetNextToken();
			// now check if right 
			if( reListElem.Matches( strElem ) )
			{
				DefOnlineImgComment elem;
				// ids
				elem.id = (long) atol( reListElem.GetMatch( strElem, 1 ).ToAscii() );
				elem.title = wxBase64Decode( reListElem.GetMatch( strElem, 2 ) );
				elem.message = wxBase64Decode( reListElem.GetMatch( strElem, 3 ) );
				// date/time
				elem.datetime = (time_t) atof( reListElem.GetMatch( strElem, 4 ).ToAscii() );
				m_vectRemoteFileList[list_id].vectComments.push_back( elem );
			}
		}

	}

	return( bStatus );
}

// get service id based on name or -1 on failure
////////////////////////////////////////////////////////////////////
int CUnimapOnline::GetServiceId( wxString strName )
{
	int i=0, nId=-1;
	for( i=0; i<m_nServiceAccounts; i++ )
	{
		if( strName.CmpNoCase( m_vectServiceAccounts[i].name ) == 0 ) nId = i;
	}
	return( nId );
}

////////////////////////////////////////////////////////////////////
// Method:	DeleteImage
////////////////////////////////////////////////////////////////////
int CUnimapOnline::DeleteImage( long imid, int nGui )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( m_pFrame, 1 );
	else
		SetHandler( NULL, 0 );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();

	// populate my post form
	vectFields.push_back( _MakewxCurlFormField( "imid", imid, CURLFORM_COPYCONTENTS  )  );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Deleting image ...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/delimage.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	PostFeedback
////////////////////////////////////////////////////////////////////
int CUnimapOnline::PostFeedback( int type, int section_id, wxString& strTitle, wxString& strMsg, int nGui )
{
	// check login and then attemp tp log in or return
	if( !m_bIsLogin ) if( !Login( ) ) return( 0 );

	int bStatus=0;
	wxString strResponse = wxT("");
	wxRegEx reReturnStatus( wxT("RETURN_STATUS=\\{([^\\}]*)\\}") );

	// set handler for progress bar
	if( nGui ) 
		SetHandler( m_pFrame, 1 );
	else
		SetHandler( NULL, 0 );

	// buid form data in vector
	std::vector<DefFormField> vectFields;
	vectFields.clear();

	// populate my post form
	vectFields.push_back( _MakewxCurlFormField( "type", (long) type, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "sid", (long) section_id, CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "title", strTitle.ToAscii(), CURLFORM_COPYCONTENTS  )  );
	vectFields.push_back( _MakewxCurlFormField( "msg", strMsg.ToAscii(), CURLFORM_COPYCONTENTS  )  );

	// gui log
	if( nGui ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Posting user feedback ...") );

	m_pHttp->SetBaseURL( m_strUrl );
	// post and check result
	if( m_pHttp->PostDF( vectFields, wxT("bin/postfeedback.php") ) )
	{
		// get result
		strResponse = m_pHttp->GetResponseBody();
		// remove eol
		strResponse.Replace( wxT("\r\n"), wxT("") );
		strResponse.Replace( wxT("\r"), wxT("") );
		strResponse.Replace( wxT("\n"), wxT("") );

		// verify if result ok
		if( reReturnStatus.Matches( strResponse ) )
		{
			wxString strStatus = reReturnStatus.GetMatch( strResponse, 1 ).Trim(0).Trim(1);
			// check if OK or error
			if( strStatus.CmpNoCase( wxT("OK") ) == 0 )
				bStatus = 1;
			else
				bStatus = 0;
		}
	}

	return( bStatus );
}
