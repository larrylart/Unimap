////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// wx
#include "wx/wxprec.h"

#include "focusevent.h"

////////////////////////////////////////////////////////////////////
CFocusEvent::CFocusEvent( const wxString& strEventName, long nId, int nStep, 
						 int nUnits, int nDirection )
{ 
	m_strEventName = strEventName;
	m_nId = nId;
	m_nStep = nStep;
	m_nUnits = nUnits;
	m_nDirection = nDirection;

	return;
}

////////////////////////////////////////////////////////////////////
CFocusEvent::~CFocusEvent( )
{
	return;
}

////////////////////////////////////////////////////////////////////
int CFocusEvent::Edit( const wxString& strName, int nStep, int nUnits, int nDirection )
{
	m_strEventName = strName;
	m_nStep = nStep;
	m_nUnits = nUnits;
	m_nDirection = nDirection;

	return( 1 );
}

