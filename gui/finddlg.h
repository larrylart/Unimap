
#ifndef _FINDDLG_H
#define _FINDDLG_H

enum
{
	wxID_VIEW_FIND_OBJECT_ENTRY		= 2200,
	wxID_VIEW_FIND_OBJECT_BUTTON	= 2201
};

// class:	CFindObjectDialog
///////////////////////////////////////////////////////
class CFindObjectDialog : public wxDialog
{
// methods
public:
    CFindObjectDialog( wxWindow *parent, const wxString& title );
	~CFindObjectDialog( );

	wxTextCtrl*		m_pFindObjectEntry;
	wxButton*		m_pFindObjectButton;

private:
	void OnDoFind( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
