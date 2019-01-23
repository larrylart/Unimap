
#ifndef _UNIMAP_ONLINE_H
#define _UNIMAP_ONLINE_H

// wx includes
#include "wx/wxprec.h"

// define :: configuration file path
#define UNIMAP_ONLINE_CONFIG_FILE					unMakeAppPath(wxT("cfg/online.ini"))
#define UNIMAP_OLNINE_TMP_FOLDER_PREVIEW_ICONS		unMakeAppPath(wxT("data/temp/uonline/preview_icons"))
#define UNIMAP_OLNINE_TMP_FOLDER_THUMB_ICONS		unMakeAppPath(wxT("data/temp/uonline/thumb_icons"))
#define UNIMAP_OLNINE_TMP_FOLDER_ASTRO_IMAGES		unMakeAppPath(wxT("data/temp/uonline/astro_images"))
// define :: standard image sizes
#define UNIMAP_OLNINE_ICON_IMAGE_WIDTH		160
#define UNIMAP_OLNINE_ICON_IMAGE_HEIGHT		120
#define UNIMAP_OLNINE_PREVIEW_IMAGE_WIDTH	320
#define UNIMAP_OLNINE_PREVIEW_IMAGE_HEIGHT	240

// define :: sections ids - starting at 1 enum
enum
{
	UNIMAP_ONLINE_CFG_FILE_SECTION_PROFILE = 1,
	UNIMAP_ONLINE_CFG_FILE_SECTION_SERVICE,
	UNIMAP_ONLINE_CFG_FILE_SECTION_UPDATES
};
// uploaded file types
enum
{
	UONLINE_UPLOADED_FILE_TYPE_IMAGE = 0,
	UONLINE_UPLOADED_FILE_TYPE_IMAGE_INI = 1,
	UONLINE_UPLOADED_FILE_TYPE_IMAGE_ICON = 2,
	UONLINE_UPLOADED_FILE_TYPE_IMAGE_PREVIEW = 3
};

// local includes
#include "../httpengine/wxcurl_http.h"

// struct :: remote file list
typedef struct
{
	int type;
	time_t datetime;
	wxString title;
	wxString desc;

} DefAppUpdate;

// struct :: online accounts
typedef struct
{
	int id;
	wxString name;
	wxString username;
	wxString reg_page;
	char password[255];
	wxString note;

} DefOnlineAccounts;

// struct :: online image comment
typedef struct
{
	long id;
	wxString title;
	wxString message;
	time_t datetime;

} DefOnlineImgComment;

// struct :: remote file list
typedef struct
{
	//ids
	long nUserId;
	long nGroupId;
	long nImageId;
	// strings
	wxString strUser;
	wxString strGroup;
	wxString strImage;
	wxString strIni;
	wxString strPreview;
	// scores
	unsigned char score_q;
	unsigned char score_a;
	unsigned char score_s;
	// date/time unix timestamp
	time_t datetime;
	// number of comments
	int nComments;
	// vector to keep comments
	std::vector<DefOnlineImgComment> vectComments;

} DefRemoteFileList;

// external classes
class CImageGroup;
class CAstroImage;
class CImageGroup;
class CUnimapWorker;
class CGUIFrame;
class CConfigMain;
//class wxCurlHTTP;

// class:	CUnimapOnline
///////////////////////////////////////////////////////
class CUnimapOnline
{
// public methods
public:
	CUnimapOnline( );
	~CUnimapOnline( );

	void SetConfig( CGUIFrame* pFrame, CUnimapWorker* pUnimapWorker, CConfigMain* pMainConfig );
	// configuration save/load handlers
	int SaveCfg( );
	int LoadCfg( );

	// set handler for progress dialog
	void SetHandler( wxEvtHandler* pEvtHandler, const bool& bSendEvts ){ m_pHttp->SetEvtHandler(pEvtHandler); m_pHttp->SendUpdateEvents(bSendEvts); /*m_pHttp->SendBeginEndEvents(bSendEvts);*/ }

