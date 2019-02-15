////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include <wx/tokenzr.h>
#include <wx/filename.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

//////////////
// elynx headers
#include <elx/image/ImageFileManager.h>
//#include <elx/image/ImageVariant.h>
//#include <elx/image/ImageImpl.h>

// local headers
#include "../config/config.h"
#include "../config/mainconfig.h"
#include "../image/imagedb.h"
#include "../image/imagegroup.h"
#include "../util/image_func.h"

// main header
#include "groupdlg.h"

using namespace eLynx;
using namespace eLynx::Image;

//		***** CLASS CGroupSetDialog *****
////////////////////////////////////////////////////////////////////
// CGroupSetDialog EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CGroupSetDialog, wxDialog)
	EVT_BUTTON( wxID_BUTTON_GROUP_ADD_FILES, CGroupSetDialog::OnAddFiles )
	EVT_BUTTON( wxID_ADD_IMG_NEW_GROUP, CGroupSetDialog::OnActionType )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGroupSetDialog
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupSetDialog::CGroupSetDialog( wxWindow *parent, const wxString& title,
									 const wxString& strPath, int bSimple  ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
	m_strPath = strPath;
	m_strFiles = wxT("");
	m_nActionType = 0;
	wxString vectSelectGroup[2];
	int i = 0;

	vectSelectGroup[0] = wxT("All Groups");

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// add basic config panel
	wxPanel* pMainPanel = new wxPanel( this, -1 );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer *nameSizer = new wxFlexGridSizer( 1,3,0,0 );
	nameSizer->AddGrowableCol( 0 );
	nameSizer->AddGrowableCol( 1 );
	sizerMainPanel = new wxFlexGridSizer( 4, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerMainPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerMainPage->Add( sizerMainPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10  );


	m_pSelectGroup = new wxChoice( pMainPanel, wxID_ADD_IMG_SELECT_GROUP_NAME,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectSelectGroup,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSelectGroup->SetSelection( 0 );

	m_pGroupNameEntry = new wxTextCtrl( pMainPanel, -1, wxT(DEFAULT_GROUP_NAME),
									wxDefaultPosition, wxSize(150,-1), wxTE_NOHIDESEL );
	m_pNewGroup = new wxButton( pMainPanel, wxID_ADD_IMG_NEW_GROUP, wxT("New"),
					wxPoint(-1, -1), wxSize(60,-1) );

	m_pGroupPathEntry = new wxTextCtrl( pMainPanel, -1, wxT(""),
									wxDefaultPosition, wxSize(210,-1) );

	///////////////////////////////////
	// group name entry
	m_pGroupNameLabel = new wxStaticText(pMainPanel, -1, wxT("Group Name:"));
	sizerMainPanel->Add( m_pGroupNameLabel, 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	sizerMainPanel->Add( nameSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	nameSizer->Add( m_pSelectGroup, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	nameSizer->Add( m_pGroupNameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	nameSizer->Add( m_pNewGroup, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// hide by default the name entry
	m_pGroupNameEntry->Hide();

	///////////////////////////////////
	// group path
	sizerMainPanel->Add(new wxButton( pMainPanel, wxID_BUTTON_GROUP_ADD_FILES, wxT("Add Files"),
					wxPoint(135, 111), wxSize(70,20) ), 0,
					wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMainPanel->Add( m_pGroupPathEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	// calculate sizers
	topsizer->Add( pMainPanel, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	pMainPanel->SetSizer( sizerMainPage );

	// hide group entry if simple
	if( bSimple )
	{
		m_pGroupNameLabel->Hide();
		m_pSelectGroup->Hide();
		m_pGroupNameEntry->Hide();
		m_pNewGroup->Hide();
	}

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGroupSetDialog
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupSetDialog::~CGroupSetDialog( )
{
	delete( m_pGroupNameLabel );
	delete( m_pSelectGroup );
	delete( m_pGroupNameEntry );
	delete( m_pGroupPathEntry );
	delete( m_pNewGroup );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CGroupSetDialog
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGroupSetDialog::SetConfig( CConfigMain* pMainConfig,
											CImageDb* pImageDb )
{
	m_pImageDb = pImageDb;
	int i = 0;

	m_pSelectGroup->Clear();
	for( i=0; i<pImageDb->m_nImageGroup; i++ )
	{
		m_pSelectGroup->Append( wxString(pImageDb->m_vectImageGroup[i]->m_strGroupNameId,wxConvUTF8) );
	}
	// select current group
	m_pSelectGroup->SetSelection( pImageDb->m_nCurrentGroup );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSetGroupPath
// Class:	CGroupSetDialog
// Purose:	when enter pressed in command windows
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGroupSetDialog::OnAddFiles( wxCommandEvent& event )
{
	m_strFiles = m_pGroupPathEntry->GetLineText(0);

	// from elynx - get supported type
//	const char * prIn = the_ImageFileManager.GetInputFileFormatList( true );
//	wxString strImageFormatListIn = wxString::FromAscii( prIn );

	wxString strImageFormatListIn = GetSupportedImageReadTypes();

	unsigned int i = 0;
	wxArrayString vectFilenames;
	wxFileDialog fileDlg( this, wxT("Select Images to load in the group"),
							m_strPath, wxT(""), strImageFormatListIn, //DEFAULT_LOAD_IMAGE_FILE_TYPE,
							wxFD_DEFAULT_STYLE | wxFD_MULTIPLE );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		// populate path field here
		fileDlg.GetPaths( vectFilenames );
		int nCount = vectFilenames.Count();
		// now loop through my files and build string
		for(i=0; i<nCount; i++ )
		{
			m_strFiles += vectFilenames[i] + wxT(";");
		}
		m_pGroupPathEntry->SetValue( m_strFiles );
		// now save path if count
		if( nCount > 0 )
		{
			// extract path for the first one
			wxFileName pFileName( vectFilenames[0] );
			m_strPath = pFileName.GetPath();
			// and save config
		}
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnActionType
////////////////////////////////////////////////////////////////////
void CGroupSetDialog::OnActionType( wxCommandEvent& event )
{

	if( m_nActionType == 1 )
	{
		m_pGroupNameEntry->Hide();
		m_pSelectGroup->Show();
		m_pSelectGroup->SetFocus();
		m_pNewGroup->SetLabel( wxT("New") );
		m_nActionType = 0;

	} else if( m_nActionType == 0 )
	{
		m_pSelectGroup->Hide();
		m_pGroupNameEntry->Show();
		// and also select current text
		wxString strSelectValue = m_pSelectGroup->GetStringSelection();
		m_pGroupNameEntry->SetValue( strSelectValue );
		m_pGroupNameEntry->SetSelection( 0, strSelectValue.Length() );
		m_pGroupNameEntry->SetFocus();
		// check button to future new
		m_pNewGroup->SetLabel( wxT("Use") );
		m_nActionType = 1;
	}

	// reset layout
	sizerMainPanel->Layout( );
	GetSizer()->Layout( );
	Fit( );

	return;
}
