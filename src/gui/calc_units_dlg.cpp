////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>

// wx
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local includes
#include "../util/func.h"
#include "../util/units.h"

// main header
#include "calc_units_dlg.h"

BEGIN_EVENT_TABLE(CCalcUnitsDlg, wxDialog)
	EVT_CHOICE( wxID_CALC_UNITS_TYPE, CCalcUnitsDlg::OnUnitsType )
	EVT_CHOICE( wxID_CALC_UNITS_FROM_FORMAT, CCalcUnitsDlg::OnFromToSet )
	EVT_CHOICE( wxID_CALC_UNITS_TO_FORMAT, CCalcUnitsDlg::OnFromToSet )
	EVT_TEXT( wxID_CALC_UNITS_FROM_VALUE, CCalcUnitsDlg::OnFromToSet )
//	EVT_TEXT( wxID_CALC_UNITS_TO_VALUE, CCalcUnitsDlg::OnFromToSet )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CCalcUnitsDlg::CCalcUnitsDlg( wxWindow *parent, wxString title ) 
						: wxDialog( parent, -1, title, wxDefaultPosition,
                          wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL )
{
	m_nDistValue = 0.0;
	m_nMassValue = 0.0;
	m_nLumValue = 0.0;

	// units type
	wxString vectUnitsType[5] = { wxT("Distances"), wxT("Masses"), wxT("Luminosity") };
	// units from to: distance
	m_vectUnitsFromToDist[DUNIT_AU] =  wxT("(AU) Astronomical Units");
	m_vectUnitsFromToDist[DUNIT_LY] =  wxT("(LY) Light Years");
	m_vectUnitsFromToDist[DUNIT_PA] =  wxT("(PC) Parsecs");
	m_vectUnitsFromToDist[DUNIT_LD] =  wxT("(LD) Lunar Distance"); 
	m_vectUnitsFromToDist[DUNIT_ER] =  wxT("Earth Radii");
	m_vectUnitsFromToDist[DUNIT_KM] =  wxT("(Km) Kilometers");
	m_vectUnitsFromToDist[DUNIT_ML] =  wxT("Miles");
	// units from to: mass
	m_vectUnitsFromToMass[MUNIT_SM] = wxT("Solar Mass"); 
	m_vectUnitsFromToMass[MUNIT_JM] = wxT("Jupiter Masses");
	m_vectUnitsFromToMass[MUNIT_EM] = wxT("Earth Masses");
	m_vectUnitsFromToMass[MUNIT_KG] = wxT("(Kg) Kilogram");
	m_vectUnitsFromToMass[MUNIT_LB] = wxT("(Lb) Pound");
	m_vectUnitsFromToMass[MUNIT_OZ] = wxT("(Oz) Ounce");
	m_vectUnitsFromToMass[MUNIT_ST] = wxT("(St) Stone");
	// units from to: luminosity
	m_vectUnitsFromToLum[LUNIT_SL] = wxT("Solar Luminosity");
	m_vectUnitsFromToLum[LUNIT_W] = wxT("Watts");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 1, 1, 5, 5 );

	// entries panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 3, 5, 10 );
	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 5 );

	// define gui elements
	m_pConvType = new wxChoice( this, wxID_CALC_UNITS_TYPE, wxDefaultPosition, wxSize(90,-1), 3, vectUnitsType );
	m_pConvType->SetSelection(0);
	m_pFromFormat = new wxChoice( this, wxID_CALC_UNITS_FROM_FORMAT, wxDefaultPosition, wxSize(140,-1), 7, m_vectUnitsFromToDist );
	m_pFromFormat->SetSelection(0);
	m_pToFormat = new wxChoice( this, wxID_CALC_UNITS_TO_FORMAT, wxDefaultPosition, wxSize(140,-1), 7, m_vectUnitsFromToDist );
	m_pToFormat->SetSelection(1);

	m_pFromValue = new wxTextCtrl( this, wxID_CALC_UNITS_FROM_VALUE, wxT(""), wxDefaultPosition, wxSize(100,-1) );
	m_pFromValue->ChangeValue( wxT("0.0") );
	m_pToValue = new wxTextCtrl( this, wxID_CALC_UNITS_TO_VALUE, wxT(""), wxDefaultPosition, wxSize(100,-1), wxTE_READONLY );

	// :: from
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Type:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pConvType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->AddStretchSpacer();
	// :: from
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("From:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pFromFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pFromValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: to
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("To:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pToFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pToValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CFileExportDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCalcUnitsDlg::~CCalcUnitsDlg( )
{
	delete( m_pConvType );
	delete( m_pFromFormat );
	delete( m_pToFormat );
	delete( m_pFromValue );
	delete( m_pToValue );
}

////////////////////////////////////////////////////////////////////
void CCalcUnitsDlg::OnUnitsType( wxCommandEvent& pEvent )
{
	int i=0;
	wxString strVal=wxT("0.0");
	int nType = m_pConvType->GetSelection();

	// celar selectors
	m_pFromFormat->Clear();
	m_pToFormat->Clear();

	// populate by type
	if( nType == CALC_UNITS_TYPE_DIST )
	{
		for( i=0; i<7; i++ )
		{
			m_pFromFormat->Append( m_vectUnitsFromToDist[i] );
			m_pToFormat->Append( m_vectUnitsFromToDist[i] );
		}

	} else if( nType == CALC_UNITS_TYPE_MASS )
	{
		for( i=0; i<7; i++ )
		{
			m_pFromFormat->Append( m_vectUnitsFromToMass[i] );
			m_pToFormat->Append( m_vectUnitsFromToMass[i] );
		}

	} else if( nType == CALC_UNITS_TYPE_LUM )
	{
		for( i=0; i<2; i++ )
		{
			m_pFromFormat->Append( m_vectUnitsFromToLum[i] );
			m_pToFormat->Append( m_vectUnitsFromToLum[i] );
		}

	}

	// set default to 0 and 1
	m_pFromFormat->SetSelection(0);
	m_pToFormat->SetSelection(1);
	// reset values to zero
	m_pFromValue->SetValue( strVal );
	m_pToValue->SetValue( strVal );

}

////////////////////////////////////////////////////////////////////
void CCalcUnitsDlg::OnFromToSet( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	// set 
	SetFromTo( );
}

////////////////////////////////////////////////////////////////////
void CCalcUnitsDlg::SetFromTo( )
{
	wxString strVal=wxT("");

	int nType = m_pConvType->GetSelection();
	int nFromFormat = m_pFromFormat->GetSelection();
	int nToFormat = m_pToFormat->GetSelection();
	double nFromValue = wxAtof( m_pFromValue->GetLineText(0) );
	double nToValue=0.0;

	////////////////////////
	// FOR :: DISTANCE
	if( nType == CALC_UNITS_TYPE_DIST )
	{
		// convert to km based on from format
		if( nFromFormat != DUNIT_KM )
			nFromValue = Convert_ToKm( nFromValue, nFromFormat );

		// convert to format
		if( nToFormat != DUNIT_KM )
			nToValue = Convert_KmTo( nFromValue, nToFormat );
		else
			nToValue = nFromValue;

		// also store
		m_nDistValue = nToValue;

	////////////////////
	// FOR :: MASS
	} else if( nType == CALC_UNITS_TYPE_MASS )
	{
		// convert to kg based on from format
		if( nFromFormat != MUNIT_KG )
			nFromValue = Convert_ToKg( nFromValue, nFromFormat );

		// convert to format
		if( nToFormat != MUNIT_KG )
			nToValue = Convert_KgTo( nFromValue, nToFormat );
		else
			nToValue = nFromValue;

		// also store
		m_nMassValue = nToValue;

	///////////////////
	// FOR :: LUMINOSITY
	} else if( nType == CALC_UNITS_TYPE_LUM )
	{
		// convert to kg based on from format
		if( nFromFormat != LUNIT_W )
			nFromValue = Convert_ToW( nFromValue, nFromFormat );

		// convert to format
		if( nToFormat != LUNIT_W )
			nToValue = Convert_WTo( nFromValue, nToFormat );
		else
			nToValue = nFromValue;

		// also store
		m_nLumValue = nToValue;
	}

	/////////////////
	// set to entry with exponent if to small or big
	if( nToValue < 0.000000000001 || nToValue > 999999999999.9 )
		strVal.Printf( wxT("%E"), nToValue );
	else
		strVal.Printf( wxT("%.12lf"), nToValue );

	m_pToValue->SetValue( strVal );
}


////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_ToKm( double val, int type )
{
	if( type == DUNIT_AU )
		return( val*UNITD_AU_TO_KM );
	else if( type == DUNIT_LY )
		return( val*UNITD_LY_TO_AU*UNITD_AU_TO_KM );
	else if( type == DUNIT_PA )
		return( val*UNITD_PA_TO_AU*UNITD_AU_TO_KM );
	else if( type == DUNIT_LD )
		return( val*UNITD_LD_TO_KM );
	else if( type == DUNIT_ER )
		return( val*UNITD_ER_TO_KM );
	else if( type == DUNIT_KM )
		return( val );
	else if( type == DUNIT_ML )
		return( val*UNITD_KM_TO_MILE );
}

////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_KmTo( double val, int type )
{
	if( type == DUNIT_AU )
		return( val/UNITD_AU_TO_KM );
	else if( type == DUNIT_LY )
		return( (val/UNITD_AU_TO_KM)/UNITD_LY_TO_AU );
	else if( type == DUNIT_PA )
		return( (val/UNITD_AU_TO_KM)/UNITD_PA_TO_AU );
	else if( type == DUNIT_LD )
		return( val/UNITD_LD_TO_KM );
	else if( type == DUNIT_ER )
		return( val/UNITD_ER_TO_KM );
	else if( type == DUNIT_KM )
		return( val );
	else if( type == DUNIT_ML )
		return( val/UNITD_KM_TO_MILE );
}

////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_ToKg( double val, int type )
{
	if( type == MUNIT_SM )
		return( val*UNITM_SM_TO_KG );
	else if( type == MUNIT_JM )
		return( val*UNITM_JM_TO_KG );
	else if( type == MUNIT_EM )
		return( val*UNITM_EM_TO_KG );
	else if( type == MUNIT_KG )
		return( val );
	else if( type == MUNIT_LB )
		return( val*UNITM_LB_TO_KG );
	else if( type == MUNIT_OZ )
		return( val*UNITM_OZ_TO_KG );
	else if( type == MUNIT_ST )
		return( val*UNITM_ST_TO_KG );
}

////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_KgTo( double val, int type )
{
	if( type == MUNIT_SM )
		return( val/UNITM_SM_TO_KG );
	else if( type == MUNIT_JM )
		return( val/UNITM_JM_TO_KG );
	else if( type == MUNIT_EM )
		return( val/UNITM_EM_TO_KG );
	else if( type == MUNIT_KG )
		return( val );
	else if( type == MUNIT_LB )
		return( val/UNITM_LB_TO_KG );
	else if( type == MUNIT_OZ )
		return( val/UNITM_OZ_TO_KG );
	else if( type == MUNIT_ST )
		return( val/UNITM_ST_TO_KG );
}

////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_ToW( double val, int type )
{
	if( type == LUNIT_SL )
		return( val*UNITL_SL_TO_W );
	else if( type == LUNIT_W )
		return( val );
}

////////////////////////////////////////////////////////////////////
double CCalcUnitsDlg::Convert_WTo( double val, int type )
{
	if( type == LUNIT_SL )
		return( val/UNITL_SL_TO_W );
	else if( type == LUNIT_W )
		return( val );
}
