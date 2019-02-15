////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CALC_UNITS_DLG_H
#define _CALC_UNITS_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines : unit types
#define CALC_UNITS_TYPE_DIST	0
#define CALC_UNITS_TYPE_MASS	1
#define CALC_UNITS_TYPE_LUM		2

// distance units ids
enum
{
	DUNIT_AU = 0,
	DUNIT_LY,
	DUNIT_PA,
	DUNIT_LD,
	DUNIT_ER,
	DUNIT_KM,
	DUNIT_ML
};

// mass units ids
enum
{
	MUNIT_SM = 0,
	MUNIT_JM,
	MUNIT_EM,
	MUNIT_KG,
	MUNIT_LB,
	MUNIT_OZ,
	MUNIT_ST
};

// luminosity units ids
enum
{
	LUNIT_SL = 0,
	LUNIT_W
};

// gui ids
enum
{
	wxID_CALC_UNITS_TYPE = 8800,
	wxID_CALC_UNITS_FROM_FORMAT,
	wxID_CALC_UNITS_TO_FORMAT,
	wxID_CALC_UNITS_FROM_VALUE,
	wxID_CALC_UNITS_TO_VALUE,
};

// class:	CCalcUnitsDlg
///////////////////////////////////////////////////////
class CCalcUnitsDlg : public wxDialog
{
// public methods
public:
    CCalcUnitsDlg( wxWindow *parent, wxString title );
	virtual ~CCalcUnitsDlg( );

	double Convert_ToKm( double val, int type );
	double Convert_KmTo( double val, int type );

	double Convert_ToKg( double val, int type );
	double Convert_KgTo( double val, int type );

	double Convert_ToW( double val, int type );
	double Convert_WTo( double val, int type );

	void SetFromTo( );

// public data
public:
	wxChoice*	m_pConvType;
	wxChoice*	m_pFromFormat;
	wxChoice*	m_pToFormat;
	wxTextCtrl* m_pFromValue;
	wxTextCtrl* m_pToValue;

	// units from to by type
	wxString m_vectUnitsFromToDist[20];
	wxString m_vectUnitsFromToMass[10];
	wxString m_vectUnitsFromToLum[5];

	// local 
	double m_nDistValue;
	double m_nMassValue;
	double m_nLumValue;

private:
	void OnUnitsType( wxCommandEvent& pEvent );
	void OnFromToSet( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
