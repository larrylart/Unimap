
#ifndef _FOCUSEVENT_H
#define _FOCUSEVENT_H

// wx
#include "wx/wxprec.h"

class CFocusEvent 
{
// methods
public:
	CFocusEvent( const wxString& strEventName, long nId, int nStep, int nUnits,
							int nDirection );
	~CFocusEvent( );
	int Edit( const wxString& strName, int nStep, int nUnits, int nDirection );

	wxString	m_strEventName;
	long	m_nId;
	int		m_nStep;
	int		m_nUnits;
	int		m_nDirection;
};

#endif
