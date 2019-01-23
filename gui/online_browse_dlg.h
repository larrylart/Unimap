
#ifndef _ONLINE_BROWSE_DLG_H
#define _ONLINE_BROWSE_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>

// gui ids
enum
{
	wxID_ONLINE_BROWSE_GETLIST = 15300,
//	wxID_ONLINE_EVENT_LIST_GET_DONE,
	wxID_ONLINE_BROWSE_IMAGE_LIST,
//	wxID_ONLINE_EVENT_PREVIEW_GET_DONE,
	wxID_ONLINE_BROWSE_LIST_SOURCE,
	// action
	wxID_ONLINE_BROWSE_ACT_DOWNLOAD,
	wxID_ONLINE_BROWSE_ACT_VIEW_FULL,
	wxID_ONLINE_BROWSE_ACT_COMMENTS,
	wxID_ONLINE_BROWSE_ACT_DELETE,
	wxID_ONLINE_BROWSE_DEST_TYPE,
	wxID_ONLINE_BROWSE_DEST_SELECT_GROUP,
	wxID_ONLINE_BROWSE_DEST_FOLDER_BUTTON,
	wxID_ONLINE_BROWSE_DEST_NEW_GROUP_BUTTON,
	wxID_ONLINE_BROWSE_SCORE_Q_SELECT,
	wxID_ONLINE_BROWSE_SCORE_A_SELECT,
	wxID_ONLINE_BROWSE_SCORE_S_SELECT,
	wxID_ONLINE_BROWSE_SCORE_RATE_IT,
//	wxID_ONLINE_EVENT_DOWNLOAD_IMAGE_DATA_DONE
};	

// list column ids
enum
{
	LIST_COLUMN_ID_USER = 0,
	LIST_COLUMN_ID_GROUP,
	LIST_COLUMN_ID_IMAGE,
	LIST_COLUMN_ID_SCORE_Q,
	LIST_COLUMN_ID_SCORE_A,
	LIST_COLUMN_ID_SCORE_S,
	LIST_COLUMN_ID_DATETIME
};

// external classes
class CUnimapOnline;
class CUnimapWorker;
class CImageDb;

// class:	COnlineBrowseDlg
///////////////////////////////////////////////////////
class COnlineBrowseDlg : public wxDialog
{
// methods
public:
    COnlineBrowseDlg( wxWindow *parent, const wxString& title );
	~COnlineBrowseDlg( );

	void SetConfig( CUnimapOnline* pUnimapOnline, CUnimapWorker* pUnimapWorker, CImageDb* pImageDb, int nSelectGroup );
	void SetScoreValues( int nIndex, int nId );
	void SetImageList( );

/////////////
// ::DATA
	CUnimapOnline* m_pUnimapOnline;
	CUnimapWorker* m_pUnimapWorker;
	CImageDb* m_pImageDb;

	// column sort flags
	int m_nSortField;
	int m_vectSortField[10];
	// selected image
	long m_nSelectedImage;
	// group to update on exit
	int m_nParentGroupId;
	int m_bNewImageToGroup;

	// lit get header button/controls
	wxTextCtrl*		m_pListMaxNo;
	wxChoice*		m_pListSource;
	wxTextCtrl*		m_pListKey;
	wxButton*		m_pGetListButton;
	// the list of images
	wxListCtrl*		m_pImagesList;
	// image preview
	wxStaticBitmap* m_pImgPreview;
	// action controls
	wxButton*		m_pDownloadButton;
	wxButton*		m_pViewFullButton;
	wxButton*		m_pAddCommentButton;
	wxButton*		m_pDeleteButton;
	// destination
	wxChoice*		m_pDestType;
	wxChoice*		m_pDestGroup;
	wxTextCtrl*		m_pDestFolderEntry;
	wxButton*		m_pDestFolderButton;
	wxButton*		m_pDestNewGroupButton;
	// score
	wxChoice*		m_pScoreQ;
	wxChoice*		m_pScoreA;
	wxChoice*		m_pScoreS;
	wxButton*		m_pRateitButton;


private:
	void OnGetImageList( wxCommandEvent& pEvent );
	void GetImageList( );
//	void OnListFetchDone( wxCommandEvent& pEvent );
	void OnSelectImage( wxListEvent& pEvent );
//	void OnPreviewGetDone( wxCommandEvent& pEvent );
	void OnSelectDestType( wxCommandEvent& pEvent );
	void OnSetDestFolder( wxCommandEvent& pEvent );
	void OnListColumn( wxListEvent& pEvent );
	void OnNewDestGroup( wxCommandEvent& pEvent );
	void OnActDownload( wxCommandEvent& pEvent );
//	void OnDownloadImageDataDone( wxCommandEvent& pEvent );
	void OnActViewFull( wxCommandEvent& pEvent );
	void OnRateImage( wxCommandEvent& pEvent );
	void OnActComments( wxCommandEvent& pEvent );
	void OnActDeleteImage( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