	// connect to online site
	int Connect( );
	// login to online site
	int Login( );
	int LoginT( wxString strUsername, wxString strPassword );
	// verify if user name exists
	int CheckUsername( const wxString& strUsername );
	// create a new account
	int CreateAccount( const wxString& strUsername, const wxString& strPassword, const wxString& strEmail, const wxString& strNickname=wxT("") );
	// upload group image file
	int UploadGroupImageFile( const wxString& strFile, const wxString& strGroupName, const wxString& strBaseImageName, const wxString& strFileName, int nType=UONLINE_UPLOADED_FILE_TYPE_IMAGE );
	// set astro image description
	int SetGroupImageDesc( const wxString& strGroupName, const wxString& strBaseImageName, const wxString& strTitle, const wxString& strDesc );
	// upload group file
	int UploadGroupFile( const wxString& strFile, const wxString& strGroupName, const wxString& strFileName, int nType=UONLINE_UPLOADED_FILE_TYPE_IMAGE );
	// upload astroimage
	int UploadAstroImage( CAstroImage* pAstroImage, int nGui=0, wxEvtHandler* pEvtHandler=NULL );
	// upload Image group
	int UploadImageGroup( CImageGroup* pGroup, int nGui=0, wxEvtHandler* pEvtHandler=NULL );
	// get file list
	int GetFileList( int nMaxNo, int nSortCol, int bSortCol, const wxString& strKey, int nGui=0, wxEvtHandler* pEvtHandler=NULL );
	// get preview icon
	int GetPreviewIconFile( const wxString& strUser, const wxString& strGroup, 
							const wxString& strFile, int nGui=0, wxEvtHandler* pEvtHandler=NULL );
	// get image
	int GetImageData( const wxString& strPath, const wxString& strUser, const wxString& strGroup, 
						const wxString& strBaseFile, const wxString& strIniFile, 
						int nGui=0, wxEvtHandler* pEvtHandler=NULL );
	// rate astro image
	int RateAstroImage( long imid, int scoreq, int scorea, int scores, int nGui=0 );
	// post comment
	int PostComment( long imid, wxString& strTitle, wxString& strMsg, int nGui=0 );
	// get image comments
	int GetImageComments( int list_id, long imid, int nGui );
	// delete image
	int DeleteImage( long imid, int nGui );

	////////
	// actions local
	int CreateOnlineThumbIcon( CAstroImage* pAstroImage, wxString& strFullPath, wxString& strFile, int bForce=0 );
	int CreateOnlinePreviewIcon( CAstroImage* pAstroImage, wxString& strFullPath, wxString& strFile, int bForce=0 );

	// service account handlers
	int GetServiceId( wxString strName );

	// post user feedback
	int PostFeedback( int type, int section_id, wxString& strTitle, wxString& strMsg, int nGui=0 );

	// check application updates
	int CheckAppUpdates();

// public data
public:
	// base objects
	CGUIFrame* m_pFrame;
	CUnimapWorker* m_pUnimapWorker;
	CConfigMain*	m_pMainConfig;

	// app updates
	time_t m_nLastAppUpdateTime;

	wxString m_strUrl;
	// main curl request object
	wxCurlHTTP* m_pHttp;
	// other vars
	bool m_bUseCookies;
	bool m_bIsLogin;

	// file upload
	wxString m_strUploadFileName;
	wxString m_strUploadFilePath;
	wxString m_strUploadGroupName;
	wxString m_strUploadGroupPath;
	// pointer
	CImageGroup*	m_pUploadImageGroup;
	CAstroImage*	m_pUploadAstroImage;
	// remote file lists
	std::vector<DefRemoteFileList>	m_vectRemoteFileList;

	// other accounts
//	std::vector<DefOnlineAccounts> m_vectServiceAccounts;

	///////////////
	// profile vars
	wxString m_strUsername;
	char m_strPassword[255];
	wxString m_strEmail;
	wxString m_strNickname;
	int m_nUseSecure;

	// updates vars
	int m_bAppUpdates;
	std::vector<DefAppUpdate> m_vectAppUpdates;

	// hardcode define aline aggounts globaly
	DefOnlineAccounts m_vectServiceAccounts[10];
	int m_nServiceAccounts;

};

#endif
