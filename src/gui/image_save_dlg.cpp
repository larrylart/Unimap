////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// elynx headers
#include <elx/image/ImageFileManager.h>

// local headers
#include "../util/func.h"
#include "../util/image_func.h"
#include "../image/astroimage.h"

// main header
#include "image_save_dlg.h"

// CImageSaveDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CImageSaveDlg, wxDialog)
	EVT_BUTTON( wxID_IMAGE_SAVE_FILE_BUTTON, CImageSaveDlg::OnFileSet )
	EVT_BUTTON( wxID_IMAGE_SAVE_PATH_BUTTON, CImageSaveDlg::OnPathSet )
//	EVT_CHECKBOX(id, func)
//	EVT_SPINCTRL( wxID_IMAGE_SAVE_SIZE_WIDTH, CImageSaveDlg::OnSizeChangeS )
//	EVT_SPINCTRL( wxID_IMAGE_SAVE_SIZE_HEIGHT, CImageSaveDlg::OnSizeChangeS )
	EVT_TEXT( wxID_IMAGE_SAVE_SIZE_WIDTH, CImageSaveDlg::OnSizeChangeT )
	EVT_TEXT( wxID_IMAGE_SAVE_SIZE_HEIGHT, CImageSaveDlg::OnSizeChangeT )
	//	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CImageSaveDlg::OnDoFind )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImageSaveDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageSaveDlg::CImageSaveDlg( wxWindow *parent, const wxString& title, CAstroImage* pImage ) : 
								wxDialog(parent, -1, title,
								wxDefaultPosition, wxDefaultSize,
								wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_bOnWidthSetValue = 0;
	m_bOnHeightSetValue = 0;

	m_pAstroImage = pImage;
	m_strPath = m_pAstroImage->m_strImagePath;
	m_strFile = m_pAstroImage->m_strImageName + wxT("_T.") + m_pAstroImage->m_strImageExt;
	m_ImgRatio = (double) m_pAstroImage->m_nHeight / (double) m_pAstroImage->m_nWidth;

	// main sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer* sizerData = new wxFlexGridSizer( 2,2,5,5 );
	sizerData->AddGrowableCol( 1 );

	////////////////////
	// create my data objects
	// file
	m_pFileSetButton = new wxButton( this, wxID_IMAGE_SAVE_FILE_BUTTON, wxT("File"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pFileSetEntry = new wxTextCtrl( this, -1, m_strFile, wxDefaultPosition, wxSize(150,-1) );
	// path
	m_pPathSetButton = new wxButton( this, wxID_IMAGE_SAVE_PATH_BUTTON, wxT("Path"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pPathSetEntry = new wxTextCtrl( this, -1, m_strPath, wxDefaultPosition, wxSize(300,-1) );
	// size
	m_pSizeWidth = new wxSpinCtrl( this, wxID_IMAGE_SAVE_SIZE_WIDTH, wxEmptyString, wxDefaultPosition,
									wxSize(60,-1), wxSP_ARROW_KEYS, 16, 60000, pImage->m_nWidth );
	m_pSizeHeight = new wxSpinCtrl( this, wxID_IMAGE_SAVE_SIZE_HEIGHT, wxEmptyString, wxDefaultPosition, 
									wxSize(60,-1), wxSP_ARROW_KEYS, 16, 60000, pImage->m_nHeight );
	m_pUseSizeConstr = new wxCheckBox( this, -1, wxT("constrain proportions") );
	m_pUseSizeConstr->SetValue( 1 );
	// use tags
	m_pUseTags = new wxCheckBox( this, -1, wxT("Save with image tags/marks/grid/etc as per view") );
	m_pUseTags->SetValue( 1 );

	///////////////////
	// :: file
	sizerData->Add( m_pFileSetButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pFileSetEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: path
	sizerData->Add( m_pPathSetButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pPathSetEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: save in image size
	sizerData->Add( new wxStaticText( this, -1, wxT("Size:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	wxFlexGridSizer* sizerSize = new wxFlexGridSizer( 1,3,0,5 );
	sizerSize->Add( m_pSizeWidth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerSize->Add( m_pSizeHeight, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerSize->Add( m_pUseSizeConstr, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( sizerSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: save type
	sizerData->AddStretchSpacer();
	sizerData->Add( m_pUseTags, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// add to main sizer
	topsizer->Add( sizerData, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImageSaveDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageSaveDlg::~CImageSaveDlg( )
{
	delete( m_pFileSetButton );
	delete( m_pFileSetEntry );
	delete( m_pPathSetButton );
	delete( m_pPathSetEntry );
	delete( m_pSizeWidth );
	delete( m_pSizeHeight );
	delete( m_pUseSizeConstr );
	delete( m_pUseTags );
}

////////////////////////////////////////////////////////////////////
// Method:		OnDoFind
// Class:		CImageSaveDlg
// Purpose:		when find button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageSaveDlg::OnFileSet( wxCommandEvent& pEvent )
{
	// from elynx - get supported type
//	const char * prIn = the_ImageFileManager.GetOutputFileFormatList( true );
//	wxString strImageFormatListOut = wxString::FromAscii( prIn );

	wxString strImageFormatListOut = GetSupportedImageWriteTypes();

	wxFileDialog fileDlg( this, wxT("Save current image as ..."),
							m_strPath, 
							m_strFile, 
							strImageFormatListOut, wxFD_SAVE|wxFD_OVERWRITE_PROMPT );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		m_strPath = fileDlg.GetDirectory();
		m_strFile = fileDlg.GetFilename();

		wxString strFileToSave = fileDlg.GetPath();
		// set fullname
		m_pFileSetEntry->SetValue( m_strFile );
		m_pPathSetEntry->SetValue( m_strPath );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CImageSaveDlg::OnPathSet( wxCommandEvent& pEvent )
{
	wxDirDialog pathDlg( this, wxT("Save current image in ..."), m_strPath );

	if( pathDlg.ShowModal() == wxID_OK )
	{
		m_strPath = pathDlg.GetPath();
		m_pPathSetEntry->SetValue( m_strPath );
	}
}

////////////////////////////////////////////////////////////////////
void CImageSaveDlg::OnSizeChangeS( wxSpinEvent& pEvent )
{
	if( !m_pUseSizeConstr->IsChecked() ) return;

	int nId = pEvent.GetId();
	int nValue = pEvent.GetPosition();
	SetSizeChange( nId, nValue );
}

// tocheck: this was meant to be used for text change ... see if you
// can get it to work else remove
////////////////////////////////////////////////////////////////////
void CImageSaveDlg::OnSizeChangeT( wxCommandEvent& pEvent )
{
	if( !m_pUseSizeConstr->IsChecked() ) return;

	int nId = pEvent.GetId();
	int nValue=0; 
	if( nId == wxID_IMAGE_SAVE_SIZE_WIDTH )
	{
		// ignore if from set value
		if( m_bOnWidthSetValue )
		{
			m_bOnWidthSetValue = 0;
			return;
		}
		nValue = m_pSizeWidth->GetValue();

	} else
	{
		// ignore if from set value
		if( m_bOnHeightSetValue )
		{
			m_bOnHeightSetValue = 0;
			return;
		}

		nValue = m_pSizeHeight->GetValue();
	}

	SetSizeChange( nId, nValue );
}

////////////////////////////////////////////////////////////////////
void CImageSaveDlg::SetSizeChange( int nId, int nValue )
{
	// see which has been changed
	if( nId == wxID_IMAGE_SAVE_SIZE_WIDTH )
	{
		m_bOnHeightSetValue = 1;
		// adjust height
		int nHeight = (int) round( (double) nValue*m_ImgRatio );
		m_pSizeHeight->SetValue( nHeight );

	} else if( nId == wxID_IMAGE_SAVE_SIZE_HEIGHT )
	{
		m_bOnWidthSetValue = 1;
		// adjust width
		int nWidth = (int) round( (double) nValue/m_ImgRatio );
		m_pSizeWidth->SetValue( nWidth );
	}
}
