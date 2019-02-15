////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

//system headers
#include <math.h>
#include <stdio.h>
#include <vector>

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/func.h"
#include "../util/folders.h"
#include "../util/webfunc.h"
#include "../unimap_worker.h"
#include "../image/astroimage.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/bibdb.h"

// main header
#include "simbad.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSimbad
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSimbad::CSimbad( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker, CSky* pSky, CBibDb* pBibDb )
{
	int i=0, j=0, k=0;

	m_pAstroImage = pAstroImage;
	m_pUnimapWorker = pUnimapWorker;
	m_pSky = pSky;
	m_pBibDb = pBibDb;

	// init counters
	m_nIdentifiers = 0;
	m_nCoordType = 0;
	m_strNotesPage = wxT( "" );
	// flags
	m_bHasDetails = 0;
	m_bHasIdentifiers = 0;
	m_bHasNotes = 0;
	// other strings
	m_strProperMotions  = wxT( "" );
	m_strParallax  = wxT( "" );
	m_strRedshift  = wxT( "" );
	m_strFluxB  = wxT( "" );
	m_strFluxV  = wxT( "" );

	// init vectors
	for( i=0; i<10; i++ ) 
	{
		m_vectCoordLabel[i] = wxT( "" );
		m_vectCoordValue[i] = wxT( "" );
	}

	m_nMeasuresTable = 0;
	for( i=0; i<20; i++ ) 
	{
		m_nMeasuresTableCol[i] = 0;
		m_nMeasuresTableRow[i] = 0;
	}

	for( i=0; i<20; i++ ) 
		for( j=0; j<20; j++ ) m_vectMeasuresTableHead[i][j] = wxT( "" );

	for( i=0; i<20; i++ ) 
		for( j=0; j<200; j++ )
			for( k=0; k<20; k++ ) m_vectMeasuresTableValues[i][j][k] = wxT( "" );

	m_strNotesPage = wxT( "" );

	m_vectMeasuresName.clear();
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSimbad
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSimbad::~CSimbad()
{
	m_pAstroImage = NULL;
	m_pUnimapWorker = NULL;

	m_vectMeasuresName.clear();
}

////////////////////////////////////////////////////////////////////
// Method:	GetBasicDetailsForObjList
// Class:	CSimbad
// Purpose:	Get basic details for an object list from simbad
// Input:	input object list, output details list 
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSimbad::GetBasicDetailsForObjList( StarDef* vectObj, unsigned long nObj,
										StarBasicDetailsDef* vectObjDetails )
{
	wxString strMsg = wxT("");
	wxString strFile = wxT("");
	wxString strUrlGetData;
	wxString strObjName = wxT("");
	wxString strObjListSAO = wxT("");;
	wxString strObjListTYCHO = wxT("");;
	wxString strObjListUSNOB1 = wxT("");;
	// simbad script arguments
	wxString strSimbadScriptArgSAO = URLEncode( wxT(" object \"%-20IDLIST(SAO)|%-10PLX(V)|%-10RV(V)|%-10SP(S)|\"") );
	wxString strSimbadScriptArgTYCHO = URLEncode( wxT(" object \"%-20IDLIST(TYC)|%-10PLX(V)|%-10RV(V)|%-10SP(S)|\"") );
	wxString strSimbadScriptArgUSNOB1 = URLEncode( wxT(" object \"%-20IDLIST(USNOB)|%-10PLX(V)|%-10RV(V)|%-10SP(S)|\"") );
	// other flags
	int i=0, bFoundMax=0, nPageNo=0;
	unsigned long nObjStart=0, nObjCnt=0;
	//StarDef vectObjT[1000];

	// setlocal file
	strFile.Printf( wxT("%s%s%s%s"), TEMP_ONLINE_OBJECT_DATA_PATH, wxT("simbad_obj_list"), m_pAstroImage->m_strImageName, wxT(".txt") );

	// build list of objects
	for( i=0; i<nObj; i++ )
	{
		// if object has a name
		if( m_pSky->m_pCatalogStarNames->GetStarSimpleCatName( vectObj[i], strObjName ) )
		{
			// now bye type :: if type 0 = sao
			if( vectObj[i].cat_type == CAT_OBJECT_TYPE_SAO )
			{
				// check if over the limit of my string
				if( strObjListSAO.Length() + strObjName.Length() > 5000 ) bFoundMax = 1;
				// concatenate the strings
				strObjListSAO += wxT("%0a") + strObjName;
				//memcpy( &(vectObjT[nObjCnt]), &(vectObj[i]), sizeof(StarDef) );
				nObjCnt++;

			} else if( vectObj[i].cat_type == CAT_OBJECT_TYPE_TYCHO )
			{
				if( strObjListTYCHO.Length() + strObjName.Length() > 5000 ) bFoundMax = 1;
				strObjListTYCHO += wxT("%0a") + strObjName;
				//memcpy( &(vectObjT[nObjCnt]), &(vectObj[i]), sizeof(StarDef) );
				nObjCnt++;

			} else if( vectObj[i].cat_type == CAT_OBJECT_TYPE_USNO )
			{
				if( strObjListUSNOB1.Length() + strObjName.Length() > 5000 ) bFoundMax = 1;
				strObjListUSNOB1 += wxT("%0a") + strObjName;
				//memcpy( &(vectObjT[nObjCnt]), &(vectObj[i]), sizeof(StarDef) );
				nObjCnt++;
			}
		}

		//////////////////////
		// check if object details
		if( bFoundMax || i >= nObj-1 )
		{
			nPageNo++;
			///////////////////////////////////
			// Simbad - use get object list details
			// http://simbad.u-strasbg.fr/simbad/sim-script?script=format object "%IDLIST(1)|%-10PLX(V)|%-10RV(V)|%-10SP(S)|"\nhd1\nhd2

			strMsg.Printf( wxT("Fetch Online data page=%d..."), nPageNo ); 
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );

			// chekc if and fetch by type
			if( strObjListSAO.Length() != 0 )
			{
				// set URL to get simbad data
				strUrlGetData.Printf( wxT("http://simbad.u-strasbg.fr/simbad/sim-script?script=format%s%s"), 
											strSimbadScriptArgSAO, strObjListSAO );
				// downlaod remote file
				LoadWebFile( strUrlGetData, strFile );
				// now load local file
				LoadSimbadObjectListDataFile( strFile, vectObj, nObjStart, nObjCnt, vectObjDetails );
			}
			// now if tycho
			if( strObjListTYCHO.Length() != 0 )
			{
				// set URL to get simbad data
				strUrlGetData.Printf( wxT("http://simbad.u-strasbg.fr/simbad/sim-script?script=format%s%s"), 
											strSimbadScriptArgTYCHO, strObjListTYCHO );
				// downlaod remote file
				LoadWebFile( strUrlGetData, strFile );
				// now load local file
				LoadSimbadObjectListDataFile( strFile, vectObj, nObjStart, nObjCnt, vectObjDetails );
			}
			// now if usnob1
			if( strObjListUSNOB1.Length() != 0 )
			{
				// set URL to get simbad data
				strUrlGetData.Printf( wxT("http://simbad.u-strasbg.fr/simbad/sim-script?script=format%s%s"), 
											strSimbadScriptArgUSNOB1, strObjListUSNOB1 );
				// downlaod remote file
				LoadWebFile( strUrlGetData, strFile );
				// now load local file
				LoadSimbadObjectListDataFile( strFile, vectObj, nObjStart, nObjCnt, vectObjDetails );
			}
			// reset flag
			bFoundMax = 0;
			nObjStart = i+1;
			nObjCnt = 0;
			strObjListSAO = wxT("");
			strObjListTYCHO = wxT("");
			strObjListUSNOB1 = wxT("");
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSimbadObjectListDataFile
// Class:	CSimbad
// Purpose:	load simbad object list data file
// Input:	file name
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSimbad::LoadSimbadObjectListDataFile( const wxString& strFile, StarDef* vectObj,
										  unsigned long nObjStart, unsigned long nObj,
										StarBasicDetailsDef* vectObjDetails )
{
	FILE* pLocalFile = NULL;
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	wxString strTmp = wxT("");
//	wxRegEx reStartData = wxT( "^\\:\\:data\\:\\:\\:" ); 
	int bDataSection = 0;
	long nObjId = -1;
	wxChar strLineFormat[50] = wxT("%[^|]|%[^|]|%[^|]|");
	wxChar strObjName[255];
	wxChar strParallax[255];
	wxChar strRadialVelocity[255];
	// matching
	unsigned long nCatZone=0;
	unsigned long nCatNo=0;
	unsigned long nCatComp=0;
	unsigned long i=0;
	wxRegEx reStarLabelSAO = wxT( "\\ *SAO\\ *([0-9]+)" );
	wxRegEx reStarLabelHIP = wxT( "\\ *HIP\\ *([0-9]+)" );
	wxRegEx reStarLabelTYCHO = wxT( "\\ *TYC\\ *([0-9]+)\\-([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabelUSNOB = wxT( "\\ *USNOB\\ *([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabelNOMAD = wxT( "\\ *NOMAD\\ *([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabel2MASS = wxT( "\\ *2MASS\\ *(.+)" );
	wxRegEx reStarLabelGSC = wxT( "\\ *GSC\\ *(.+)" );
	char strCatName[255];
	wxString strWxCatName=wxT("");

	m_pUnimapWorker->SetWaitDialogMsg( wxT("Load data file ...") );

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
//		wxString strWxLine = strLine;
		//strWxLine.Trim( 1 ).Trim( 0 );

		// check for empty lines - or small
		if( wxStrlen( strLine ) < 20 ) continue;
		// check i fdata block to stars
		if( bDataSection == 0 )
		{
			//if( reStartData.Matches( strWxLine ) ) bDataSection = 1;
			if( wxStrncmp( strLine, wxT("::data:::"), 9 ) == 0 ) bDataSection = 1;
			continue;

		} else
		{
			// get all fields with sscanf
			wxSscanf( strLine, strLineFormat, strObjName, strParallax, strRadialVelocity );
			// get object name
//			wxString strObjName = strWxLine.Mid( 0, 20 );
			// get object if from current image
			//nObjId = m_pAstroImage->GetStarIdByName( vectObj, nObj, strObjName );

			nObjId = -1;
			if( reStarLabelSAO.Matches( strObjName ) )
			{
				if( reStarLabelSAO.GetMatch( strObjName, 1 ).ToULong( &nCatNo ) )
				{
					for( i=nObjStart; i<nObj; i++ )
						if( vectObj[i].cat_type == CAT_OBJECT_TYPE_SAO && vectObj[i].cat_no == nCatNo ) { nObjId = i; break; }
				}

			} else if( reStarLabelHIP.Matches( strObjName ) )
			{
				if( reStarLabelHIP.GetMatch( strObjName, 1 ).ToULong( &nCatNo ) )
				{
					// for all catalog stars
					for( i=nObjStart; i<nObj; i++ )
						if( vectObj[i].cat_type == CAT_OBJECT_TYPE_HIPPARCOS && vectObj[i].cat_no == nCatNo ) { nObjId = i; break; }
				}

			} else if( reStarLabelTYCHO.Matches( strObjName ) )
			{
				if( reStarLabelTYCHO.GetMatch( strObjName, 1 ).ToULong( &nCatZone ) &&
					reStarLabelTYCHO.GetMatch( strObjName, 2 ).ToULong( &nCatNo ) &&
					reStarLabelTYCHO.GetMatch( strObjName, 3 ).ToULong( &nCatComp ) )
				{
					// for all catalog stars
					for( i=nObjStart; i<nObj; i++ )
						if( vectObj[i].cat_type == CAT_OBJECT_TYPE_TYCHO &&
							vectObj[i].zone_no == nCatZone &&
							vectObj[i].cat_no == nCatNo &&
							vectObj[i].comp_no == nCatComp ) { nObjId = i; break; }
				}

			} else if( reStarLabelUSNOB.Matches( strObjName ) )
			{
				if( reStarLabelUSNOB.GetMatch( strObjName, 1 ).ToULong( &nCatZone ) &&
					reStarLabelUSNOB.GetMatch( strObjName, 2 ).ToULong( &nCatNo ) )
				{
					// for all catalog stars
					for( i=nObjStart; i<nObj; i++ )
						if( vectObj[i].cat_type == CAT_OBJECT_TYPE_USNO &&
							vectObj[i].zone_no == nCatZone &&
							vectObj[i].cat_no == nCatNo ) { nObjId = i; break; }
				}

			} else if( reStarLabelNOMAD.Matches( strObjName ) )
			{
				if( reStarLabelNOMAD.GetMatch( strObjName, 1 ).ToULong( &nCatZone ) &&
					reStarLabelNOMAD.GetMatch( strObjName, 2 ).ToULong( &nCatNo ) )
				{
					// for all catalog stars
					for( i=nObjStart; i<nObj; i++ )
						if( vectObj[i].cat_type == CAT_OBJECT_TYPE_NOMAD &&
							vectObj[i].zone_no == nCatZone &&
							vectObj[i].cat_no == nCatNo ) { nObjId = i; break; }
				}

			} else if( reStarLabel2MASS.Matches( strObjName ) )
			{
				strWxCatName = reStarLabel2MASS.GetMatch( strObjName, 1 ).Trim(0).Trim(1);
				strcpy( strCatName, strWxCatName.ToAscii() );

				// for all catalog stars
				for( i=nObjStart; i<nObj; i++ )
					if( vectObj[i].cat_name && stricmp( strCatName, vectObj[i].cat_name ) == 0 ) { nObjId = i; break; }

			} else if( reStarLabelGSC.Matches( strObjName ) )
			{
				strWxCatName = reStarLabelGSC.GetMatch( strObjName, 1 ).Trim(0).Trim(1);
				strcpy( strCatName, strWxCatName.ToAscii() );

				// for all catalog stars
				for( i=nObjStart; i<nObj; i++ )
					if( vectObj[i].cat_name && stricmp( strCatName, vectObj[i].cat_name ) == 0 ) { nObjId = i; break; }
			}

			/////////////////
			// check if found
			if( nObjId >= 0 )
			{
				vectObjDetails[nObjId].parallax = wxAtof( strParallax );
				vectObjDetails[nObjId].radial_velocity = wxAtof( strRadialVelocity );

				/*
				// get paralax
				strTmp = strWxLine.Mid( 21, 10 ).Trim(0).Trim(1);
				if( !strTmp.ToDouble( &(vectObjDetails[nObjId].parallax) ) )
					vectObjDetails[nObjId].parallax = 0;
				// get radial velocity
				strTmp = strWxLine.Mid( 32, 10 ).Trim(0).Trim(1);
				if( !strTmp.ToDouble( &(vectObjDetails[nObjId].radial_velocity) ) )
					vectObjDetails[nObjId].radial_velocity = 0;
				*/
				// get spctral type -- todo :: get map string to integer
				// strWxLine.Mid( 43, 10 ).Trim(0).Trim(1)
				vectObjDetails[nObjId].spectral_type = 0;
				// reset distance to zero
				//vectObjDetails[nObjId].distance = 0;
			}
		}
	}
	// close file
	fclose( pLocalFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetObjectData
// Class:	CSimbad
// Purpose:	Get information from online simbad data
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSimbad::GetObjectData( wxString& strObjectCatName ) 
{
	wxString strFile;
//	char strUrlSetOutput[500];
	wxString strUrlGetData;

	// setlocal file
	strFile.Printf( wxT("%s%s%s%s"), TEMP_ONLINE_OBJECT_DATA_PATH, wxT("simbad_"), strObjectCatName, wxT(".txt") );

	wxString strWxFile = strFile;
	if( ::wxFileExists( strWxFile ) ==  0 )
	{
		///////////////////////////////////
		// Aladin - use get get image 
		// http://aladin.u-strasbg.fr/alapre.pl?frame=result&-c=06%2044%2020.102%2b81%2037%2027.42&ident=sao4099
		// http://aladin.u-strasbg.fr/alapre.pl?out=getFile&file=stsci_POSSI_xe004_E_242601_293847.jpg&fmt=jpg

		// NED
		// http://nedwww.ipac.caltech.edu/cgi-bin/NEDspectra?objname=NGC891&extend=always&detail=1&preview=1&numpp=20&refcode=ANY&bandpass=ANY&line=ANY
		// STSCI
		// http://stdatu.stsci.edu/cgi-bin/dss_form?target=TYC+4383-00589-1&resolver=SIMBAD

		///////////////////////////////////
		// Simbad - use to get object data
		// http://simbad.u-strasbg.fr/simbad/sim-id?Ident=sao1045&NbIdent=1&Radius=2&Radius.unit=arcmin&submit=submit+id
		/////
		// simbad output set
		// http://simbad.u-strasbg.fr/simbad/sim-fout?httpRadio=Get&output.format=ASCII&output.file=on&output.max=10000&list.idsel=on&list.idopt=FIRST&list.idcat=&list.otypesel=on&otypedisp=V&otypedispall=on&list.coo1=on&frame1=ICRS&epoch1=2000&equi1=2000&coodisp1=s2&obj.coo2=on&list.coo2=on&frame2=FK5&epoch2=2000&equi2=2000&coodisp2=s2&obj.coo3=on&list.coo3=on&frame3=FK4&epoch3=1950&equi3=1950&coodisp3=s2&obj.coo4=on&list.coo4=on&frame4=Gal&epoch4=2000&equi4=2000&coodisp4=d2&obj.pmsel=on&list.pmsel=on&obj.plxsel=on&list.plxsel=on&obj.rvsel=on&list.rvsel=on&rvDatabase=on&rvRedshift=on&rvRadvel=on&rvCZ=on&obj.fluxsel=on&list.fluxsel=on&list.flux1=B&list.flux2=V&list.flux3=U&obj.spsel=on&list.spsel=on&obj.mtsel=on&list.mtsel=on&obj.sizesel=on&list.sizesel=on&obj.bibsel=on&list.bibsel=on&bibyear1=1850&bibyear2=2008&bibjnls=&bibdisplay=refsum&bibcom=on&obj.notesel=on&list.notesel=on&notedisplay=A&obj.messel=on&list.messel=on&list.mescat=&mesdisplay=A&obj.extsel=on&extVizier=on&extHeasarc=on&save=SAVE
		// httpRadio=Get
		// output.format=ASCII
		// output.file=on 
		// output.max=10000
		// list.idsel=on
		// list.idopt=FIRST
		// list.idcat=
		// list.otypesel=on
		// otypedisp=V
		// otypedispall=on
		// list.coo1=on
		// frame1=ICRS
		// epoch1=2000&equi1=2000&coodisp1=s2&obj.coo2=on&list.coo2=on&frame2=FK5&epoch2=2000&equi2=2000&coodisp2=s2&obj.coo3=on&list.coo3=on&frame3=FK4&epoch3=1950&equi3=1950&coodisp3=s2&obj.coo4=on&list.coo4=on&frame4=Gal&epoch4=2000&equi4=2000&coodisp4=d2&
		// obj.pmsel=on
		// list.pmsel=on
		// obj.plxsel=on
		// list.plxsel=on
		// obj.rvsel=on
		// list.rvsel=on
		// rvDatabase=on
		// rvRedshift=on
		// rvRadvel=on
		// rvCZ=on
		// obj.fluxsel=on
		// list.fluxsel=on
		// list.flux1=B
		// list.flux2=V
		// list.flux3=U
		// obj.spsel=on
		// list.spsel=on&obj.mtsel=on&list.mtsel=on&obj.sizesel=on&list.sizesel=on&obj.bibsel=on&list.bibsel=on&bibyear1=1850&
		// bibyear2=2008&bibjnls=&bibdisplay=refsum&bibcom=on&obj.notesel=on&list.notesel=on&notedisplay=A&
		// obj.messel=on&list.messel=on&list.mescat=&mesdisplay=A&obj.extsel=on&extVizier=on&extHeasarc=on&save=SAVE

		m_pUnimapWorker->SetWaitDialogMsg( wxT("Fetch Simbad data ...") );

		// set url to set simbad output
	//	sprintf( strUrlSetOutput, "http://simbad.u-strasbg.fr/simbad/sim-fout?httpRadio=Post&output.format=ASCII&output.file=on&output.max=10000&list.idsel=on&list.idopt=FIRST&list.idcat=&list.otypesel=on&otypedisp=V&otypedispall=on&list.coo1=on&frame1=ICRS&epoch1=2000&equi1=2000&coodisp1=s2&obj.coo2=on&list.coo2=on&frame2=FK5&epoch2=2000&equi2=2000&coodisp2=s2&obj.coo3=on&list.coo3=on&frame3=FK4&epoch3=1950&equi3=1950&coodisp3=s2&obj.coo4=on&list.coo4=on&frame4=Gal&epoch4=2000&equi4=2000&coodisp4=d2&obj.pmsel=on&list.pmsel=on&obj.plxsel=on&list.plxsel=on&obj.rvsel=on&list.rvsel=on&rvDatabase=on&rvRedshift=on&rvRadvel=on&rvCZ=on&obj.fluxsel=on&list.fluxsel=on&list.flux1=B&list.flux2=V&list.flux3=U&obj.spsel=on&list.spsel=on&obj.mtsel=on&list.mtsel=on&obj.sizesel=on&list.sizesel=on&obj.bibsel=on&list.bibsel=on&bibyear1=1850&bibyear2=2008&bibjnls=&bibdisplay=refsum&bibcom=on&obj.notesel=on&list.notesel=on&notedisplay=A&obj.messel=on&list.messel=on&list.mescat=&mesdisplay=A&obj.extsel=on&extVizier=on&extHeasarc=on&save=SAVE" );
		// set URL to get simbad data
		strUrlGetData.Printf( wxT("http://simbad.u-strasbg.fr/simbad/sim-id?output.format=ASCII&otypedisp=V&otypedispall=on&bibyear1=1850&bibyear2=2008&obj.notesel=on&list.notesel=on&notedisplay=A&obj.messel=on&list.messel=on&mesdisplay=A&obj.extsel=on&extVizier=on&extHeasarc=on&Ident=%s"),
									strObjectCatName );

		// load webfile ... perhaps I should use LoadWebFileT...
		LoadWebFile( strUrlGetData, strFile );

	}

	m_pUnimapWorker->SetWaitDialogMsg( wxT("Load Simbad file ...") );
	// now load local file
	LoadSimbadObjectDataFile( strFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSimbadObjectDataFile
// Class:	CSimbad
// Purpose:	load simbad object data file
// Input:	file name
// Output:	statsus
/////////////////////////////////////////////////////////////////////
int CSimbad::LoadSimbadObjectDataFile( const wxString& strFile )
{
	FILE* pLocalFile = NULL;
	wxString strTmp;

	// flags
	int bIdentifiers = 0;
	int bBibcodes = 0;
	int bMeasures = 0;
	int bNotes = 0;
	long nIdentifiers = 0;
	int bMeasuresHeader = 0;
	int nMesTableNo = 0;
	m_strRadialVelocity = wxT("");
	m_strCZ = wxT("");

//	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	wxRegEx reNameHeader = wxT( "Object\\ *([^\\-]+)\\-+\\ *(.+)?\\-\\-\\-\\ *OID\\=" ); 
	wxRegEx reCoordSexagesimal = wxT( "^Coordinates\\(([^\\)]+)\\):\\ *([0-9\\+\\.\\-]+ [0-9\\+\\.\\-]+ [0-9\\+\\.\\-]+)  ([0-9\\+\\.\\-]+ [0-9\\+\\.\\-]+ [0-9\\+\\.\\-]+)\\ " );
	wxRegEx reCoordDecimal = wxT( "^Coordinates\\(([^\\)]+)\\):\\ *([0-9\\+\\.\\-]+)  ([0-9\\+\\.\\-]+)\\ " );
	wxRegEx reProperMotions = wxT( "^Proper motions:\\ *([0-9\\.\\+\\-]+)\ *([0-9\\.\\+\\-]+)" );
	wxRegEx reParallax = wxT( "^Parallax:\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reRadialVelocity = wxT( "^Radial Velocity:\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reRedshift= wxT( "Redshift:\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reCZ = wxT( "cz:\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reFluxB = wxT( "Flux B :\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reFluxV = wxT( "Flux V :\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reFluxJ = wxT( "Flux J :\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reFluxH = wxT( "Flux H :\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reFluxK = wxT( "Flux K :\\ *([0-9\\+\\.\\-]+)" );
	wxRegEx reSpectralType = wxT( "Spectral type:\\ *([a-zA-Z0-9\\.]+)" );
	wxRegEx reMorphologicalType = wxT( "Morphological type:\\ *([^\\~]+)\\~" );
	wxRegEx reAngularSize = wxT( "Angular size:\\ *([0-9\\+\\-\\.]+)[\\ \\~]+([0-9\\+\\-\\.]+)[\\ \\~]+([0-9\\+\\-\\.]+)" );
	wxRegEx reIdentifiers = wxT( "Identifiers\\ *\\(([0-9]+)\\):" );
	wxRegEx reBibcodes = wxT( "Bibcodes\\ *([0-9]+)\\-([0-9]+) \\(\\) \\(([0-9]+)\\):" );
	// measures
	wxRegEx reMeasures = wxT( "Measures\\ *\\((.+)\\):" );
	wxRegEx reMeasuresName = wxT( "^([^\\:]+):\\ *$" );
	wxRegEx reMeasuresTableLine = wxT( "^([^\\|]+)\\|(.*)$" );
	wxRegEx reMeasuresTableLineWord = wxT( "([^\\|]+)\\|" );
	// notes
	wxRegEx reNotes = wxT( "Notes \\(([0-9]+)\\) :" );
	wxRegEx reNotesLine = wxT( "^\\(S\\) *(.*)$" );
	wxRegEx reObjectTag( wxT( "^(.*)\\\\object\\{([^\\}]*)\\}(.*)$" ), wxRE_ADVANCED  );
	// sections regex - ^[\\ ]+([^\\ ]+)\\ ([^\\ ]+)?\\ \\ 
	wxRegEx reIdentifiersWord( wxT( "[ +]?([^ \\+]+)([ \\+]{1})(.+?)  .*" ), wxRE_ADVANCED  );
	wxRegEx reBibcodesWord( wxT( "[ +]?([^ ]+)  .*" ), wxRE_ADVANCED  );
	// special :: name
	wxRegEx reObjectName( wxT( "^NAME\\ *(.+)$" ), wxRE_ADVANCED  );
	// end of page
	wxRegEx reEndOfPage( wxT( "^[\\=]+" ), wxRE_ADVANCED  );

	// clear/zero fields
	m_nCoordType = 0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim( 1 ).Trim( 0 );
		strWxLine = wxT("  ") + strWxLine + wxT("  ");

		//////////////////////////////////////
		// here we read sections set like: Identifiers, etc
		if( bIdentifiers == 1 )
		{
		}

		// check now fields
		if( reNameHeader.Matches( strWxLine ) )
		{
			m_strObjectName = reNameHeader.GetMatch( strWxLine, 1 );
			m_strObjectName.Trim( 1 ).Trim( 0 );
			// check if name/replace and set title
			if( reObjectName.Matches( m_strObjectName ) )
			{
				m_strObjectName = reObjectName.GetMatch( m_strObjectName, 1 );
			}

			m_strObjectType = reNameHeader.GetMatch( strWxLine, 2 );

		} else if( reCoordSexagesimal.Matches( strWxLine ) )
		{
			m_vectCoordLabel[m_nCoordType] = reCoordSexagesimal.GetMatch(strWxLine, 1);
			wxString strCoordValue = wxT("RA: ") + reCoordSexagesimal.GetMatch(strWxLine, 2 ) + wxT(" DEC: ") +
										reCoordSexagesimal.GetMatch(strWxLine, 3 );
			m_vectCoordValue[m_nCoordType] = strCoordValue;
			m_nCoordType++;
			// here we set to show the details panel as well
			m_bHasDetails = 1;

		} else if( reCoordDecimal.Matches( strWxLine ) )
		{
			m_vectCoordLabel[m_nCoordType] = reCoordDecimal.GetMatch( strWxLine, 1);
			wxString strCoordValue = wxT("RA: ") + reCoordDecimal.GetMatch(strWxLine, 2 ) + wxT(" DEC: ") +
										reCoordDecimal.GetMatch(strWxLine, 3 );
			m_vectCoordValue[m_nCoordType] = strCoordValue;
			m_nCoordType++;

		} else if( reProperMotions.Matches( strWxLine ) )
		{
			//double nMotA = 0, nMotB = 0;
			//reProperMotions.GetMatch(strLine, 1 ).ToDouble( &nMotA );
			//reProperMotions.GetMatch(strLine, 2 ).ToDouble( &nMotB );
			//sprintf( strTmp, "%f %f", nMotA, nMotB );
			m_strProperMotions.Printf( wxT("%s %s"), reProperMotions.GetMatch(strWxLine, 1 ), reProperMotions.GetMatch(strWxLine, 2 ) );

		} else if( reParallax.Matches( strWxLine ) )
		{
			m_strParallax.Printf( wxT("%s"), reParallax.GetMatch(strWxLine, 1 ) );

		} else if( reRadialVelocity.Matches( strWxLine ) )
		{
			m_strRadialVelocity = reRadialVelocity.GetMatch(strWxLine, 1 );
//			m_pRadialVelocity->SetLabel( strTmp );

		} else if( reRedshift.Matches( strWxLine ) )
		{
			m_strRedshift.Printf( wxT("%s"), reRedshift.GetMatch(strWxLine, 1 ) );

		} else if( reCZ.Matches( strWxLine ) )
		{
			m_strCZ = reCZ.GetMatch(strWxLine, 1 );

		} else if( reFluxB.Matches( strWxLine ) )
		{
			m_strFluxB = reFluxB.GetMatch(strWxLine, 1);

		} else if( reFluxV.Matches( strWxLine ) )
		{
			m_strFluxV = reFluxV.GetMatch(strWxLine, 1);

		} else if( reFluxJ.Matches( strWxLine ) )
		{
			m_strFluxJ = reFluxJ.GetMatch(strWxLine, 1);

		} else if( reFluxH.Matches( strWxLine ) )
		{
			m_strFluxH = reFluxH.GetMatch(strWxLine, 1);

		} else if( reFluxK.Matches( strWxLine ) )
		{
			m_strFluxK = reFluxK.GetMatch(strWxLine, 1);

		} else if( reSpectralType.Matches( strWxLine ) )
		{
			m_strSpectralType.Printf( wxT("%s"), reSpectralType.GetMatch(strWxLine, 1 ) );

		} else if( reMorphologicalType.Matches( strWxLine ) )
		{
			m_strMorphologicalType = reMorphologicalType.GetMatch(strWxLine, 1 ).Trim(0).Trim(1);

		} else if( reAngularSize.Matches( strWxLine ) )
		{
			m_strAngSize = reAngularSize.GetMatch(strWxLine, 1 ) + wxT(" ") +
									reAngularSize.GetMatch(strWxLine,  2 ) + wxT(" ") +
									reAngularSize.GetMatch(strWxLine, 3 );

		} else if( reIdentifiers.Matches( strWxLine ) )
		{
			if( bIdentifiers == 0 )
			{
				reIdentifiers.GetMatch(strWxLine, 1 ).ToLong( &nIdentifiers );
				// set section flags
				bIdentifiers = 1;
				bMeasures = 0;
				bNotes = 0;
				bBibcodes = 0;
				
				continue;
			}

		} else if( reBibcodes.Matches( strWxLine ) )
		{
//			m_pRefBibcode->Clear( );

			bIdentifiers = 0;
			bMeasures = 0;
			bNotes = 0;
			bBibcodes = 1;

			continue;

		} else if( reMeasures.Matches( strWxLine ) )
		{
			// reset counters
			m_nMeasuresTable = 0;
//			m_pMeasuresName->Clear( );;

			// set flags
			bIdentifiers = 0;
			bMeasures = 1;
			bNotes = 0;
			bBibcodes = 0;

			continue;

		} else if( reNotes.Matches( strWxLine ) )
		{
			// check if there are more then just zero notes
			long nNotes = 0;
			reNotes.GetMatch(strWxLine, 1 ).ToLong( &nNotes );
			if( nNotes > 0 )
			{
				// clear page 
				m_strNotesPage = wxT("<html><body><table width='100%' HEIGHT='100%' border=0><tr valign='CENTER'><td align='left' VALIGN='CENTER'><UL TYPE=DISC>");

				// show panel

				// set flag
				bNotes = 1;
			}
			// reset othe flags
			bIdentifiers = 0;
			bMeasures = 0;
			bBibcodes = 0;

			continue;
		} 

		////////////////////////////////////////////////////////////
		// read sections ie: indentifiers, bibcodes, measures, notes
		if( bIdentifiers == 1 )
		{
			while( reIdentifiersWord.Matches( strWxLine ) )
			{
				m_vectIdentifiersCat[m_nIdentifiers] = reIdentifiersWord.GetMatch( strWxLine, 1 );
				m_vectIdentifiersName[m_nIdentifiers] = reIdentifiersWord.GetMatch( strWxLine, 3 );
				// remove string fromline
				wxString strIdentifier = m_vectIdentifiersCat[m_nIdentifiers] + reIdentifiersWord.GetMatch( strWxLine, 2 ) + m_vectIdentifiersName[m_nIdentifiers];
				strWxLine.Replace( strIdentifier, wxT(""), false );
				// add to identifiers to the sizer
//				wxString strIdentifierFull = m_vectIdentifiersCat[m_nIdentifiers] + ": " + m_vectIdentifiersName[m_nIdentifiers];
//				wxMutexGuiEnter();
//				m_pIdentifiersDataSizer->Add( new wxStaticText( m_pPanelIdentifiers, -1, m_vectIdentifiersCat[m_nIdentifiers] + ": "), 0,
//											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
//				m_pIdentifiersDataSizer->Add( new wxStaticText( m_pPanelIdentifiers, -1, m_vectIdentifiersName[m_nIdentifiers]), 0,
//											wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
//				wxMutexGuiLeave();

				//m_pIdentifiersCat->Append( m_vectIdentifiersCat[m_nIdentifiers] );
				//m_pIdentifiersName->Append( m_vectIdentifiersName[m_nIdentifiers] );

				// increment counter
				m_nIdentifiers++;
			}
//			m_pIdentifiersCat->SetSelection( 0 );
//			m_pIdentifiersName->SetSelection( 0 );

//			GetSizer()->Layout( );
//			Fit( );
			if( m_nIdentifiers > 0 ) m_bHasIdentifiers = 1;


		} else if( bBibcodes == 1 )
		{
			while( reBibcodesWord.Matches( strWxLine ) )
			{
				// check for limit allocated
				if( m_pBibDb->m_nBibCode >= 4000 ) break;

				m_pBibDb->m_vectBibCode[m_pBibDb->m_nBibCode] = reBibcodesWord.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
				// remove string fromline
				strWxLine.Replace( m_pBibDb->m_vectBibCode[m_pBibDb->m_nBibCode], wxT(""), false );

				// increment counter
				 m_pBibDb->m_nBibCode++;
			}

		} else if( bMeasures == 1 )
		{
			if( reMeasuresName.Matches( strWxLine ) )
			{
				bMeasuresHeader = 1;
				nMesTableNo = m_nMeasuresTable;
				//m_vectMeasuresTableName[nMesTableNo]
				m_vectMeasuresName.push_back( reMeasuresName.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ) );
				// increment counter
				m_nMeasuresTable++;
				// set to zero rows/cols
				m_nMeasuresTableCol[nMesTableNo] = 0;
				m_nMeasuresTableRow[nMesTableNo] = 0;

			} else if( reMeasuresTableLine.Matches( strWxLine ) )
			{
				wxString strTableLine = reMeasuresTableLine.GetMatch( strWxLine, 2 ).Trim( 1 ).Trim( 0 );
				int nMesTableColId = 0;
				// check if first line - ie header
				if( bMeasuresHeader == 1 )
				{
					bMeasuresHeader = 0;

					while( reMeasuresTableLineWord.Matches( strTableLine ) )
					{
						// Set header
						wxString strTableLineWord = reMeasuresTableLineWord.GetMatch( strTableLine, 1 );
						m_vectMeasuresTableHead[nMesTableNo][nMesTableColId] = strTableLineWord;
						m_vectMeasuresTableHead[nMesTableNo][nMesTableColId].Trim( 1 ).Trim( 0 );
						// remove string fromline
						strTableLineWord += wxT("|");
						strTableLine.Replace( strTableLineWord, wxT(""), false );
						// increment counter
						nMesTableColId++;
					}
					m_nMeasuresTableCol[nMesTableNo] = nMesTableColId;

				} else
				{
					int nMesTableLineId = m_nMeasuresTableRow[nMesTableNo];

					while( reMeasuresTableLineWord.Matches( strTableLine ) )
					{
						// Set table data
						wxString strTableLineWord = reMeasuresTableLineWord.GetMatch( strTableLine, 1 );
						m_vectMeasuresTableValues[nMesTableNo][nMesTableLineId][nMesTableColId] = strTableLineWord;
						m_vectMeasuresTableValues[nMesTableNo][nMesTableLineId][nMesTableColId].Trim( 1 ).Trim( 0 );
						strTableLineWord += wxT("|");
						// remove string fromline
						strTableLine.Replace( strTableLineWord, wxT(""), false );
						// increment counter
						nMesTableColId++;
					}
					// increment line
					m_nMeasuresTableRow[nMesTableNo]++;

				}
			}

		} else if( bNotes == 1 )
		{
			if( ! reEndOfPage.Matches( strWxLine ) ) 
			{
				wxString strNoteLine(strLine,wxConvUTF8);
				// check for object tag ie:
				// http://simbad.u-strasbg.fr/simbad/sim-id?Ident=CCDM%20J05408-0156AB
				while( reObjectTag.Matches( strNoteLine ) )
				{
					wxString strObjectTag = reObjectTag.GetMatch( strNoteLine, 2 );
					wxString strNoteLineHead = reObjectTag.GetMatch( strNoteLine, 1 );
					wxString strNoteLineTail = reObjectTag.GetMatch( strNoteLine, 3 );

					strNoteLine = strNoteLineHead +
								wxT("<a href=\"http://simbad.u-strasbg.fr/simbad/sim-id?Ident=") + 
								URLEncode( strObjectTag ) + wxT("\">") + strObjectTag + wxT("</a>") +
								strNoteLineTail;
				} 

				// check for star of notes line
				if( reNotesLine.Matches( strNoteLine ) )
				{
					m_strNotesPage += wxT("<li>") + reNotesLine.GetMatch( strNoteLine, 1 ) + wxT("<br>\n");
					m_bHasNotes = 1;
				} else
					m_strNotesPage += strNoteLine + wxT("<br>\n");
			} else
				bNotes = 0;
		} 
	}

	// close my file
	fclose( pLocalFile );

	return( 1 );
}
