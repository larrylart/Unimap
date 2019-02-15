
#ifndef _MMMULTIBUTTON_INCLUDED
#define _MMMULTIBUTTON_INCLUDED

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/event.h>                   // Needed for wxEvent
#include <wx/button.h>                  // Needed for wxButton
#include <wx/bitmap.h>

// mmMultiButton styles (re-use wxFrame/wxDialog style flags)
#define mmMB_FOCUS         0x0400 // Change bitmap on enter/leave.
#define mmMB_SELECT        0x0800 // Change bitmap on select.
#define mmMB_TOGGLE        0x1000 // Change bitmap on toggle.
#define mmMB_DROPDOWN      0x2000 // Draw a drop-down arrow in separate box.
#define mmMB_WHOLEDROPDOWN 0x4000 // Draw a drop-down arrow on the bitmap.
#define mmMB_STATIC        0x8000 // Do not react on mouse events.
#define mmMB_NO_AUTOSIZE   0x0020 // Button should use the given size, and
                                  // not resize itself to fit bitmap/label.
#define mmMB_AUTODRAW      0x0010 // Automatically manage drawing of bitmap,
		  		  // label and border.

// MultiButton events

const int mmEVT_MULTIBUTTON_FIRST = 10000+5400; //wxEVT_FIRST + 5400;

BEGIN_DECLARE_EVENT_TYPES()
     DECLARE_EVENT_TYPE(mmEVT_TOGGLE, mmEVT_MULTIBUTTON_FIRST + 1)
     DECLARE_EVENT_TYPE(mmEVT_WHOLEDROP_TOGGLE, mmEVT_MULTIBUTTON_FIRST + 2)
     DECLARE_EVENT_TYPE(mmEVT_DROP_TOGGLE, mmEVT_MULTIBUTTON_FIRST + 3)
END_DECLARE_EVENT_TYPES()

#define EVT_TOGGLE(id, fn) DECLARE_EVENT_TABLE_ENTRY( mmEVT_TOGGLE, id, -1, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, (wxObject *) NULL ),
#define EVT_WHOLEDROP_TOGGLE(id, fn) DECLARE_EVENT_TABLE_ENTRY( mmEVT_WHOLEDROP_TOGGLE, id, -1, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, (wxObject *) NULL ),
#define EVT_DROP_TOGGLE(id, fn) DECLARE_EVENT_TABLE_ENTRY( mmEVT_DROP_TOGGLE, id, -1, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, (wxObject *) NULL ),

//////////////////////////////////////////////////////////////////////////////
// class mmMultiButton
class mmMultiButton: public wxWindow
{
	DECLARE_DYNAMIC_CLASS(mmMultiButton)

public:
	// Constructor
    mmMultiButton() { }
	// - parent: The button's parent.
	// - id: Button identifier.
	// - label: Optional label.
	// - bitmapDefault: Optional default icon
	// - pos: Position.
	// - size: Size.
	// - style: The following styles are available:
		//   mmMB_AUTODRAW : Automatically manage drawing of bitmap,
		//   		       label and border.
		//   mmMB_FOCUS    : Change icon on enter/leave.
		//   mmMB_SELECT   : Change icon on select.
		//   mmMB_TOGGLE   : Change icon on toggle.
		//   mmMB_STATIC   : Do not react on mouse events.
    mmMultiButton( wxWindow *parent,
					const wxWindowID id = -1,
					const wxString& label = wxEmptyString,
					const wxString& tip = wxEmptyString,
					wxBitmap& defaultBitmap = wxNullBitmap,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					const long int style = 0 )
    { CreateTB(parent, id, label, tip, defaultBitmap, pos, size, style); }

    bool CreateTB( wxWindow *parent,
                const wxWindowID id = -1,
                const wxString& label = wxEmptyString,
				const wxString& tip = wxEmptyString,
				wxBitmap&  defaultBitmap = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const long int style = 0 );

    void     SetDefaultBitmap(wxBitmap& bm);
    void     SetDefaultBitmap_noref(wxBitmap bm);
    void     SetSelectedBitmap(wxBitmap& bm);
    void     SetFocusBitmap(wxBitmap& bm);
    void     SetToggleBitmap(wxBitmap& bm);

    void     SetLabel(wxString str); // Change button label.
    void     SetStyle(const long style); // Change style.
    void     SetFocus(const bool hasFocus);
    void     SetSelected(const bool isSelected);
    void     SetToggleDown(const bool isToggleDown);
    void     SetWholeDropToggleDown(const bool isWholeDropToggleDown);
    void     SetDropToggleDown(const bool isDropToggleDown);

    wxString GetLabel(void) const { return mLabelStr; }
             // Return the label of the button (if any).

    bool     IsToggleDown(void) const { return mIsToggleDown; }
             // Return TRUE if the button is pressed and WHOLEDROP is set
    bool     IsWholeDropPressed(void) const { return mIsWholeDropToggleDown; }
             // Return TRUE if the drop down arrow is pressed
    bool     IsDropToggleDown(void) const { return mIsDropToggleDown; }
             // Return TRUE if the drop down arrow is pressed
    bool     IsSelected(void) const { return mIsSelected; }
             // Return TRUE if the button is selected
    bool     HasFocus(void) const { return mHasFocus; }
             // Return TRUE if the button has focus

    bool     MouseIsOnButton();
             // Returns TRUE if mouse pointer is over the button

    bool     Enable(bool enable);

  private:
    DECLARE_EVENT_TABLE()

    void     OnPaint(wxPaintEvent& event);
    void     OnMouse(wxMouseEvent& event);

    void     RedrawBitmaps(wxDC& dc);
    void     RedrawLabel(wxDC& dc);
    void     RedrawBorders(wxDC& dc);

    void     FindAndSetSize();

    long     mStyle;
    wxString mLabelStr;

    bool     mHasFocus;              // TRUE if mouse is over button.
    bool     mIsActivated;           // TRUE if button is activated.
    bool     mIsToggleDown;          // TRUE if button is toggled down.
    bool     mIsWholeDropToggleDown; // TRUE if mouse down over drop arrow
    bool     mIsDropToggleDown;      // TRUE if mouse down over drop arrow
    bool     mIsSelected;            // TRUE if button is selected.

    int      mBorderSize;
    int      mMarginSize;
    int      mBM_MarginH;

    wxBitmap mMyBitmap;
    wxBitmap* mDefaultBitmap;
    wxBitmap* mSelectedBitmap;
    wxBitmap* mFocusBitmap;
    wxBitmap* mToggleBitmap;
    wxBitmap gDownBM;
    wxBitmap gDisableBM;
}; 

#endif

