// each image group will be connected to each other with links
// with relative weigths
//
// for example:
//
// group1 - linkto(weigth=x) - group2
// group1 - linkto(weigth=y) - group3
//
// also words can trigger a group activation
// an image activation can trigger a group to change

#ifndef _IMAGE_GROUP_NETWORK
#define _IMAGE_GROUP_NETWORK

// system
#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// local includes
//#include "../util/folders.h"

#define IMAGE_NET_GROUP_LIST_INI_FILE unMakeAppPath(wxT("data/image_db.ini"))
#define IMAGE_DB_SQLITE_FILE unMakeAppPath(wxT("data/image_db_sqlite.dat"))
//#define IMAGE_NET_GROUP_LINK_INI_FILE "image_group_net_link.ini"

#define MAX_IMAGE_GROUP 600

// external classes
class CImageGroup;
class CAstroImage;
class CImageArea;
class CLogger;
class CGUIFrame;
class CUnimapWorker;
class CUnimap;
class CSky;
class CConfigMain;
class CConfigDetect;
class CUnimapDb;

// constellation artwork struct
typedef struct
{
	int group_id;
	int image_id;

} DefImageListRec;

// class : CImageDb
class CImageDb
{
// methods
public:
	CImageDb();
	~CImageDb();

	int Init( );

	int LoadGroups( );
	int SaveGroups( );

	void SetLogger( CLogger* pLogger );
//	CImageGroup* GetGroup( );
	int GetGroupId( CImageGroup* pGroup );
	CAstroImage* GetImage( );

	CAstroImage* FindImageInGroupByName( CImageGroup* pImageGroup, wxString& strImageName );
	CImageGroup* FindImageGroupByName( wxString& strImageGroup );
	int FindGroupIdByName( wxString strImageGroup );

	int SaveAllGroups( );

	int SelectGroup( int nGroupId );
	int CreateGroupThumbnails( int nGroupId );
	int SetCurrentGroupThumbnails( );

	int SelectImageFromGroup( int nGroupId, int nImageId );

	int AddGroup( const wxString& strGroupName, const wxString& strGroupPath=wxT(""), 
					int bInclude=0, int bRecursive=0 );
	int DeleteGroup( int nGroupId );
	int AddImages( const wxString& strName, const wxString& strImages );

	// slide show 
	void InitImageList( int nSource=0, int nOrder=0 );
	int GetNextImageFromList( int& group, int& image );
	int GetPreviousImageFromList( int& group, int& image );

// data
public:

	std::vector<CImageGroup*> m_vectImageGroup;
	int m_nImageGroup;

	// full list of images - use for slideshow
	std::vector<DefImageListRec> m_vectImageList;
	int m_nImageListCurrent;

	CLogger*		m_pLogger;
	CGUIFrame*		m_pFrame;
	CUnimapWorker*	m_pUnimapWorker;
	CUnimap*		m_pUnimap;
	CUnimapDb*		m_pDb;

	CConfigMain* m_pMainConfig;
	CConfigDetect* m_pDetectConfig;
	CSky* m_pSky;

	CAstroImage* m_pCurrentImage;
	CImageGroup* m_pCurrentGroup;
	int	m_nCurrentGroup;

	wxString m_strImageFormatsInput;
	wxString m_strImageFormatsOutput;
};

#endif
