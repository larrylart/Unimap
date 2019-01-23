////////////////////////////////////////////////////////////////////
// multi function button
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tooltip.h>

#include "mmMultiButton.h"

#ifdef __MMDEBUG__
// For global debug output:
extern wxTextCtrl *gDebug;

#endif

#include "../cresources/down_btn.xpm"
//static wxBitmap gDownBM(down_btn_xpm);
#include "../cresources/disable.xpm"
//static wxBitmap gDisableBM(wxDISABLE_BUTTON_BITMAP);

static wxColour mmDARK_GREY(100, 100, 100);

DEFINE_EVENT_TYPE(mmEVT_TOGGLE)
DEFINE_EVENT_TYPE(mmEVT_WHOLEDROP_TOGGLE)
DEFINE_EVENT_TYPE(mmEVT_DROP_TOGGLE)

IMPLEMENT_DYNAMIC_CLASS(mmMultiButton, wxWindow)

BEGIN_EVENT_TABLE(mmMultiButton, wxWindow)
	EVT_PAINT(mmMultiButton:: OnPaint)
	EVT_MOUSE_EVENTS(mmMultiButton:: OnMouse)
END_EVENT_TABLE()

// TileBitmap
////////////////////////////////////////////////////////////////////
bool TileBitmap(const wxRect &rect, wxDC &dc, wxBitmap &bitmap)
{
    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();
    int i, j;
	wxBitmap bmap = bitmap;

	if( w>rect.width && h>rect.height ) 
		bmap = bitmap.GetSubBitmap(rect);

    for( i = rect.x ; i < rect.x + rect.width ; i += w )
    {
        for (j = rect.y ; j < rect.y + rect.height ; j += h)
			dc.DrawBitmap(bmap, i, j, TRUE);
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////
bool mmMultiButton::CreateTB( wxWindow *parent,
							const wxWindowID id,
							const wxString &label,
							const wxString& tip,
							wxBitmap &defaultBitmap,
							const wxPoint &pos,
							const wxSize &size,
							const long int style )
{
//	wxToolTip* ttp = GetToolTip();
//	if( !ttp ) ttp = new wxToolTip( wxT("My button") );
//	ttp->SetDelay( 100 );
//	SetToolTip( ttp );

    wxBitmap tmp1(down_btn_xpm), tmp2(wxDISABLE_BUTTON_BITMAP);
    //.Create(down_btn_xpm);:
    gDownBM = tmp1;
    //.Create(wxDISABLE_BUTTON_BITMAP);:
    gDisableBM = tmp2;

    if( !wxWindow::Create(parent, id, pos, size, wxTAB_TRAVERSAL|wxWANTS_CHARS ) ) return FALSE;
    //if( !wxControl::Create(parent, id, pos, size, 0) ) return(false);

    //if ( !Create(parent, id, pos, size, 0, wxDefaultValidator, wxControlNameStr) )
    //   return false;
    //if ( !MSWCreateControl(wxT("BUTTON"), label, pos, size) )
    //  return false;

	// set background
    if( parent )
		SetBackgroundColour( parent->GetBackgroundColour() );
    else
		SetBackgroundColour( *wxLIGHT_GREY );

	// set tooltip
	if( !tip.IsEmpty() ) SetToolTip( tip );

#if 1
    if(!gDisableBM.GetMask())
    {
        // Only set mask for this global bitmap once
        wxMask *mask = new wxMask(gDisableBM, *wxLIGHT_GREY);
        gDisableBM.SetMask(mask);
    }
    if (!gDownBM.GetMask())
    {
        // Only set mask for this global bitmap once
        wxMask *mask = new wxMask(gDownBM, GetBackgroundColour());
        gDownBM.SetMask(mask);
    }
#endif

    mIsActivated = FALSE;
    mHasFocus = FALSE;
    mIsToggleDown = FALSE;
    mIsWholeDropToggleDown = FALSE;
    mIsDropToggleDown = FALSE;
    mIsSelected = FALSE;
    mStyle = style;
    mLabelStr = label;

    //mDefaultBitmap = &defaultBitmap;
	SetDefaultBitmap_noref( defaultBitmap );

    mFocusBitmap = mDefaultBitmap;
    mSelectedBitmap = mDefaultBitmap;
    mToggleBitmap = mDefaultBitmap;

    FindAndSetSize();

    Refresh();
    return TRUE;
    // Constructor:
}

// FindAndSetSize
////////////////////////////////////////////////////////////////////
void mmMultiButton::FindAndSetSize()
{
    // Set (total) border size
    if( mStyle & wxBU_AUTODRAW )
		mBorderSize = 3;
    else if( (mStyle & wxRAISED_BORDER) || (mStyle & wxSUNKEN_BORDER) )
		mBorderSize = 2;
    else if( mStyle & wxSIMPLE_BORDER )
		mBorderSize = 2;
    else
		// Default: No border
		mBorderSize = 0;

    // Set internal margin size (for each side)
    mMarginSize = 4;
    // add more margin left/right to the bitmap:
    mBM_MarginH = 20;

	// if no auto size
    if( !(mStyle & mmMB_NO_AUTOSIZE) )
    {
        // Refresh and set size of button.
        int total_width = 0,
        total_height = 0;
        if (mDefaultBitmap)
        {
            total_width = mDefaultBitmap -> GetWidth() + mBM_MarginH *2;
            total_height = mDefaultBitmap -> GetHeight();
        }
        int labw = 0, labh = 0, ext = 0;
        if( mLabelStr != wxT("") ) GetTextExtent(mLabelStr, &labw, &labh, &ext);
        labh += ext;
#if 1
        if ((mStyle &mmMB_WHOLEDROPDOWN))
        total_width = wxMax(total_width + gDownBM.GetWidth(), labw);
        else
#endif
        total_width = wxMax(total_width, labw);
        total_height += labh;
#if 1
        if ((mStyle &mmMB_DROPDOWN))
        {
            total_width += gDownBM.GetWidth();
            total_height = wxMax(total_height, gDownBM.GetHeight());
        }
#endif
        int w_extra = mBorderSize + 2 *mMarginSize;
        int h_extra = mBorderSize + 2 *mMarginSize;
        if( mStyle & mmMB_DROPDOWN ) w_extra += 2 *mMarginSize;
        if( mLabelStr != wxT("") && mDefaultBitmap ) h_extra += mMarginSize;
        total_width += w_extra;
        total_height += h_extra;

        SetSize(total_width, total_height);
        SetAutoLayout(TRUE);
        Layout();
    }
}

// OnMouse: Update button state
////////////////////////////////////////////////////////////////////
void mmMultiButton::OnMouse( wxMouseEvent& event )
{
    mIsActivated = FALSE;
    if( (mStyle & mmMB_STATIC) || !IsEnabled() )
    {
        mHasFocus = FALSE;
        mIsToggleDown = FALSE;
        mIsWholeDropToggleDown = FALSE;
        mIsDropToggleDown = FALSE;
        mIsSelected = FALSE;
        return;
    }

    if(! (mStyle & mmMB_TOGGLE) ) mIsToggleDown = FALSE;
    if(! (mStyle & mmMB_WHOLEDROPDOWN) ) mIsWholeDropToggleDown = FALSE;
    if(! (mStyle & mmMB_DROPDOWN) ) mIsDropToggleDown = FALSE;

    bool focus_changed = FALSE,
    toggle_changed = FALSE,
    wholedroptoggle_changed = FALSE,
    droptoggle_changed = FALSE,
    selected_changed = FALSE;
    int w, h;
    GetClientSize( &w, &h);
    wxPoint mp = event.GetPosition();

    if( event.Entering() )
    {
        // ENTER
        if( (mStyle & mmMB_AUTODRAW) || (mStyle & mmMB_FOCUS) ) focus_changed =! mHasFocus;
        mHasFocus = TRUE;

    } else if( event.Leaving() )
    {
        // LEAVE
        mIsSelected = FALSE;
        if( !mIsDropToggleDown && !mIsWholeDropToggleDown )
        {
            if( (mStyle & mmMB_AUTODRAW) || (mStyle & mmMB_FOCUS) ) focus_changed = mHasFocus;
            mHasFocus = FALSE;
            if( HasCapture() ) ReleaseMouse();
        }

    } else if( event.LeftDown() || event.LeftDClick() )
    {
        // SELECT
        if (mStyle &mmMB_TOGGLE)
        {
            // TOGGLE
            if( mIsSelected ) selected_changed = TRUE;
            mIsSelected = FALSE;
            CaptureMouse();

        } else if( mStyle & mmMB_WHOLEDROPDOWN )
        {
            // WHOLEDROPDOWN
            if( MouseIsOnButton() )
            {
                if( !mIsSelected ) selected_changed = TRUE;
                mIsSelected = TRUE;
                wholedroptoggle_changed = TRUE;
                mIsWholeDropToggleDown =!mIsWholeDropToggleDown;
                if( mIsWholeDropToggleDown )
					CaptureMouse();
                else
					if( HasCapture() ) ReleaseMouse();

            } else
            {
                // Pressed outside of button
                if( mIsSelected ) selected_changed = TRUE;
                mIsSelected = FALSE;
                if( mIsWholeDropToggleDown ) wholedroptoggle_changed = TRUE;
                mIsWholeDropToggleDown = FALSE;
                if( HasCapture() ) ReleaseMouse();
            }

        } else if( mStyle & mmMB_DROPDOWN )
        {
            // DROPDOWN
            if( mp.x > w - gDownBM.GetWidth() - mBorderSize &&mp.x < w &&
				mp.y > mBorderSize &&mp.y < h - mBorderSize )
            {
                // Drop down arrow pressed
                if( mIsSelected ) selected_changed = TRUE;
                mIsSelected = FALSE;
                droptoggle_changed = TRUE;
                mIsDropToggleDown = !mIsDropToggleDown;
                if( mIsDropToggleDown )
					CaptureMouse();
                else
					if( HasCapture() ) ReleaseMouse();

            } else if( MouseIsOnButton() )
            {
                // Button (not arrow) pressed
                if( !mIsSelected ) selected_changed = TRUE;
                mIsSelected = TRUE;
                //if (mIsDropToggleDown)
                //droptoggle_changed = TRUE;
                //mIsDropToggleDown = FALSE;
                CaptureMouse();

            } else
            {
                // Pressed outside of button
                if( mIsSelected ) selected_changed = TRUE;
                mIsSelected = FALSE;
                if (mIsDropToggleDown)
                droptoggle_changed = TRUE;
                mIsDropToggleDown = FALSE;
                if( HasCapture() ) ReleaseMouse();
            }

        } else
        {
            // 'Normal' button
            if( !mIsSelected ) selected_changed = TRUE;
            mIsSelected = TRUE;
            CaptureMouse();
        }

        if( !MouseIsOnButton() )
        {
            focus_changed = mHasFocus;
            mHasFocus = FALSE;
        }

    } else if (event.LeftUp())
    {
        // ACTIVATE
        if( mStyle & mmMB_TOGGLE )
        {
            // TOGGLE
            if( mIsSelected ) selected_changed = TRUE;
            mIsSelected = FALSE;
            toggle_changed = TRUE;
            mIsToggleDown =!mIsToggleDown;
            if( HasCapture() ) ReleaseMouse();

        } else if (mStyle &mmMB_WHOLEDROPDOWN)
        {
            // WHOLEDROPDOWN
            if( mIsSelected ) selected_changed = TRUE;
            mIsSelected = FALSE;
            if( !mIsWholeDropToggleDown )
				if( HasCapture() ) ReleaseMouse();

        } else if( mStyle & mmMB_DROPDOWN )
        {
            // DROPDOWN
            if( mIsSelected ) selected_changed = TRUE;
            mIsSelected = FALSE;
            if( mp.x > w - gDownBM.GetWidth() - mBorderSize &&mp.x < w &&
				mp.y > mBorderSize &&mp.y < h - mBorderSize )
            {
                // Drop down arrow pressed
                if( !mIsDropToggleDown )
					if( HasCapture() ) ReleaseMouse();

            } else if( MouseIsOnButton() )
            {
                // Button (not arrow) pressed
                if( mIsDropToggleDown ) droptoggle_changed = TRUE;
                mIsDropToggleDown = FALSE;
                if( !droptoggle_changed )
                // NOTE! SEND ACTIVATE SIGNAL!:
					mIsActivated = TRUE;
                if( HasCapture() ) ReleaseMouse();
            }

        } else
        {
            // 'Normal' button
            if( mIsSelected ) selected_changed = TRUE;
            mIsSelected = FALSE;
            // NOTE! SEND ACTIVATE SIGNAL!:
            mIsActivated = TRUE;
            if( HasCapture() ) ReleaseMouse();
        }
    }
    // Redraw only if neccessary
    if( focus_changed || selected_changed || 
		wholedroptoggle_changed || droptoggle_changed || toggle_changed )
    {
        Refresh();
        // Generate events to let derived class know what happened
        if( focus_changed )
        {
            // ENTER/LEAVE
            wxCommandEvent ev(event.GetEventType(), GetId());
            if( mHasFocus )
				ev.SetEventType(wxEVT_ENTER_WINDOW);
            else
				ev.SetEventType(wxEVT_LEAVE_WINDOW);
            // Neccessary?:
            GetEventHandler()->ProcessEvent(ev);
        }

        if( toggle_changed )
        {
            // TOGGLE
            wxCommandEvent ev(mmEVT_TOGGLE, GetId());
            GetEventHandler()->ProcessEvent(ev);
        }

        if( wholedroptoggle_changed )
        {
            // WHOLEDROPDOWN
            wxCommandEvent ev(mmEVT_WHOLEDROP_TOGGLE, GetId());
            GetEventHandler()->ProcessEvent(ev);
        }

        if( droptoggle_changed )
        {
            // DROPDOWN
            wxCommandEvent ev(mmEVT_DROP_TOGGLE, GetId());
            GetEventHandler()->ProcessEvent(ev);
        }

        if( selected_changed )
        {
            // SELECT
            wxCommandEvent ev(wxEVT_COMMAND_LEFT_CLICK, GetId());
            GetEventHandler()->ProcessEvent(ev);
        }

        if( mIsActivated )
        {
            // ACTIVATE
            wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
            GetEventHandler()->ProcessEvent(ev);
        }
    }
    event.Skip();
}

// OnPaint
void mmMultiButton::OnPaint( wxPaintEvent &event )
{

    wxPaintDC dc(this);
	// clear background
    dc.SetBackground( *wxTheBrushList -> FindOrCreateBrush(GetBackgroundColour(), wxSOLID));
    dc.Clear();
	// draw my elements
    RedrawBorders(dc);
    RedrawBitmaps(dc);
    RedrawLabel(dc);
}

// Redraw icons - RedrawBitmaps
////////////////////////////////////////////////////////////////////
void mmMultiButton::RedrawBitmaps(wxDC &dc)
{
    // Find the current bitmap
    // Default bitmap:
    wxBitmap *bm = mDefaultBitmap;

	// Focused bitmap:
	if( (mStyle & mmMB_FOCUS) && mHasFocus ) bm = mFocusBitmap;

	// Selected bitmap:
	if( (mStyle & mmMB_SELECT) && mIsSelected ) bm = mSelectedBitmap;

	// Toggle bitmap:
	if( (mStyle & mmMB_TOGGLE) && mIsToggleDown ) bm = mToggleBitmap;

    int w=0, h=0;
    GetClientSize( &w, &h );
    int offset = 0;
	if( mStyle & mmMB_AUTODRAW )
		if( ( (mHasFocus && mIsSelected) || mIsToggleDown || 
			mIsWholeDropToggleDown) && !mIsDropToggleDown) offset = 1;

	////////////////////////
	// calculate the bitmap vars
	int lw = 0, lh = 0, ext = 0;
	if( mLabelStr != wxT("") ) GetTextExtent( mLabelStr, &lw, &lh, &ext );
	lh += ext;
	int bmdown_x_off = 0;
	if( (mStyle & mmMB_WHOLEDROPDOWN) || (mStyle & mmMB_DROPDOWN) )
		bmdown_x_off = gDownBM.GetWidth();

	int x_off = bm->GetWidth() + bmdown_x_off;
	int y_off = bm->GetHeight() + lh;
	int x = offset + wxMax(int((w - x_off) /2), int((lw - x_off) /2));
	int y = offset + int((h - y_off) /2);

	////////////////////////
	// here we draw the bitmap
    if( bm && bm->Ok() )
    {
        if( IsEnabled() )
		{
			// id down
			if( mIsToggleDown ) 
			{
				wxRect rect( 2, 2, GetClientSize().x-3, GetClientSize().y-3 );
				//wxBitmap tmpbd(wxDISABLE_BUTTON_BITMAP);
				TileBitmap( rect, dc, gDisableBM );
			}
			dc.DrawBitmap( *bm, x, y, TRUE );

		} else
        {
            dc.DrawBitmap( *bm, x, y, TRUE );
            wxRect rect( x, y, bm->GetWidth(), bm->GetHeight() );
            TileBitmap( rect, dc, gDisableBM );
        }
    }

	///////////////////
    // Draw the drop-down-arrow
    if( ( (mStyle & mmMB_DROPDOWN) || (mStyle & mmMB_WHOLEDROPDOWN) ) )
    {
        if( mIsDropToggleDown ) offset = 1;
        int x_off = gDownBM.GetWidth();
        int y_off = gDownBM.GetHeight();
        int x = offset + w - x_off;
        int y = offset + int((h - y_off) /2);
        dc.DrawBitmap(gDownBM, x, y, TRUE);
    }
}

// Redraw label - RedrawLabel
////////////////////////////////////////////////////////////////////
void mmMultiButton::RedrawLabel(wxDC &dc)
{
    int w, h;
    GetClientSize( &w, &h );
    int offset = 0;

    if( mStyle & mmMB_AUTODRAW )
		if( ((mHasFocus &&mIsSelected) || mIsToggleDown || 
			mIsWholeDropToggleDown) && !mIsDropToggleDown ) offset = 1;

    int lw = 0, lh = 0, ext = 0;
    GetTextExtent( mLabelStr, &lw, &lh, &ext );
    lh += ext;
    int bmdown_off = 0;
    if( mStyle & mmMB_DROPDOWN ) bmdown_off = gDownBM.GetWidth();

    int x_off = lw + bmdown_off;
    int y_off = lh + mMarginSize;
    int x = offset + wxMax(int((w - x_off) /2), int((lw - x_off) /2)) - 1;
    int y = offset + wxMax(h - y_off, 0);
    dc.SetFont(GetFont());

    if( IsEnabled() )
    {
        dc.SetTextForeground(GetForegroundColour());
        dc.DrawText(mLabelStr, x, y);

    } else
    {
        dc.SetTextForeground( *wxWHITE);
        dc.DrawText(mLabelStr, x + 1, y + 1);
        dc.SetTextForeground(mmDARK_GREY);
        dc.DrawText(mLabelStr, x, y);
        dc.SetTextForeground( *wxBLACK);
    }
}

// Draw all borders of the button - RedrawBorders
////////////////////////////////////////////////////////////////////
void mmMultiButton::RedrawBorders(wxDC &dc)
{
	// If not mmMB_AUTODRAW and no border, there's nothing to do.
    if( (mStyle & wxBU_AUTODRAW) ||
		// Don't draw borders if wxBitmapButton does it.
		((mStyle & mmMB_AUTODRAW) && !mHasFocus && !mIsToggleDown) ||
		// If mmMB_AUTODRAW, borders are not drawn if button dont have focus and is not toggledown.
		(! (mStyle & mmMB_AUTODRAW) && (mStyle & wxNO_BORDER)) ) return;

    int w, h;
    GetClientSize( &w, &h);

    if( !mHasFocus && mIsToggleDown )
    {
        //if button is toggled down and don't have focus
        wxRect rect(0, 0, GetClientSize().x, GetClientSize().y);
        TileBitmap(rect, dc, gDisableBM);
    }
	if( mStyle & mmMB_DROPDOWN ) w -= gDownBM.GetWidth();

    if( mStyle & wxSIMPLE_BORDER )
    {
        dc.SetPen( *wxThePenList->FindOrCreatePen( *wxBLACK, 1, wxSOLID) );
		//dc.SetPen( *wxGREY_PEN );
        dc.DrawLine(0, 0, 0, h - 1);
        dc.DrawLine(0, 0, w, 0);
        dc.DrawLine(0, h - 1, w, h - 1);
        dc.DrawLine(w - 1, 0, w - 1, h - 1);
        // Drop-down arrow
        if( mStyle & mmMB_DROPDOWN )
        {
            w += gDownBM.GetWidth();
            int x = w - gDownBM.GetWidth() - 1;
            dc.SetPen( *wxThePenList->FindOrCreatePen( *wxBLACK, 1, wxSOLID));
            dc.DrawLine(x, 0, x, h - 1);
            dc.DrawLine(x, 0, w, 0);
            dc.DrawLine(x, h - 1, w, h - 1);
            dc.DrawLine(w - 1, 0, w - 1, h - 1);
        }

    } else if( ( (mStyle & wxSUNKEN_BORDER) &&
				! (mIsSelected || mIsToggleDown || mIsWholeDropToggleDown) && !mIsDropToggleDown) ||
				( (mStyle & wxRAISED_BORDER) &&
				( mIsSelected || mIsToggleDown || mIsWholeDropToggleDown) && !mIsDropToggleDown ) ||
				( !(mStyle & wxSUNKEN_BORDER) &&
				(mIsSelected || mIsToggleDown || mIsWholeDropToggleDown) && !mIsDropToggleDown ) )
    {
        // Has focus, and is selected or toggled down
        //dc.SetPen( *wxThePenList -> FindOrCreatePen( *wxBLACK, 1, wxSOLID));
		dc.SetPen( *wxGREY_PEN );
        dc.DrawLine(0, 0, 0, h);
        dc.DrawLine(0, 0, w, 0);
        dc.SetPen( *wxWHITE_PEN );
        dc.DrawLine(0, h - 1, w - 1, h - 1);
        dc.DrawLine(w - 1, 0, w - 1, h);
       // Drop-down arrow
        if( mStyle & mmMB_DROPDOWN )
        {
            w += gDownBM.GetWidth();
            int x = w - gDownBM.GetWidth();
            dc.SetPen( *wxWHITE_PEN);
            dc.DrawLine(x, h - 1, w - 1, h - 1);
            dc.DrawLine(w - 1, 0, w - 1, h);
            dc.SetPen( *wxThePenList->FindOrCreatePen( *wxBLACK, 1, wxSOLID));
            dc.DrawLine(x, 0, x, h);
            dc.DrawLine(x, 0, w, 0);
        }

	} else
	{
        // Has focus, not selected or toggled down
        dc.SetPen( *wxWHITE_PEN);
        dc.DrawLine(0, 0, 0, h - 1);
        dc.DrawLine(0, 0, w - 1, 0);
        //dc.SetPen( *wxThePenList -> FindOrCreatePen( *wxBLACK, 1, wxSOLID));
		dc.SetPen( wxPen(wxColor(64,64,64)) );
        dc.DrawLine(0, h - 1, w - 1, h - 1);
        dc.DrawLine(w - 1, 0, w - 1, h);
		dc.SetPen( *wxGREY_PEN );
        dc.DrawLine(1, h - 2, w - 2, h - 2);
        dc.DrawLine(w - 2, 1, w - 2, h-1);

        // Drop-down arrow
        if( mStyle & mmMB_DROPDOWN )
        {
            w += gDownBM.GetWidth();
            int x = w - gDownBM.GetWidth();
            if( !mIsDropToggleDown )
				dc.SetPen( *wxWHITE_PEN);
            else
				dc.SetPen( *wxThePenList -> FindOrCreatePen( *wxBLACK, 1, wxSOLID));

            dc.DrawLine(x, 0, x, h - 1);
            dc.DrawLine(x, 0, w - 1, 0);
            if( mIsDropToggleDown )
				dc.SetPen( *wxWHITE_PEN);
            else
				dc.SetPen( *wxThePenList->FindOrCreatePen( *wxBLACK, 1, wxSOLID) );

            dc.DrawLine(x, h - 1, w - 1, h - 1);
            dc.DrawLine(w - 1, 0, w - 1, h);
        }
    }
}

// SetDefaultBitmap
void mmMultiButton::SetDefaultBitmap(wxBitmap& bm)
{
    mDefaultBitmap = &bm;
    Refresh();
}

// SetDefaultBitmap
void mmMultiButton::SetDefaultBitmap_noref(wxBitmap bm)
{
    mMyBitmap = bm;
    mDefaultBitmap = &mMyBitmap;
    Refresh();
}

// SetFocusBitmap
void mmMultiButton::SetFocusBitmap(wxBitmap &bm)
{
    mFocusBitmap = &bm;
    if (!mDefaultBitmap)
    mDefaultBitmap = &bm;
    Refresh();
}

// SetSelectedBitmap
void mmMultiButton::SetSelectedBitmap(wxBitmap &bm)
{
    mSelectedBitmap = &bm;
    if (!mDefaultBitmap)
    mDefaultBitmap = &bm;
    Refresh();
}

// SetToggleBitmap
void mmMultiButton::SetToggleBitmap(wxBitmap &bm)
{
	mToggleBitmap = &bm;
	if( !mDefaultBitmap ) mDefaultBitmap = &bm;
	Refresh();
}

// Sets the string label
void mmMultiButton::SetLabel(wxString label)
{
    mLabelStr = label;
    // update size to avoid text display problem if bigger:
    FindAndSetSize();
    Refresh();
}

void mmMultiButton::SetStyle(const long style)
{
    mStyle = style;
    Refresh();
}

// Update button state
void mmMultiButton::SetFocus(const bool hasFocus)
{
    mHasFocus = hasFocus;
    Refresh();
}

// Update button state
void mmMultiButton::SetSelected(const bool isSelected)
{
    mIsSelected = isSelected;
    Refresh();
}

// Update button state
void mmMultiButton::SetToggleDown(const bool isToggleDown)
{
    mIsToggleDown = isToggleDown;
    Refresh();
}

// Update button state - SetWholeDropToggleDown
void mmMultiButton::SetWholeDropToggleDown(const bool isWholeDropToggleDown)
{
    mIsWholeDropToggleDown = isWholeDropToggleDown;
    Refresh();
}

// Update button state - SetDropToggleDown
void mmMultiButton:: SetDropToggleDown(const bool isDropToggleDown)
{
    mIsDropToggleDown = isDropToggleDown;
    Refresh();
}

// MouseIsOnButton
bool mmMultiButton::MouseIsOnButton()
{
    int cx = 0, cy = 0;
    ClientToScreen( &cx, &cy );
    int cw = 0, ch = 0;
    GetClientSize( &cw, &ch );
    int mpx, mpy;
    :: wxGetMousePosition( &mpx, &mpy );
    return(mpx >= cx && mpx <= cx + cw && mpy >= cy && mpy <= cy + ch);
}

// Enable
bool mmMultiButton::Enable(bool enable)
{
    bool ret = wxWindowBase::Enable(enable);

    Refresh();
    return ret;
}

