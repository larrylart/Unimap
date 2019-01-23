
//system headers
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <time.h>

// opencv includes
//#include "highgui.h"
//#include "cv.h"
//#include "cvaux.h"

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tokenzr.h>

// elynx headers
#include <elx/image/ImageFileManager.h>

// local includes
#include "astroimage.h"
//#include "imagearea.h"
#include "imagegroup.h"
#include "../util/func.h"
#include "../util/image_func.h"
#include "../util/folders.h"
#include "../logger/logger.h"
#include "../gui/frame.h"
#include "../unimap_worker.h"
#include "../unimap.h"
#include "../sky/sky.h"
#include "../config/mainconfig.h"
#include "../config/detectconfig.h"
#include "../db/unimap_db.h"

// local header
#include "imagedb.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CImageDb::CImageDb( )
{
	m_nImageGroup = 0;
	m_nCurrentGroup = 0;
	m_pCurrentImage = NULL;
	m_pUnimapWorker = NULL;
	m_pUnimap = NULL;
	m_vectImageGroup.clear();
	m_vectImageList.clear();

	// from elynx - get supported types
//	const char * prIn = the_ImageFileManager.GetInputFileFormatList( true );
//	m_strImageFormatsInput = wxString::FromAscii( prIn );

	m_strImageFormatsInput = GetSupportedImageReadTypes();

	// and output
//	const char * prOut = the_ImageFileManager.GetOutputFileFormatList( true );
//	m_strImageFormatsOutput = wxString::FromAscii( prOut );

	m_strImageFormatsOutput = GetSupportedImageWriteTypes();

	// initialize image database
	m_pDb = new CUnimapDb( IMAGE_DB_SQLITE_FILE );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CImageDb::~CImageDb()
{	
	int i = 0;

	SaveGroups( );

	for( i=0; i<m_nImageGroup; i++ )
	{
		delete( m_vectImageGroup[i] );
	}
	m_vectImageGroup.clear();
	m_vectImageList.clear();

	delete( m_pDb );
}

////////////////////////////////////////////////////////////////////
// Method:		SaveGroups
// Class:		CImageDb
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::SaveGroups( )
{	
	int i = 0;
	FILE* pFile = NULL;
	wxString strFile;

	strFile.Printf( wxT("%s"), IMAGE_NET_GROUP_LIST_INI_FILE );

	// open file to write
	pFile = wxFopen( strFile, wxT("w") );
	// check if there is any file to save
	if( !pFile ) return( 0 );

	for( i=0; i<m_nImageGroup; i++ )
	{
		// save first group label
		wxFprintf( pFile, wxT("[%s]\n"), m_vectImageGroup[i]->m_strGroupNameId );
		// save group path
		wxFprintf( pFile, wxT("path=%s\n"), m_vectImageGroup[i]->m_strGroupPath );
		// save no of images
		fprintf( pFile, "no=%d\n", (int) m_vectImageGroup[i]->m_vectImageNode.size() );

		// add new line at the end of group definition
		fprintf( pFile, "\n" );

	}

	// close my file handler
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SaveAllGroups
// Class:		CImageDb
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::SaveAllGroups( )
{	
	int i = 0;

	for( i=0; i<m_nImageGroup; i++ )
	{
//		m_vectImageGroup[i]->SaveAreas( );
		m_vectImageGroup[i]->Save( );
	}
	// clear list
	
	

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Init
// Class:		CImageDb
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::Init( )
{	
	// debug
//	int n = sizeof( CImageDb );
//	n = sizeof( CImageGroup );
//	n = sizeof( CAstroImage );

	int i = 0;

	if( LoadGroups( ) <= 0 ) return( 0 );

	//int nt = 0;
	for( i=0; i<m_vectImageGroup.size(); i++ )
	{
		if( m_vectImageGroup[i] != NULL ) m_vectImageGroup[i]->Init( );

		//nt += sizeof( CAstroImage )*m_vectImageGroup[i]->m_nImageNode;
	}

	// now here set frame groups and images
	// set first group display
	m_pFrame->m_pGroupSelect->SetSelection( 0 );
	SelectGroup( 0 );
	m_pFrame->m_pImageSelect->SetSelection( 0 );

	//////////////////////////////////////////
	// check group tumbnails
	if( m_pFrame->m_pMainConfig->m_bShowImageThumbs ) 
	{
//		int nSelect = m_pFrame->m_pGroupSelect->GetSelection();
//		SelectGroup( nSelect );

		CreateGroupThumbnails( 0 );
		// now set worker thread
		DefCmdAction cmd;
		cmd.id = THREAD_ACTION_SET_GROUP_THUMBNAILS;
		m_pFrame->m_pUnimapWorker->SetAction( cmd );
		// set init flag
		m_pFrame->m_bGroupThumbnailsInitialized = 1;		
	}

//	SelectImageFromGroup( 0, 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SetLogger
// Class:		CImageDb
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageDb::SetLogger( CLogger* pLogger )
{	
	m_pLogger = pLogger;

}

////////////////////////////////////////////////////////////////////
// Method:		LoadGroups
// Class:		CImageDb
// Purpose:		load my group configuration
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::LoadGroups( )
{	
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nLineSize=0;
	int bFoundGroup = 0;

//	wxRegEx reGroupLabel = wxT( "^\\[([a-zA-Z0-9\\_\\.[:punct:]]+)\\]" );
//	wxRegEx reGroupPath = wxT( "^path=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx reGroupName = wxT( "^actor=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx reGroupNo = wxT( "^no=([0-9]+)" );

	// IMAGE_NET_GROUP_LIST_INI_FILE;
	pFile = wxFopen( IMAGE_NET_GROUP_LIST_INI_FILE, wxT("r") );

	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
//		bzero( strLine, 255 );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 3 || strLine[0] == '#' ) continue;

		// check section
		//if( reSectionLabel.Matches( strLine ) )
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundGroup = 0;
			// get group name id
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strTmp );

			// get group name id
			wxString strGroupNameId = strTmp; //reGroupLabel.GetMatch(strLine, 1 );

			// add image group
			AddGroup( strGroupNameId );

			bFoundGroup = 1;

		} else if( bFoundGroup == 1 && wxStrncmp( strLine, wxT("path="), 5 ) == 0 )
		{
			// get path
			wxSscanf( strLine, wxT("path=%[^\n]\n"), strTmp );
			wxString strGroupPath = strTmp; //reGroupPath.GetMatch(strLine, 1 );

			m_vectImageGroup[m_nImageGroup-1]->SetPath( strGroupPath );

		} else if( bFoundGroup == 1 && wxStrncmp( strLine, wxT("actor="), 6 ) == 0 )
		{
			// get actor
			wxSscanf( strLine, wxT("actor=%[^\n]\n"), strTmp );
			wxString strGroupNameActor = strTmp; //reGroupName.GetMatch(strLine, 1 );
//			m_vectImageGroup[m_nImageGroup-1]->SetActorName( strGroupNameActor.GetData() );

		} else if( bFoundGroup == 1 && wxStrncmp( strLine, wxT("no="), 3 ) == 0 )
		{
			// get no???
			wxSscanf( strLine, wxT("no=%[^\n]\n"), strTmp );
			wxString strGroupImagesNo = strTmp;
//			m_vectImageGroup[m_nImageGroup-1]->SetMaxNoOfImages( atoi( strGroupImagesNo.GetData( ) ) );
		}
	} 

	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetGroupId
// Class:		CImageDb
// Purpose:		get current most loaded group
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::GetGroupId( CImageGroup* pGroup )
{	
	int i = 0;
	int nFoundId = -1;

	// for each group
	for( i=0; i<m_nImageGroup; i++ )
	{
		if( m_vectImageGroup[i] == pGroup )
		{
			nFoundId = i;
			break;
		}
	}

	return( nFoundId );
}


////////////////////////////////////////////////////////////////////
// Method:		GetImage
// Class:		CImageDb
// Purpose:		get current most loaded image 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage* CImageDb::GetImage()
{	
	char strMsg[255];
	int i = 0, j = 0;
	int nMaxCharge = -55555;
	CAstroImage* pImageNode = NULL;
	CAstroImage* pMaxImageNode = NULL;
	CImageGroup* pImageGroup = NULL;

//	CImageGroup* pImageGroup = GetGroup( );
/*
	// for each group
	for( i=0; i<m_nImageGroup; i++ )
	{
		pImageGroup = m_vectImageGroup[i];

		// for each node in a group
		for( j=0; j<pImageGroup->m_nImageNode; j++ )
		{
			pImageNode = pImageGroup->m_vectImageNode[j];
			// find most loaded image node
			if( pImageNode->m_nCurrentCharge > nMaxCharge )
			{
				nMaxCharge = pImageNode->m_nCurrentCharge;
				pMaxImageNode = pImageNode;

			}
		}
	}

	// now set best current image group
	pImageGroup = pMaxImageNode->m_pImageGroup;
*/
	CImageArea* pImageArea =  NULL;
	CImageArea* pMaxImageArea =  NULL;
	// reset threshold back 
	nMaxCharge = -55555;

	return( NULL );
}

////////////////////////////////////////////////////////////////////
// Method:		FindImageGroupByName
// Class:		CImageDb
// Purpose:		get current most loaded image 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CImageGroup* CImageDb::FindImageGroupByName( wxString& strImageGroup )
{
	int i = 0;
	CImageGroup* pImageGroup = NULL;

	// for each group
	for( i=0; i<m_vectImageGroup.size(); i++ )
	{
		if( m_vectImageGroup[i] == NULL ) continue;

		pImageGroup = m_vectImageGroup[i];
		// check if image is named by 
		if( strImageGroup.CmpNoCase( pImageGroup->m_strGroupNameId ) == 0 )
		{
				return( pImageGroup );
		}

	}

	return( NULL );
}

// Method:	FindGroupIdByName
////////////////////////////////////////////////////////////////////
int CImageDb::FindGroupIdByName( wxString strImageGroup )
{
	int i = 0;
	CImageGroup* pImageGroup = NULL;

	// for each group
	for( i=0; i<m_vectImageGroup.size(); i++ )
	{
		if( m_vectImageGroup[i] == NULL ) continue;

		pImageGroup = m_vectImageGroup[i];
		// check if image is named by 
		if( strImageGroup.CmpNoCase( pImageGroup->m_strGroupNameId ) == 0 )
		{
				return( i );
		}

	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:		FindImageInGroupByName
// Class:		CImageDb
// Purpose:		find image in the group by it's name
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage* CImageDb::FindImageInGroupByName( CImageGroup* pImageGroup, 
													wxString& strImageName )
{
	int i = 0;
	CAstroImage* pImageNode = NULL;

	// for each node in a group
	for( i=0; i<pImageGroup->m_vectImageNode.size(); i++ )
	{
		if( pImageGroup->m_vectImageNode[i] == NULL ) continue;

		pImageNode = pImageGroup->m_vectImageNode[i];
		// check if image is named by 
		if( strImageName.CmpNoCase( pImageNode->m_strImageName ) == 0 )
		{
			return( pImageNode );
		}
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
// Method:		SelectGroup
// Class:		CImageDb
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::SelectGroup( int nGroupId )
{
	int i = 0;

	// keep group selectd
	m_nCurrentGroup = nGroupId;

	CImageGroup* pGroup = m_vectImageGroup[nGroupId];

	if( pGroup == NULL || m_nImageGroup == 0 ) return( 0 );

	m_pCurrentGroup = pGroup;

	// first clear the browsing list
	m_pFrame->ClearImageBrowsingList( );
	// now for every image in the group add to the browsing list
	for( i=0; i<pGroup->m_vectImageNode.size(); i++ )
	{
		CAstroImage* pImage = pGroup->m_vectImageNode[i];
		// add image name to select list
		m_pFrame->AddImageToSelect( pImage->m_strImageName );

	}

	// tell frame to mark select first image
	m_pFrame->SelectImageFromList( 0 );
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CreateGroupThumbnails
// Class:		CImageDb
// Purpose:		Create Group Thumbnails
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::CreateGroupThumbnails( int nGroupId )
{
	int i = 0;

	// keep group selectd
//	m_nCurrentGroup = nGroupId;

	CImageGroup* pGroup = m_vectImageGroup[nGroupId];

	if( pGroup == NULL || m_nImageGroup == 0 ) return( 0 );

	wxImageList* imageList = new wxImageList( 100, 80, true, pGroup->m_vectImageNode.size() );
	// first clear the browsing list
	m_pFrame->ClearGroupThumbnails( );
	// now for every image in the group add to the browsing list
	for( i=0; i<pGroup->m_vectImageNode.size(); i++ )
	{
		CAstroImage* pImage = pGroup->m_vectImageNode[i];
		// add image name to select list
		m_pFrame->AddImageToGroupThumbs( i, pImage->m_strImageName );
		// create image blank
		wxBitmap bmp = wxBitmap( DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT );
		// add to image list
		imageList->Add( bmp );
	}
	// assign image list
	m_pFrame->AssignImagesToGroupThumbnails( imageList, pGroup->m_vectImageNode.size() );

	// tell frame to mark select first image
//	m_pFrame->SelectGroupImageThumbnail( 0 );
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SetCurrentGroupThumbnails
// Class:		CImageDb
// Purpose:		set current Group Thumbnails
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::SetCurrentGroupThumbnails( )
{
	int i=0;

	// then initialize my nodes
	for( i=0; i<m_pCurrentGroup->m_vectImageNode.size(); i++ )
	{
		CAstroImage* pImage = m_pCurrentGroup->m_vectImageNode[i];
		wxBitmap bmp;

		int bFound = 0;
		//if( pImage->m_bHasThumnail )
			bFound = pImage->LoadThumbnail( bmp );
		//else
		//	bFound = pImage->CreateThumbnail( bmp );

		// set real image
		if( bFound ) 
		{
			wxMutexGuiEnter();
			m_pFrame->SetGroupThumbnailImage( i, bmp );
			wxMutexGuiLeave();
		}

	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SelectImageFromGroup
// Class:		CImageDb
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::SelectImageFromGroup( int nGroupId, int nImageId )
{
	// exit if not set
	if( nImageId < 0 || nGroupId < 0 || nGroupId >= m_vectImageGroup.size() ) return( 0 );

	CImageGroup* pGroup = m_vectImageGroup[nGroupId];

	// set current image
	m_pCurrentImage = pGroup->m_vectImageNode[nImageId];

	// set logger/sky
	m_pCurrentImage->m_pLogger = m_pLogger;
	m_pCurrentImage->m_pSky = m_pUnimap->m_pSky;

// no point in having it here - to clean
	// try to load exif profile
//	m_pCurrentImage->LoadExifProfile( );
	// load image main profile if any
//	m_pCurrentImage->LoadProfile( );

	// also set config
	m_pCurrentImage->SetConfig( m_pMainConfig, m_pDetectConfig );
	m_pCurrentImage->SetSky( m_pSky );


//	pGroup->m_vectImageNode[nImageId]->m_nCurrentCharge = 999999;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		AddGroup
// Class:		CImageDb
// Purpose:		add a new group
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::AddGroup( const wxString& strGroupName, const wxString& strGroupPath, 
							int bInclude, int bRecursive )
{
	int nNoOfImages = 0;
	// create image group
	CImageGroup* pGroup = new CImageGroup( strGroupName );
	pGroup->m_pImageDb = this;

	// if empty set to default
	if( strGroupPath.IsEmpty() ) 
		pGroup->SetPath( wxGetCwd() );
	else	
		pGroup->SetPath( strGroupPath );
	pGroup->m_pLogger = m_pLogger;
//	m_vectImageGroup[m_nImageGroup]->SetActorName( strGroupActor );

	// set initial number of images in the group's folder if any
//	m_vectImageGroup[m_nImageGroup]->SetMaxNoOfImages( 0 );

	// add to gui
	m_pFrame->AddGroupToSelect( strGroupName );

	if( bInclude == 1 )
	{
		// include all images in this path
		nNoOfImages = pGroup->AddAllImagesFromPath( bRecursive );
	}

	// push in the vector
	m_vectImageGroup.push_back( pGroup );

	m_nImageGroup = m_vectImageGroup.size();

	return( m_nImageGroup-1 );
}

////////////////////////////////////////////////////////////////////
int CImageDb::DeleteGroup( int nGroupId )
{
	if( !m_vectImageGroup[nGroupId] || nGroupId <= 0 ) return( 0 );
	delete( m_vectImageGroup[nGroupId] );
	m_vectImageGroup[nGroupId] = NULL;
	m_vectImageGroup.erase( m_vectImageGroup.begin() + nGroupId );
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		AddImages
// Class:		CImageDb
// Purpose:		add a new group
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageDb::AddImages( const wxString& strName, const wxString& strImages )
{
	wxString strGroupName = strName;
//	wxString strImageFiles = strImages;

	int nNoOfImages = 0;
	int nFirstImageId = -1;
	int nGroupId = -1;

	// try to find group by name
	CImageGroup* pGroup = FindImageGroupByName( strGroupName );
	// check if this group already exists
	if( pGroup == NULL ) AddGroup( strGroupName );

	// and now get my group id
	nGroupId = GetGroupId( pGroup );

	/// add images to this group
	nNoOfImages = pGroup->AddImages( strImages, nFirstImageId );

	// set first group display
	m_pFrame->m_pGroupSelect->SetSelection( nGroupId );
	SelectGroup( nGroupId );
	m_pFrame->m_pImageSelect->SetSelection( nFirstImageId+1 );
	SelectImageFromGroup( nGroupId, nFirstImageId );

//	m_vectImageGroup[m_nImageGroup]->SetPath( strGroupPath );

//	m_nImageGroup++;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CImageDb::InitImageList( int nSource, int nOrder )
{
	int i=0, j=0;

	m_vectImageList.clear();
	m_nImageListCurrent = 0;

	////////////
	// just for the current group
	if( nSource == 0 )
	{
		for( j=0; j<m_pCurrentGroup->m_vectImageNode.size(); j++ )
		{
			DefImageListRec rec;
			rec.group_id = m_nCurrentGroup;
			rec.image_id = j;
			m_vectImageList.push_back( rec );
		}

	// for stack
	} else if( nSource == 1 )
	{
		for( j=0; j<m_pCurrentGroup->m_vectStack.size(); j++ )
		{
			DefImageListRec rec;
			rec.group_id = m_nCurrentGroup;
			rec.image_id = m_pCurrentGroup->m_vectStack[j].image_id;
			m_vectImageList.push_back( rec );
		}

	// for all groups
	} else if( nSource == 2 )
	{
		for( i=0; i<m_vectImageGroup.size(); i++ )
		{
			// and for all images in the group
			for( j=0; j<m_vectImageGroup[i]->m_vectImageNode.size(); j++ )
			{
				DefImageListRec rec;
				rec.group_id = i;
				rec.image_id = j;
				m_vectImageList.push_back( rec );
			}
		}
	}

	///////////////////
	// arange my list - if random shuffle
	if( nOrder == 1 )
	{
		int nCount = m_vectImageList.size();
		for( i=0; i<nCount; i++ )
		{
			// get a random number between 0 and nCount
			unsigned int nSwapTo = (unsigned int) GetRandInt( 0, nCount-1 );
			DefImageListRec tmp;
			tmp.group_id = m_vectImageList[i].group_id;
			tmp.image_id = m_vectImageList[i].image_id;
			m_vectImageList[i].group_id = m_vectImageList[nSwapTo].group_id;
			m_vectImageList[i].image_id = m_vectImageList[nSwapTo].image_id;
			m_vectImageList[nSwapTo].group_id = tmp.group_id;
			m_vectImageList[nSwapTo].image_id = tmp.image_id;

			//std::swap( m_vectImageList.begin()+i, m_vectImageList.begin()+nSwapTo );
		}
	}
}

////////////////////////////////////////////////////////////////////
int CImageDb::GetNextImageFromList( int& group, int& image )
{
	if( m_nImageListCurrent >= m_vectImageList.size() ) return(0);

	group = m_vectImageList[m_nImageListCurrent].group_id;
	image = m_vectImageList[m_nImageListCurrent].image_id;
	m_nImageListCurrent++;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CImageDb::GetPreviousImageFromList( int& group, int& image )
{
	// if 1 then 0 was loaded so nothing to load
	if( m_nImageListCurrent == 1 ) return(0);
	// if 0 the probably nothing was loaded - so load 0
	if( m_nImageListCurrent == 0 )
	{
		group = m_vectImageList[0].group_id;
		image = m_vectImageList[0].image_id;

	} else
	{
		// go back two
		m_nImageListCurrent -= 2;
		group = m_vectImageList[m_nImageListCurrent].group_id;
		image = m_vectImageList[m_nImageListCurrent].image_id;

	}
	// increment as usual with 1
	m_nImageListCurrent++;

	return( 1 );
}