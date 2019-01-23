
#ifndef _GROUPDLG_H
#define _GROUPDLG_H

// some defaults
#define DEFAULT_GROUP_NAME "Default"
#define DEFAULT_GROUP_PATH "./"
#define DEFAULT_LOAD_IMAGE_FILE_TYPE "All Images (*.jpg;*.bmp;*.tif;*.png;*.tga;*.gif)|*.jpg;*.bmp;*.tif;*.png;*.tga;*.gif|JPEG (*.jpg)|*.jpg|BPM (*.bmp)|*.bmp|TIFF (*.tif)|*.tif|GIF (*.gif)|*.gif|TGA (*.tga)|*.tga|PNG (*.png)|*.png"

// IDs for the controls and the menu commands
enum
{
	wxID_BUTTON_GROUP_ADD_FILES		= 2002,
	wxID_ADD_IMG_NEW_GROUP			= 2101,
	wxID_ADD_IMG_SELECT_GROUP_NAME	= 2102

};

class CConfigMain;
class CImageDb;

// class:	CGroupSetDialog
///////////////////////////////////////////////////////
class CGroupSetDialog : public wxDialog
{
// methods
public:
    CGroupSetDialog( wxWindow *parent, const wxString& title, const wxString& strPath, int bSimple = 0 );

	wxStaticText*	m_pGroupNameLabel;
	wxTextCtrl*		m_pGroupNameEntry;
	wxTextCtrl*		m_pGroupPathEntry;
	wxButton*		m_pNewGroup;
	wxChoice*		m_pSelectGroup;

	wxFlexGridSizer *sizerMainPanel;

	wxString		m_strFiles;
	wxString		m_strPath;
	int				m_nActionType;

	CImageDb*		m_pImageDb;

	~CGroupSetDialog();

	void OnAddFiles( wxCommandEvent& event );
	void SetConfig( CConfigMain* pMainConfig, CImageDb* pImageDb );

private:
	void OnActionType( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
