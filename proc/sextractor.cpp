////////////////////////////////////////////////////////////////////
// File:		se-xtractor.cpp
// Class:		CSextractor
// Purpose:		detect objects in image
////////////////////////////////////////////////////////////////////

//system headers
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/image.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// include local headers
#include "../config/config.h"
#include "../config/detectconfig.h"
#include "../config/mainconfig.h"

// main header
#define NOCTYPES
	#include "sextractor.h"
#undef NOCTYPES

// inculde the static init header
#include "init_static.h"

// c++ local headers
#include "../logger/logger.h"
#include "../image/astroimage.h"
#include "../gui/astro_img_view.h"
#include "../unimap_worker.h"

////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CSextractor
// Purpose:		build my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CSextractor::CSextractor( )
{
	int i =0;

	m_nKey = 0;
	m_nObjKey = 0;
	m_nHeadKey = 0;
	m_nHeadKeyOne = 0;

	objtab = NULL;
	catopen_flag = 0;
	objlistRefine = NULL;

	outobj2.flux_aper = NULL;

	xmlstack = NULL;
	nxml=0, nxmlmax=0;

	InitPrefsStruct( prefs );
	InitCatStruct( thecat );
	// init my objects
	InitObjStruct( outobj );
	InitObjTwoStruct( outobj2 );
	// init lists of keys
	InitObjKeyList( );
	InitHeadKeyList( );
	InitHeadKeyOneList( );
	InitKeyList( );
	// initializa catalog params
	InitCatParams( );

	int nElem = sizeof(key)/sizeof(pkeystruct);
	keylist = (char**) calloc( nElem, sizeof(char*) );
	for( i=0;i<nElem; i++ )
	{
		keylist[i] = (char*) malloc( 32*sizeof(char) );
	}

	m_bIsDataInit = 1;
	// init other by larry
	m_nImageChannels = 2; // default green
	m_bUseAreasToMask = 1; // default to use
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CSextractor
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CSextractor::~CSextractor()
{
	// close / free data
//	ClosePrefs( );
	Close( );

//	if( outobj2.flux_aper )
//	{
//		free( outobj2.flux_aper );
//		outobj2.flux_aper= NULL;
//	}

//	if( objlist ) free( objlist );
//	if( objlist->obj && )
//	{
//		free( objlist->obj );
//		objlist->obj = NULL;
//	}

//	free( key );
}

////////////////////////////////////////////////////////////////////
// Method:		Close
// Class:		CSextractor
// Purpose:		close object free data
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Close( )
{
	int i =0;

	ClosePrefs( );

	if( keylist )
	{
		int nElem = sizeof(key)/sizeof(pkeystruct);
		for( i=0;i<nElem; i++ )
		{
			if( keylist[i] )
			{
				free( keylist[i] );
				keylist[i] = NULL;
			}
		}
		free( keylist );
		keylist = NULL;
	}

	// also free image data
	m_pAstroImage->FreeImageData( );

	m_bIsDataInit = 0;
}

////////////////////////////////////////////////////////////////////
// Method:		InitData
// Class:		CSextractor
// Purpose:		initialize my object data for another run ?
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitData( )
{
	if( m_bIsDataInit ) return;

	int i=0;

	m_nKey = 0;
	m_nObjKey = 0;
	m_nHeadKey = 0;
	m_nHeadKeyOne = 0;

	objtab = NULL;
	catopen_flag = 0;
	objlistRefine = NULL;

	outobj2.flux_aper = NULL;

	xmlstack = NULL;
	nxml=0, nxmlmax=0;

	// init my objects
	InitObjStruct( outobj );
	InitObjTwoStruct( outobj2 );
	// init lists of keys
	InitObjKeyList( );
	InitHeadKeyList( );
	InitHeadKeyOneList( );
	InitKeyList( );
	// initializa catalog params
	InitCatParams( );

	int nElem = sizeof(key)/sizeof(pkeystruct);
	keylist = (char**) calloc( nElem, sizeof(char*) );
	for( i=0;i<nElem; i++ )
	{
		keylist[i] = (char*) malloc( 32*sizeof(char) );
	}

	m_bIsDataInit = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Init
// Class:		CSextractor
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Init( )
{
	prefs.pipe_flag = 0;
	prefs.nimage_name = 1;
	prefs.image_name[0] = "image";

// larry :: disable this config
//	strcpy(prefs.prefs_name, "default.sex");

	// this is the original for preferences
//	readprefs(prefs.prefs_name, argkey, argval, narg);

// larry :: is is now disables
//	ReadPrefs(prefs.prefs_name, NULL, NULL, 0);
	
	// this the new method to set prefeces to replace the one above
	GetConfig( );
}

////////////////////////////////////////////////////////////////////
// Method:		GetConfig
// Class:		CSextractor
// Purpose:		get configuration from detect config object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::GetConfig( )
{
	int nVarInt = 0, nProfileId = 0;
	wxString strVar=wxT("");

	// first get profile id
	m_pMainConfig->GetIntVar( SECTION_ID_DETECTION, CONF_DETECTION_PROFILE, nProfileId, 0 );
	DefConfigDetector* profile = &(m_pDetectConfig->m_vectProfile[nProfileId]);

	////////////////////////
	// SECTION :: DETECT
	// :: image channels to use
	m_nImageChannels = profile->ImageChannels;
	// :: use areas to mask
	m_bUseAreasToMask = profile->MaskAreas;
	// detect type
	if( profile->ImageType == 0 )
		prefs.detect_type = CCD;
	else if( profile->ImageType == 1 )
		prefs.detect_type = PHOTO;
	// set minumin area in pixels
	prefs.ext_minarea = profile->DetectMinArea;
	// set detection threshold if has one vale or two
	prefs.ndthresh = 1;
	// if one param
	if( prefs.ndthresh == 1 )
		prefs.dthresh[0] = profile->DetectThresh;
	// set analysis threshold if has one vale or two
	prefs.nthresh = 1;
	if( prefs.nthresh == 1 )
	{
		prefs.thresh[0] = profile->AnalysisThresh;
		//prefs.thresh[1] = prefs.thresh[0];
	}
	// get deblending sub thresh
	prefs.deblend_nthresh = profile->DeblendNThresh;
	// get deblending contrast
	prefs.deblend_mincont = profile->DeblendNMinCont;
	// clean
	prefs.clean_flag = profile->DetectClean;
	// clean param
	prefs.clean_param = profile->CleanParam;
	/////////////////////////////////////////////////////
	// SECTION :: Processing
	// is filter ?
	prefs.filter_flag = profile->isFilter;
	// get filter id
	prefs.filter_id = profile->FilterName;
	// filter treshold
	// here I shoudl probably use two params option ?
	prefs.nfilter_thresh = 1;
	prefs.filter_thresh[0] = profile->FilterThresh;
	// get mask type
	if( profile->MaskType == 0 )
		prefs.mask_type = MASK_NONE;
	else if( profile->MaskType == 1 )
		prefs.mask_type = MASK_BLANK;
	else if( profile->MaskType == 2 )
		prefs.mask_type = MASK_CORRECT;
	// get thresh type
	prefs.nthresh_type = 1;
	nVarInt = profile->ThreshType;
	if( nVarInt == 0 )
		prefs.thresh_type[0] = THRESH_RELATIVE;
	else if( nVarInt == 1 )
		prefs.thresh_type[0] = THRESH_ABSOLUTE;
	// get weight type
	// set to 1 param
	prefs.nweight_type = 1;
	// get select match by type
	if( profile->WeightType == 0 )
		prefs.weight_type[0] = WEIGHT_NONE;
	else if( profile->WeightType == 1 )
		prefs.weight_type[0] = WEIGHT_FROMBACK;
	else if( profile->WeightType == 2 )
		prefs.weight_type[0] = WEIGHT_FROMRMSMAP;
	else if( profile->WeightType == 3 )
		prefs.weight_type[0] = WEIGHT_FROMVARMAP;
	else if( profile->WeightType == 4 )
		prefs.weight_type[0] = WEIGHT_FROMWEIGHTMAP;
	else if( profile->WeightType == 5 )
		prefs.weight_type[0] = WEIGHT_FROMINTERP;
	// get ScanIsoApRatio
	//prefs.??? = profile->ScanIsoApRatio;
	// set Blank
	prefs.blank_flag = profile->Blank;

	//////////////////////////////////////////////////////////////
	// SECTION :: PHOTOMETRY
	// get Phot Apertures
	prefs.mag_apersize = profile->PhotApertures;
	// get PhotAutoparamKron
	prefs.nautoparam = 2;
	prefs.autoparam[0] = profile->PhotAutoparamKron;
	// get PhotAutoparamMinRad
	prefs.autoparam[1] = profile->PhotAutoparamMinRad;
	// get PhotPetroparamFact
	prefs.npetroparam = 2;
	prefs.petroparam[0] = profile->PhotPetroparamFact;
	// get PhotPetroparamMinRad
	prefs.petroparam[1] = profile->PhotPetroparamMinRad;
	// get SaturationLevel
	prefs.satur_level = profile->SaturationLevel;
	// get MagZeroPoint
	prefs.mag_zeropoint = profile->MagZeroPoint;
	// get MagGamma
	prefs.mag_gamma = profile->MagGamma;
	// get Gain
	prefs.gain = profile->Gain;
	// get PixelScale - this in tab seeing
	prefs.pixel_scale = profile->PixelScale;

	//////////////////////////////////////////////////////////////
	// SECTION :: CLASSIFICATION
	// get SeeingFwhm
	prefs.seeing_fwhm = profile->SeeingFwhm;
	// get StarNnwName
	strVar = profile->NNClassName;
	strcpy( prefs.nnw_name, strVar.ToAscii() );
	// get BackSize
	prefs.nbacksize = 1;
	prefs.backsize[0] = profile->BackSize;
	// get BackFilterSize
	prefs.nbackfsize = 1;
	prefs.backfsize[0] = profile->BackFilterSize;
	// get BackPhotoType
	nVarInt = profile->BackPhotoType;
	if( nVarInt == 0 )
		prefs.pback_type = GLOBAL;
	else if( nVarInt == 1 )
		prefs.pback_type = LOCAL;
	// get BackFilterThresh
	prefs.backfthresh = profile->BackFilterThresh;
	// get CheckImageType
	prefs.ncheck_type = 1;
	nVarInt = profile->CheckImageType;
	// map by type
	if( nVarInt == 0 )
	{
		prefs.check_type[0] = CHECK_NONE;
		prefs.ncheck_type = 0;

	} else if( nVarInt == 1 )
		prefs.check_type[0] = CHECK_IDENTICAL;
	else if( nVarInt == 2 )
		prefs.check_type[0] = CHECK_BACKGROUND;
	else if( nVarInt == 3 )
		prefs.check_type[0] = CHECK_MINIBACKGROUND;
	else if( nVarInt == 4 )
		prefs.check_type[0] = CHECK_MINIBACKRMS;
	else if( nVarInt == 5 )
		prefs.check_type[0] = CHECK_SUBTRACTED;
	else if( nVarInt == 6 )
		prefs.check_type[0] = CHECK_FILTERED;
	else if( nVarInt == 7 )
		prefs.check_type[0] = CHECK_OBJECTS;
	else if( nVarInt == 8 )
		prefs.check_type[0] = CHECK_APERTURES;
	else if( nVarInt == 9 )
		prefs.check_type[0] = CHECK_SEGMENTATION;
	else if( nVarInt == 10 )
		prefs.check_type[0] = CHECK_ASSOC;
	else if( nVarInt == 11 )
		prefs.check_type[0] = CHECK_SUBOBJECTS;
	else if( nVarInt == 12 )
		prefs.check_type[0] = CHECK_SUBPSFPROTOS;
	else if( nVarInt == 13 )
		prefs.check_type[0] = CHECK_PSFPROTOS;
	else if( nVarInt == 14 )
		prefs.check_type[0] = CHECK_SUBPCPROTOS;
	else if( nVarInt == 15 )
		prefs.check_type[0] = CHECK_PCPROTOS;
	else if( nVarInt == 16 )
		prefs.check_type[0] = CHECK_PCOPROTOS;
	else if( nVarInt == 17 )
		prefs.check_type[0] = CHECK_MAPSOM;
	// get CheckImageName
	if( !strVar.IsEmpty() && prefs.check_type[0] != CHECK_NONE )
	{
		prefs.ncheck_name = 1;
		prefs.check_name[0] = (char*) malloc( sizeof( strVar.Length()+1 ) );
		strcpy( prefs.check_name[0], strVar.ToAscii() );
	} else
		prefs.ncheck_name = 0;

	////////////////////////
	// SECTION :: OTHER GLOBALS
	// get MemObjStack
	prefs.clean_stacksize = m_pDetectConfig->m_nMemObjStack;
	// get MemPixStack
	prefs.mem_pixstack = m_pDetectConfig->m_nMemPixStack;
	// get MemBufSize
	prefs.mem_bufsize = m_pDetectConfig->m_nMemBufSize;
	// get VerboseType
	nVarInt = m_pDetectConfig->m_nVerboseType;
	if( nVarInt == 0 )
		prefs.verbose_type = QUIET;
	else if( nVarInt == 1 )
		prefs.verbose_type = NORM;
	else if( nVarInt == 2 )
		prefs.verbose_type = WARN;
	else if( nVarInt == 3 )
		prefs.verbose_type = FULL;
	// get WriteXml
	prefs.xml_flag = m_pDetectConfig->m_bWriteXmlFile;
	// get XmlName
	strcpy( prefs.xml_name, m_pDetectConfig->m_strXmlFileName.ToAscii() );

	////////////////////////////////
	// larry :: hard code for now params file
	strcpy( prefs.param_name, "default.param" );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitGlob
// Class:		CSextractor
// Purpose:		Manage the whole stuff
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitGlob( )
{	
	int	i;
	// init trigonometric functions values
	for (i=0; i<37; i++)
	{
		ctg[i] = (float) cos(i*PI/18);
		stg[i] = (float) sin(i*PI/18);
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Make
// Class:		CSextractor
// Purpose:		Manage the whole stuff
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Make( CAstroImage* pImg )
{	
	checkstruct		*check;
	picstruct		*dfield, *field,*pffield[MAXFLAG], *wfield,*dwfield;
//	catstruct		*imacat;
//	tabstruct		*imatab;
	static time_t	thetime1, thetime2;
	struct tm		*tm;
	int				i=0, nok=0, ntab=0, next=0;
	wxString strLog;

	m_pAstroImage = pImg;

	// extract data  with channel and area mask param
	m_pAstroImage->GetImageData( m_nImageChannels, m_bUseAreasToMask );

	// Install error logging - don't know what this is so maybe i 
	// should remove it - also remove function WriteError!! LARRY
//	error_installfunc( WriteError );

	// Processing start date and time
	thetimet = time(NULL);
	tm = localtime(&thetimet);
	sprintf(prefs.sdate_start,"%04d-%02d-%02d",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
	sprintf(prefs.stime_start,"%02d:%02d:%02d",
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	//	Log time when detection has started
	strLog.Printf( wxT("INFO :: Detection started on %s at %s running on %d thread%s"),
						wxString(prefs.sdate_start,wxConvUTF8), wxString(prefs.stime_start,wxConvUTF8), prefs.nthreads,
						prefs.nthreads>1 ? wxT("s") : wxT("") );
	m_pUnimapWorker->Log( strLog );

	// Initialize globals variables
	InitGlob( );

	m_pUnimapWorker->Log( wxT("INFO :: Setting catalog parameters") );
	// this orig is out 
	// ReadCatParams(prefs.param_name);
	// use hard code cat param init instead
	ReadCatParamsHardcode( );

	// update things accor. to prefs parameters 
	UsePrefs( );			

	if( prefs.psf_flag )
	{
		m_pUnimapWorker->Log( wxT("INFO :: Reading PSF information") );

		thepsf = PsfLoad(prefs.psf_name[0]); 
		if( prefs.dpsf_flag ) ppsf = PsfLoad(prefs.psf_name[1]);
		// Need to check things up because of PSF context parameters 
		UpdateParamFlags( );
		UsePrefs( );
	}

	if( prefs.filter_flag )
	{
		m_pUnimapWorker->Log( wxT("INFO :: Reading detection filter") );
		// get the detection filter 
		GetFilter( prefs.filter_id, prefs.filter_name );	
	}

	///////////////////////////
	// Neural network classification engine
	if( FLAG( obj2.sprob ) )
	{
		m_pUnimapWorker->Log( wxT("INFO :: Initialize classification engine...") );
		NeurInit();
		m_pUnimapWorker->Log( wxT("INFO :: Reading classification engine configuration...") );
		//GetNnw(); - prefs.nnw_name
		wxString str_name(prefs.nnw_name,wxConvUTF8);
		m_pDetectConfig->LoadNNConfig( str_name, brain );
	}

	if( prefs.somfit_flag )
	{
		int	margin;

		thesom = SomLoad(prefs.som_name);

		if( (margin=(thesom->inputsize[1]+1)/2) > prefs.cleanmargin )
			prefs.cleanmargin = margin;

		if( prefs.somfit_vectorsize>thesom->neurdim )
		{
			prefs.somfit_vectorsize = thesom->neurdim;
			sprintf(gstr,"%d", prefs.somfit_vectorsize);
//			warning("Dimensionality of the SOM-fit vector limited to ", gstr);
		}
	}

	// Prepare growth-curve buffer 
	if( prefs.growth_flag ) InitGrowth( );

	// Compute the number of valid input extensions
//	if( !(imacat = read_cat(prefs.image_name[0])) )
//		error(EXIT_FAILURE, "*Error*: cannot open ", prefs.image_name[0]);
//
//	close_cat(imacat);
//	imatab = imacat->tab;
//	next = 0;
//	for (ntab = 0 ; ntab<imacat->ntab; ntab++, imatab = imatab->nexttab)
//	{
//		// Check for the next valid image extension
//		if( (imatab->naxis < 2) ||
//			!strncmp(imatab->xtension, "BINTABLE", 8) ||
//			!strncmp(imatab->xtension, "ASCTABLE", 8) ) continue;
//
//		next++;
//	}
//
//	thecat.next = next;

	// Init the CHECK-images
	if (prefs.check_flag)
	{
		checkenum	c;

//		NFPRINTF(OUTPUT, "Initializing check-image(s)");
		for (i=0; i<prefs.ncheck_type; i++)
		{
			if ((c=prefs.check_type[i]) != CHECK_NONE)
			{
//				if (prefs.check[c])
//					error(EXIT_FAILURE,"*Error*: 2 CHECK_IMAGEs cannot have the same ",
//							" CHECK_IMAGE_TYPE");

				prefs.check[c] = InitCheck(prefs.check_name[i], prefs.check_type[i], next);
				free(prefs.check_name[i]);
			}
		}
	}

//	NFPRINTF(OUTPUT, "Initializing catalog");
//	initcat();

////////////////////////////////////////////////////////////
// Larry's new code

	dfield = field = wfield = dwfield = NULL;

	if( prefs.dimage_flag )
	{
		///////////////// here info is extracted ???????????????
		// Init the Detection and Measurement-images 
//		dfield = newfield(prefs.image_name[0], DETECT_FIELD, nok);
//		field = newfield(prefs.image_name[1], MEASURE_FIELD, nok);
		dfield = NewField( m_pAstroImage, DETECT_FIELD );
		field = NewField( m_pAstroImage, MEASURE_FIELD );

		if( (field->width!=dfield->width) || (field->height!=dfield->height) )
			error(EXIT_FAILURE, "*Error*: Frames have different sizes","");

		// Prepare interpolation 
		if( prefs.dweight_flag && prefs.interp_type[0] == INTERP_ALL ) 
			InitInterpolate(dfield, -1, -1);

		if( prefs.interp_type[1] == INTERP_ALL ) InitInterpolate(field, -1, -1);

	} else
	{
//		field = newfield(prefs.image_name[0], DETECT_FIELD | MEASURE_FIELD, nok);
//		// Prepare interpolation - 0.0 or anything else
//		if( (prefs.dweight_flag || prefs.weight_flag) &&
//			prefs.interp_type[0] == INTERP_ALL) init_interpolate(field, -1, -1);


		// init field image 
		field = NewField( m_pAstroImage, DETECT_FIELD | MEASURE_FIELD );
		// check interpolation ?
		if( (prefs.dweight_flag || prefs.weight_flag) &&
					prefs.interp_type[0] == INTERP_ALL) InitInterpolate(field, -1, -1); 
	}

	// Init the FLAG-images
	for( i=0; i<prefs.nimaflag; i++ )
	{
//		pffield[i] = newfield(prefs.fimage_name[i], FLAG_FIELD, nok);
		pffield[i] = NewField( m_pAstroImage, FLAG_FIELD );

		if( (pffield[i]->width!=field->width) ||
			(pffield[i]->height!=field->height))
		{
			error(EXIT_FAILURE, "*Error*: Incompatible FLAG-map size in ", 
								prefs.fimage_name[i] );
		}
	}

	// Compute background maps for `standard' fields
//		QPRINTF(OUTPUT, dfield? "Measurement image:"
//			: "Detection+Measurement image: ");
	MakeBack(field, wfield);
//		QPRINTF(OUTPUT, (dfield || (dwfield&&dwfield->flags^INTERP_FIELD))? "(M)   "
//			"Background: %-10g RMS: %-10g / Threshold: %-10g \n"
//			: "(M+D) "
//			"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//			field->backmean, field->backsig, (field->flags & DETECT_FIELD)?
//			field->dthresh: field->thresh);
	if( dfield )
	{
//			QPRINTF(OUTPUT, "Detection image: ");
		MakeBack( dfield, dwfield? dwfield
				: (prefs.weight_type[0] == WEIGHT_NONE?NULL:wfield));
//			QPRINTF(OUTPUT, "(D)   "
//					"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//			dfield->backmean, dfield->backsig, dfield->dthresh);
	} else if( dwfield && dwfield->flags^INTERP_FIELD )
	{
		MakeBack(field, dwfield);
//			QPRINTF(OUTPUT, "(D)   "
//					"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//					field->backmean, field->backsig, field->dthresh);
	}

	// For interpolated weight-maps, copy the background structure 
	if( dwfield && dwfield->flags&(INTERP_FIELD|BACKRMS_FIELD) )
		CopyBack(dwfield->reffield, dwfield);
	if( wfield && wfield->flags&(INTERP_FIELD|BACKRMS_FIELD) )
		CopyBack(wfield->reffield, wfield);

	// Prepare learn and/or associations 
	if( prefs.assoc_flag ) InitAssoc( field ); // initialize assoc tasks 

	// Update the CHECK-images
	if( prefs.check_flag )
		for (i=0; i<MAXCHECK; i++)
			if ((check=prefs.check[i]))
				ReInitCheck(field, check);

	// Initialize PSF contexts and workspace
	if( prefs.psf_flag )
	{
		PsfReadContext(thepsf, field);
		PsfInit(thepsf);
		if (prefs.dpsf_flag)
		{
			PsfReadContext( thepsf, dfield );
			PsfInit( thepsf ); // ???
		}
	}

	// Copy field structures to static ones (for catalog info)
	if( dfield )
	{
		thefield1 = *field;
		thefield2 = *dfield;
	} else
		thefield1 = thefield2 = *field;

	if( wfield )
	{
		thewfield1 = *wfield;
		thewfield2 = dwfield? *dwfield: *wfield;
	}
	else if (dwfield)
		thewfield2 = *dwfield;

	ReInitCat( field );

	//////////////////////////:: SCANIMAGE ENGINE ::///////////////////////
	// Start the extraction pipeline 
//		NFPRINTF(OUTPUT, "Scanning image");
//	scanimage( field, dfield, pffield, prefs.nimaflag, wfield, dwfield );
	ScanImage( field, dfield, pffield, prefs.nimaflag, wfield, dwfield );
	///////////////////////////////////////////////////////////////////////


	// Finish the current CHECK-image processing
	if (prefs.check_flag)
		for (i=0; i<MAXCHECK; i++)
			if ((check=prefs.check[i]))
				ReEndCheck(field, check);

	ReEndCat();

	/////////////////////// :: probably here i learn how tp extract my data
	// Update XML data
//	if( prefs.xml_flag || prefs.cat_type==ASCII_VO )
//	{
//		update_xml(&thecat, dfield? dfield:field, field,
//		dwfield? dwfield:wfield, wfield);
//	}


	// Close ASSOC routines 
	EndAssoc(field);

	for (i=0; i<prefs.nimaflag; i++)
		EndField(pffield[i]);

	EndField(field);

	if (dfield) EndField(dfield);
	if (wfield) EndField(wfield);
	if (dwfield) EndField(dwfield);

	//////////////// here is from where to get the objects detected
	QPRINTF(OUTPUT, "Objects: detected %-8d / sextracted %-8d               \n",
			thecat.ndetect, thecat.ntotal);




///////////////////////////////////////////////////////////
// from here comment and re-write a new call routine to 
// work directly wi the image
/*
	// Initialize XML data
	if( prefs.xml_flag || prefs.cat_type == ASCII_VO ) init_xml( next );

	/////////////////////////////// :: perhaps here ??? //////////////
	// Go through all images
	nok = -1;
	for( ntab = 0 ; ntab<imacat->ntab; ntab++, imatab = imatab->nexttab )
    {
		// Check for the next valid image extension
		if( (imatab->naxis < 2) ||
			!strncmp(imatab->xtension, "BINTABLE", 8) ||
			!strncmp(imatab->xtension, "ASCTABLE", 8) ) continue;

		nok++;

		// Initial time measurement
		time(&thetime1);
		thecat.currext = nok+1;

		dfield = field = wfield = dwfield = NULL;

		if( prefs.dimage_flag )
		{
			///////////////// here info is extracted ???????????????
			// Init the Detection and Measurement-images 
			dfield = newfield(prefs.image_name[0], DETECT_FIELD, nok);
			field = newfield(prefs.image_name[1], MEASURE_FIELD, nok);

			if( (field->width!=dfield->width) || (field->height!=dfield->height) )
				error(EXIT_FAILURE, "*Error*: Frames have different sizes","");

			// Prepare interpolation 
			if( prefs.dweight_flag && prefs.interp_type[0] == INTERP_ALL ) 
				init_interpolate(dfield, -1, -1);

			if( prefs.interp_type[1] == INTERP_ALL ) init_interpolate(field, -1, -1);

		} else
		{
			field = newfield(prefs.image_name[0], DETECT_FIELD | MEASURE_FIELD, nok);
			// Prepare interpolation - 0.0 or anything else
			if( (prefs.dweight_flag || prefs.weight_flag) &&
				prefs.interp_type[0] == INTERP_ALL) init_interpolate(field, -1, -1); 
		}

		// Init the WEIGHT-images
		if( prefs.dweight_flag || prefs.weight_flag ) 
		{
			weightenum	wtype;
			PIXTYPE	interpthresh;

			if (prefs.nweight_type>1)
			{
				// Double-weight-map mode 
				if (prefs.weight_type[1] != WEIGHT_NONE)
				{
					// First: the "measurement" weights
					wfield = newweight(prefs.wimage_name[1],field,prefs.weight_type[1], nok);
					wtype = prefs.weight_type[1];
					interpthresh = prefs.weight_thresh[1];
					// Convert the interpolation threshold to variance units
					weight_to_var(wfield, &interpthresh, 1);
					wfield->weight_thresh = interpthresh;
					if( prefs.interp_type[1] != INTERP_NONE)
					{
						init_interpolate(wfield, prefs.interp_xtimeout[1], 
											prefs.interp_ytimeout[1]);
					}
				}

				// The "detection" weights
				if (prefs.weight_type[0] != WEIGHT_NONE)
				{
					interpthresh = prefs.weight_thresh[0];
					if (prefs.weight_type[0] == WEIGHT_FROMINTERP)
					{
						dwfield=newweight(prefs.wimage_name[0],wfield,prefs.weight_type[0], nok);
						weight_to_var(wfield, &interpthresh, 1);
					} else
					{
						dwfield = newweight(prefs.wimage_name[0], dfield?dfield:field,
						prefs.weight_type[0], nok);
						weight_to_var(dwfield, &interpthresh, 1);
					}

					dwfield->weight_thresh = interpthresh;
					if( prefs.interp_type[0] != INTERP_NONE )
					{
						init_interpolate(dwfield, prefs.interp_xtimeout[0], 
											prefs.interp_ytimeout[0]);
					}
				}
			} else
			{
				/////////////// here maybe is my case ??????????
				// Single-weight-map mode 
				wfield = newweight(prefs.wimage_name[0], dfield?dfield:field,
									prefs.weight_type[0], nok );
				wtype = prefs.weight_type[0];
				interpthresh = prefs.weight_thresh[0];
				// Convert the interpolation threshold to variance units
				weight_to_var(wfield, &interpthresh, 1);
				wfield->weight_thresh = interpthresh;
				if( prefs.interp_type[0] != INTERP_NONE )
				{
					init_interpolate(wfield, prefs.interp_xtimeout[0], 
												prefs.interp_ytimeout[0]);
				}
			}
		}

		// Init the FLAG-images
		for( i=0; i<prefs.nimaflag; i++ )
		{
			pffield[i] = newfield(prefs.fimage_name[i], FLAG_FIELD, nok);

			if( (pffield[i]->width!=field->width) ||
				(pffield[i]->height!=field->height))
			{
				error(EXIT_FAILURE, "*Error*: Incompatible FLAG-map size in ", 
									prefs.fimage_name[i] );
			}
		}

		// Compute background maps for `standard' fields
//		QPRINTF(OUTPUT, dfield? "Measurement image:"
//			: "Detection+Measurement image: ");
		makeback(field, wfield);
//		QPRINTF(OUTPUT, (dfield || (dwfield&&dwfield->flags^INTERP_FIELD))? "(M)   "
//			"Background: %-10g RMS: %-10g / Threshold: %-10g \n"
//			: "(M+D) "
//			"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//			field->backmean, field->backsig, (field->flags & DETECT_FIELD)?
//			field->dthresh: field->thresh);
		if( dfield )
		{
//			QPRINTF(OUTPUT, "Detection image: ");
			makeback( dfield, dwfield? dwfield
					: (prefs.weight_type[0] == WEIGHT_NONE?NULL:wfield));
//			QPRINTF(OUTPUT, "(D)   "
//					"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//			dfield->backmean, dfield->backsig, dfield->dthresh);
		} else if( dwfield && dwfield->flags^INTERP_FIELD )
		{
			makeback(field, dwfield);
//			QPRINTF(OUTPUT, "(D)   "
//					"Background: %-10g RMS: %-10g / Threshold: %-10g \n",
//					field->backmean, field->backsig, field->dthresh);
		}

		// For interpolated weight-maps, copy the background structure 
		if( dwfield && dwfield->flags&(INTERP_FIELD|BACKRMS_FIELD) )
			copyback(dwfield->reffield, dwfield);
		if( wfield && wfield->flags&(INTERP_FIELD|BACKRMS_FIELD) )
			copyback(wfield->reffield, wfield);

		// Prepare learn and/or associations 
		if( prefs.assoc_flag ) init_assoc(field); // initialize assoc tasks 

		// Update the CHECK-images
		if( prefs.check_flag )
			for (i=0; i<MAXCHECK; i++)
				if ((check=prefs.check[i]))
					reinitcheck(field, check);

		// Initialize PSF contexts and workspace
		if( prefs.psf_flag )
		{
			psf_readcontext(thepsf, field);
			psf_init(thepsf);
			if (prefs.dpsf_flag)
			{
				psf_readcontext(thepsf, dfield);
				psf_init(thepsf); // ???
			}
		}

		// Copy field structures to static ones (for catalog info)
		if( dfield )
		{
			thefield1 = *field;
			thefield2 = *dfield;
		} else
			thefield1 = thefield2 = *field;

		if( wfield )
		{
			thewfield1 = *wfield;
			thewfield2 = dwfield? *dwfield: *wfield;
		}
		else if (dwfield)
			thewfield2 = *dwfield;

		reinitcat( field );

		//////////////////////////:: SCANIMAGE ENGINE ::///////////////////////
		// Start the extraction pipeline 
//		NFPRINTF(OUTPUT, "Scanning image");
		scanimage( field, dfield, pffield, prefs.nimaflag, wfield, dwfield );
		///////////////////////////////////////////////////////////////////////

		// Finish the current CHECK-image processing
		if (prefs.check_flag)
			for (i=0; i<MAXCHECK; i++)
				if ((check=prefs.check[i]))
					reendcheck(field, check);

		// Final time measurements
		if (time(&thetime2)!=-1)
		{
			if (!strftime(thecat.ext_date, 12, "%d/%m/%Y", localtime(&thetime2)))
				error(EXIT_FAILURE, "*Internal Error*: Date string too long ","");
			if (!strftime(thecat.ext_time, 10, "%H:%M:%S", localtime(&thetime2)))
				error(EXIT_FAILURE, "*Internal Error*: Time/date string too long ","");
				thecat.ext_elapsed = difftime(thetime2, thetime1);
		}

		reendcat();

		/////////////////////// :: probably here i learn how tp extract my data
		// Update XML data
		if( prefs.xml_flag || prefs.cat_type==ASCII_VO )
		{
			update_xml(&thecat, dfield? dfield:field, field,
			dwfield? dwfield:wfield, wfield);
		}


		// Close ASSOC routines 
		end_assoc(field);

		for (i=0; i<prefs.nimaflag; i++)
			endfield(pffield[i]);

		endfield(field);

		if (dfield) endfield(dfield);
		if (wfield) endfield(wfield);
		if (dwfield) endfield(dwfield);

		//////////////// here is from where to get the objects detected
		QPRINTF(OUTPUT, "Objects: detected %-8d / sextracted %-8d               \n",
				thecat.ndetect, thecat.ntotal);
	}


	if( nok<0 )
	{
		error( EXIT_FAILURE, "Not enough valid FITS image extensions in ", prefs.image_name[0] );
	}
*/
//	free_cat(&imacat, 1);


//	NFPRINTF(OUTPUT, "Closing files");

	// End CHECK-image processing
	if (prefs.check_flag)
	{
		for (i=0; i<MAXCHECK; i++)
		{
			if ((check=prefs.check[i])) EndCheck(check);
			prefs.check[i] = NULL;
		}
	}

	// closing filters, neural net etc ....
	if (prefs.filter_flag) EndFilter();
	if (prefs.somfit_flag) SomEnd(thesom);
	if (prefs.growth_flag) EndGrowth();
	if (prefs.psf_flag) PsfEnd(thepsf,thepsfit); //???
	if (prefs.dpsf_flag) PsfEnd(ppsf,ppsfit);
	if (FLAG(obj2.sprob)) NeurClose();

	// Processing end date and time */
	thetimet2 = time(NULL);
	tm = localtime(&thetimet2);
	sprintf(prefs.sdate_end,"%04d-%02d-%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
	sprintf(prefs.stime_end,"%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	prefs.time_diff = difftime(thetimet2, thetimet);

	// log detection time
	strLog.Printf( wxT("INFO :: Detection done in %.0f seconds"), prefs.time_diff );
	m_pUnimapWorker->Log( strLog );
	strLog.Printf( wxT("INFO :: Found %d objects"), pImg->m_nStar );
	m_pUnimapWorker->Log( strLog );

	//////////////// or check here in write xml
	// Write XML 
//	if (prefs.xml_flag) write_xml(prefs.xml_name);

	EndCat((char *)NULL);

//	if( prefs.xml_flag || prefs.cat_type==ASCII_VO ) end_xml();

	m_bIsDataInit = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ReadAstroImageParam
// Class:		CSextractor
// Purpose:		create a bew field for picstruct
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::ReadAstroImageParam( CAstroImage* pImg, 
											picstruct	*field )
{
	int		j,l, n=0;
	char	wstr1[TNX_MAXCHARS], wstr2[TNX_MAXCHARS];
	char	st[80], str[80],
			*buf=NULL, *point;

	// Larry :: Basic keywords :: BP_BYTE, BP_SHORT, BP_LONG, BP_FLOAT, BP_DOUBLE
	field->bitpix = BP_BYTE;

	field->bytepix = (field->bitpix>0?field->bitpix:-field->bitpix)>>3;
	field->width = pImg->m_nWidth;
	field->height = pImg->m_nHeight;
	field->npix = (KINGSIZE_T)field->width*field->height;
	field->bscale = 1.0;

	field->bzero = 0.0;
	field->bitsgn = 1;
	if (field->bitsgn && prefs.fitsunsigned_flag) field->bitsgn = 0;
	
	// larry: perhaps i don't need this
	strcpy( field->ident, "Unnamed");

	// Compression 
	field->compress_type = ICOMPRESS_NONE;
	
	// now set pointer to image buffer
//	field->opencvbuff = pImg->m_pIplImage->imageData;
	field->vectImageBuff = pImg->m_vectImageBuff;


	// Astrometry 
	// Presently, astrometry is done only on the measurement and detect images 
	if (field->flags&(MEASURE_FIELD|DETECT_FIELD))
	{
		astromstruct	*as;
		double		drota, s;
		int		naxis;

		QCALLOC(as, astromstruct, 1);
		field->astrom = as;

		naxis = as->naxis = 2;

		// larry : I changed FITSTOS to LFITSTOS - not not try to load 
		// fits header from file just copy  the default
		for (l=0; l<naxis; l++)
		{
			sprintf(str, "CTYPE%-3d", l+1);
			LFITSTOS(str, str, "");
			strncpy(as->ctype[l], str, 8);
			sprintf(str, "CUNIT%-3d", l+1);
			LFITSTOS(str, str, "deg");
			strncpy(as->cunit[l], str, 32);
			sprintf(str, "CRVAL%-3d", l+1);
			as->crval[l] = LFITSTOF(str, 0.0);
			sprintf(str, "CRPIX%-3d", l+1);
			as->crpix[l] = LFITSTOF(str, 1.0);
			sprintf(str, "CDELT%-3d", l+1);
			as->cdelt[l] = LFITSTOF(str, 1.0);

			if (fabs(as->cdelt[l]) < 1/BIG)
				error(EXIT_FAILURE, "*Error*: CDELT parameters out of range in ", field->filename);
		}

// out fits part
/*		if (fitsnfind(buf, "CD1_1", n))
		{
			// If CD keywords exist, use them for the linear mapping terms... 
			for (l=0; l<naxis; l++)
			{
				for (j=0; j<naxis; j++)
				{
					sprintf(str, "CD%d_%d", l+1, j+1);
					as->pc[l*naxis+j] = LFITSTOF(str, l==j?1.0:0.0)/as->cdelt[l];
				}
			}

		} else if (fitsnfind(buf, "PC001001", n))
		{
			// ...If PC keywords exist, use them for the linear mapping terms... 
			for (l=0; l<naxis; l++)
			{
				for (j=0; j<naxis; j++)
				{
					sprintf(str, "PC%03d%03d", l+1, j+1);
					as->pc[l*naxis+j] = LFITSTOF(str, l==j?1.0:0.0);
				}
			}

		} else*/
		{
			// ...otherwise take the obsolete CROTA2 parameter 
			s = as->cdelt[1]/as->cdelt[0];
			drota = LFITSTOF("CROTA2  ", 0.0);
			as->pc[3] = as->pc[0] = cos(drota*DEG);
			as->pc[1] = -(as->pc[2] = sin(drota*DEG));
			as->pc[1] *= s;
			as->pc[2] /= s;
		}

		QMALLOC(as->wcs, struct wcsprm, 1);
		// Test if the WCS is recognized and a celestial pair is found 
		if( prefs.world_flag && !l_wcsset(as->naxis,(const char(*)[9])as->ctype, as->wcs) &&
			as->wcs->flag<999)
		{
			char	*pstr;
			double	date;
			int	biss, dpar[3];

			as->wcs_flag = 1;
			// Coordinate reference frame 
			// Search for an observation date expressed in Julian days 
			date = LFITSTOF("MJD-OBS ",  -1.0);
			// Precession date (defined from Ephemerides du Bureau des Longitudes) 
			// in Julian years from 2000.0 
			if (date>0.0)
			{
				as->equinox = 2000.0 - (MJD2000 - date)/365.25;
			} else
			{
				// Search for an observation date expressed in "civil" format 
				LFITSTOS("DATE-OBS", str, "");
				if (*str)
				{
					// Decode DATE-OBS format: DD/MM/YY or YYYY-MM-DD 
					for (l=0; l<3 && (pstr = strtok(l?NULL:str,"/- ")); l++)
						dpar[l] = atoi(pstr);

					if (l<3 || !dpar[0] || !dpar[1] || !dpar[2])
					{
						// If DATE-OBS value corrupted or incomplete, assume 2000-1-1 
						warning("Invalid DATE-OBS value in header: ", str);
						dpar[0] = 2000; dpar[1] = 1; dpar[2] = 1;

					} else if (strchr(str, '/') && dpar[0]<32 && dpar[2]<100)
					{
						j = dpar[0];
						dpar[0] = dpar[2]+1900;
						dpar[2] = j;
					}

					biss = (dpar[0]%4)?0:1;
					// Convert date to MJD 
					date = -678956 + (365*dpar[0]+dpar[0]/4) - biss
							+ ((dpar[1]>2?((int)((dpar[1]+1)*30.6)-63+biss)
							:((dpar[1]-1)*(63+biss))/2) + dpar[2]);

					as->equinox = 2000.0 - (MJD2000 - date)/365.25;
				} else
				{
					// Well if really no date is found 
					as->equinox = 2000.0;
				}
			}

			if (field->flags&MEASURE_FIELD) prefs.epoch = as->equinox;

			LFITSTOS("RADECSYS", str, as->equinox<1984.0?
						"FK4": (as->equinox<1999.9999? "FK5" : "ICRS"));

			if (!strcmp(str, "ICRS"))
			{
				as->radecsys = RDSYS_ICRS;
				as->equinox = LFITSTOF("EQUINOX ", 2000.0);

			} else if (!strcmp(str, "FK5"))
			{
				as->radecsys = RDSYS_FK5;
				as->equinox = LFITSTOF("EQUINOX ", FITSTOF("EPOCH  ", 2000.0));
				if (field->flags&MEASURE_FIELD)
					sprintf(prefs.coosys, "eq_FK5");

			} else if (!strcmp(str, "FK4"))
			{
				if (as->equinox == 2000.0)
					as->equinox = LFITSTOF("EQUINOX ", FITSTOF("EPOCH  ", 1950.0));
				as->radecsys = RDSYS_FK4;
				warning("FK4 precession formulae not yet implemented:\n",
						"            Astrometry may be slightly inaccurate");

			} else if (!strcmp(str, "FK4-NO-E"))
			{
				if (as->equinox == 2000.0)
					as->equinox = LFITSTOF("EQUINOX ", FITSTOF("EPOCH  ", 1950.0));
				as->radecsys = RDSYS_FK4_NO_E;
				warning("FK4 precession formulae not yet implemented:\n",
						"            Astrometry may be slightly inaccurate");

			} else if (!strcmp(str, "GAPPT"))
			{
				as->radecsys = RDSYS_GAPPT;
				warning("GAPPT reference frame not yet implemented:\n",
						"            Astrometry may be slightly inaccurate");
			} else
			{
				warning("Using ICRS instead of unknown astrometric reference frame: ", str);
				as->radecsys = RDSYS_ICRS;
				as->equinox = LFITSTOF("EQUINOX ", 2000.0);
			}

			// Projection parameters 
			if (!strcmp(as->wcs->pcode, "TNX"))
			{
				// IRAF's TNX projection: decode these #$!?@#!! WAT parameters 
				if (fitsfind(buf, "WAT?????") != RETURN_ERROR)
				{
					//  First we need to concatenate strings 
					pstr = wstr1;
					sprintf(str, "WAT1_001");

					for (j=2; fitsread(buf,str,pstr,H_STRINGS,T_STRING)==RETURN_OK; j++)
					{
						sprintf(str, "WAT1_%03d", j);
						pstr += strlen(pstr);
					}

					pstr = wstr2;
					sprintf(str, "WAT2_001");
					for (j=2; fitsread(buf,str,pstr,H_STRINGS,T_STRING)==RETURN_OK; j++)
					{
						sprintf(str, "WAT2_%03d", j);
						pstr += strlen(pstr);
					}

					// LONGPOLE defaulted to 180 deg if not found 
					if ((pstr = strstr(wstr1, "longpole"))
						|| (pstr = strstr(wstr2, "longpole")))
					{
						pstr = strpbrk(pstr, "1234567890-+.");
					}

					as->longpole = pstr? atof(pstr) : 999.0;
					as->latpole = 999.0;

					// RO defaulted to 180/PI if not found 
					if ((pstr = strstr(wstr1, "ro"))
						|| (pstr = strstr(wstr2, "ro")))
					{
						pstr = strpbrk(pstr, "1234567890-+.");
					}

					as->r0 = pstr? atof(pstr) : 0.0;

					// Read the remaining TNX parameters 
					if ((pstr = strstr(wstr1, "lngcor"))
						|| (pstr = strstr(wstr2, "lngcor")))
					{
						as->tnx_lngcor = read_tnxaxis(pstr);
					}

					if (!as->tnx_lngcor) error(EXIT_FAILURE, "*Error*: incorrect TNX parameters in ", field->filename);
					if ((pstr = strstr(wstr1, "latcor"))
						|| (pstr = strstr(wstr2, "latcor")))
					{
						as->tnx_latcor = read_tnxaxis(pstr);
					}

					if (!as->tnx_latcor) error(EXIT_FAILURE, "*Error*: incorrect TNX parameters in ", field->filename);
				}

			} else
			{
				as->longpole = LFITSTOF("LONGPOLE", 999.0);
				as->latpole = LFITSTOF("LATPOLE ", 999.0);

				if (fitsnfind(buf, "PROJP1  ", n))
				{
					for (l=0; l<10; l++)
					{
						sprintf(str, "PROJP%-3d", l);
						as->projp[l] = LFITSTOF(str, 0.0);
					}
				}
			}

		} else
		{
			// No need to keep memory allocated for a useless WCS structure 
			free(as->wcs);
			as->wcs_flag = 0;
		}
	}
	// end of astrometry loading
	//////////////////////////////////////////////////////////////////////


  field->fitshead = buf;
  field->fitsheadsize = n*FBSIZE;

  return;	
}

////////////////////////////////////////////////////////////////////
// Method:		AnalyseEndObject
// Class:		CSextractor
// Purpose:		Final processing of object data, just before saving 
//				it to the catalog.
// Input:		picstruct *field, picstruct *dfield, picstruct *wfield,
//				picstruct *dwfield, int n, objliststruct *objlist
// Output:		void
////////////////////////////////////////////////////////////////////
void CSextractor::AnalyseEndObject( picstruct *field, picstruct *dfield, 
								picstruct *wfield, picstruct *dwfield, 
								int n, objliststruct *objlist )
{
	checkstruct	*check;
	int		i,j, ix,iy,selecflag, newnumber,nsub;
	objstruct	*obj;

	obj2struct	*obj2 = &outobj2;

	obj = &objlist->obj[n];

	// Current FITS extension
	obj2->ext_number = thecat.currext;

	// Source position 
	// That's standard FITS 
	obj2->sposx = (float)(obj2->posx = obj->mx+1.0); 
	obj2->sposy = (float)(obj2->posy = obj->my+1.0);

	// calculate Integer coordinates 
	ix=(int)(obj->mx+0.49999);
	iy=(int)(obj->my+0.49999);

	// Association
	if( prefs.assoc_flag ) obj2->assoc_number = DoAssoc(field, obj2->sposx, obj2->sposy);

	if( prefs.assoc_flag && prefs.assocselec_type!=ASSOCSELEC_ALL )
	{
	  selecflag = (prefs.assocselec_type==ASSOCSELEC_MATCHED)?
					obj2->assoc_number:!obj2->assoc_number;
	} else
		selecflag = 1;

	if (selecflag)
	{
		// Paste back to the image the object's pixels if BLANKing is on 
		if (prefs.blank_flag)
		{
			PasteImage(field, obj->blank, obj->subw, obj->subh, obj->subx, obj->suby);
			if (obj->dblank)
			{
				PasteImage(dfield, obj->dblank, obj->subw, obj->subh,
							obj->subx, obj->suby);
			}
		}

		// Error ellipse parameters
		if (FLAG(obj2.poserr_a))
		{
			double	pmx2,pmy2,temp,theta;

			if( fabs(temp=obj->poserr_mx2-obj->poserr_my2) > 0.0 )
				theta = atan2(2.0 * obj->poserr_mxy,temp) / 2.0;
			else
				theta = PI/4.0;

			temp = sqrt(0.25*temp*temp+obj->poserr_mxy*obj->poserr_mxy);
			pmy2 = pmx2 = 0.5*(obj->poserr_mx2+obj->poserr_my2);
			pmx2+=temp;
			pmy2-=temp;

			obj2->poserr_a = (float)sqrt(pmx2);
			obj2->poserr_b = (float)sqrt(pmy2);
			obj2->poserr_theta = theta*180.0/PI;
		}

		if (FLAG(obj2.poserr_cxx))
		{
			double	xm2,ym2, xym, temp;

			xm2 = obj->poserr_mx2;
			ym2 = obj->poserr_my2;
			xym = obj->poserr_mxy;
			obj2->poserr_cxx = (float)(ym2/(temp=xm2*ym2-xym*xym));
			obj2->poserr_cyy = (float)(xm2/temp);
			obj2->poserr_cxy = (float)(-2*xym/temp);
		}

		// Aspect ratio 
		if( FLAG(obj2.elong) ) obj2->elong = obj->a/obj->b;
		if( FLAG(obj2.ellip) ) obj2->ellip = 1-obj->b/obj->a;
		if( FLAG(obj2.polar) ) 
			obj2->polar = (obj->a*obj->a - obj->b*obj->b) / (obj->a*obj->a + obj->b*obj->b);

		// Photometry 

		// Convert the father of photom. error estimates from variance to RMS
		obj2->flux_iso = obj->flux;
		obj2->fluxerr_iso = sqrt(obj->fluxerr);
		if (FLAG(obj.flux_prof))
		{
			obj2->flux_prof = obj->flux_prof;
			obj2->fluxerr_prof = sqrt(obj->fluxerr_prof);
		}

		if( FLAG(obj2.flux_isocor) ) ComputeIsocorFlux(field, obj);

		if( FLAG(obj2.flux_aper) )
			for (i=0; i<prefs.naper; i++) 
				ComputeAperFlux(field, wfield, obj, i);

		if (FLAG(obj2.flux_auto)) 
			ComputeAutoFlux(field, dfield, wfield, dwfield, obj);

		if (FLAG(obj2.flux_petro)) ComputePetroFlux(field, dfield, wfield, dwfield, obj);

		// Growth curve 
		if (prefs.growth_flag) MakeAverGrowth(field, wfield, obj);

		// Windowed barycenter 
		if( FLAG(obj2.winpos_x) ) 
			ComputeWinpos(field, wfield, obj);

		// What about the peak of the profile? 
		if( obj->peak+obj->bkg >= prefs.satur_level ) obj->flag |= OBJ_SATUR;

		// Check-image CHECK_APERTURES option 
		if ((check = prefs.check[CHECK_APERTURES]))
		{
			// draw circle - larry, removed as no need - 03/02/2014
			//if (FLAG(obj2.flux_aper))
			//	for (i=0; i<prefs.naper; i++)
			//		SexCircle( (float*) check->pix, check->width, check->height,
			//					obj->mx, obj->my, prefs.apert[i]/2.0, check->overlay );

			// draw ellipse 
			//if (FLAG(obj2.flux_auto))
			//	SexEllips( (float*)check->pix, check->width, check->height,
			//				obj->mx, obj->my, obj->a*obj2->kronfactor,
			//				obj->b*obj2->kronfactor, obj->theta,
			//				check->overlay, obj->flag&OBJ_CROWDED );

			// draw ellipse 
			//if (FLAG(obj2.flux_petro))
			//	SexEllips( (float*)check->pix, check->width, check->height,
			//				obj->mx, obj->my, obj->a*obj2->petrofactor,
			//				obj->b*obj2->petrofactor, obj->theta,
			//				check->overlay, obj->flag&OBJ_CROWDED );
		}

		///////////////////////////////////
		// Star/Galaxy classification
		if (FLAG(obj2.sprob))
		{
			double	fac2, input[10], output, fwhm;

			fwhm = prefs.seeing_fwhm;

			fac2 = fwhm/field->pixscale;
			fac2 *= fac2;
			input[j=0] = log10(obj->iso[0]? obj->iso[0]/fac2: 0.01);
			input[++j] = field->thresh>0.0?
							log10(obj->peak>0.0? obj->peak/field->thresh: 0.1) :-1.0;

			for (i=1; i<NISO; i++)
				input[++j] = log10(obj->iso[i]? obj->iso[i]/fac2: 0.01);

			input[++j] = log10(fwhm);
			NeurResp(input, &output);
			obj2->sprob = (float)output;
		}

		///////////////////////////////////////////////////////////
		// Put here your calls to "BLIND" custom functions. Ex:
		// ex : compute_myotherparams(obj); 
		///////////////////////////////////////////////////////////

		newnumber = ++thecat.ntotal;

		// update segmentation map 
		if ((check=prefs.check[CHECK_SEGMENTATION]))
		{
			ULONG	*pix;
			ULONG	newsnumber = newnumber,
			oldsnumber = obj->number;
			int	dx,dx0,dy,dpix;

			pix = (ULONG *)check->pix + check->width*obj->ymin + obj->xmin;
			dx0 = obj->xmax-obj->xmin+1;
			dpix = check->width-dx0;

			for (dy=obj->ymax-obj->ymin+1; dy--; pix += dpix)
				for (dx=dx0; dx--; pix++)
						if (*pix==oldsnumber) *pix = newsnumber;
		}
		obj->number = newnumber;

		// SOM fitting 
		if (prefs.somfit_flag)
		{
			float    *input;

			input = thesom->input;
			CopyImage(field,input,thesom->inputsize[0],thesom->inputsize[1],ix,iy);

			if (thesom->nextrainput)
			{
				input += thesom->ninput-thesom->nextrainput;
				*(input) = (obj->mx+1)/field->width;
				*(input+1) = (obj->my+1)/field->height;
			}

			SomPhot( thesom, obj->bkg, field->backsig,
						(float)prefs.gain, obj->mx-ix, obj->my-iy,
						FLAG(obj2.vector_somfit)?outobj2.vector_somfit:NULL, -1.0 );

			obj2->stderr_somfit = thesom->stderror;
			obj2->flux_somfit = thesom->amp;
			outobj2.fluxerr_somfit = thesom->sigamp;
		}

		if( FLAG(obj2.vignet) )
			CopyImage( field,outobj2.vignet,prefs.vignetsize[0],prefs.vignetsize[1], ix,iy );

		if( FLAG(obj2.vigshift) )
			CopyImageCenter( field, outobj2.vigshift, prefs.vigshiftsize[0],
										prefs.vigshiftsize[1], obj->mx, obj->my );

		// Express everything in magnitude units 
		ComputeMags(field, obj);

		//////////////////////////////////
		// PSF fitting 
		nsub = 1;
		if( prefs.psf_flag )
		{
			if( prefs.dpsf_flag )
				DoublePsfFit(ppsf, field, wfield, obj, thepsf, dfield, dwfield);
			else
				PsfFit(thepsf, field, wfield, obj);

			obj2->npsf = thepsfit->npsf;
			if (prefs.psfdisplay_type == PSFDISPLAY_SPLIT)
			{
				nsub = thepsfit->npsf;
				if (nsub<1) nsub = 1;

			} else
			{
				for (j=0; j<thepsfit->npsf; j++)
				{
					if (FLAG(obj2.x_psf) && j<prefs.psf_xsize)
						obj2->x_psf[j] = thepsfit->x[j];
					if (FLAG(obj2.y_psf) && j<prefs.psf_ysize)
						obj2->y_psf[j] = thepsfit->y[j];
					if (FLAG(obj2.flux_psf) && j<prefs.psf_fluxsize)
						obj2->flux_psf[j] = thepsfit->flux[j];
					if (FLAG(obj2.magerr_psf) && j<prefs.psf_magerrsize)
						obj2->magerr_psf[j] = obj2->fluxerr_psf[j]>0.0?
												1.086*obj2->fluxerr_psf[j]/thepsfit->flux[j] : 99.0;
					if (FLAG(obj2.fluxerr_psf) && j<prefs.psf_fluxerrsize)
						obj2->fluxerr_psf[j] = obj2->fluxerr_psf[j];     
					if (FLAG(obj2.mag_psf) && j<prefs.psf_magsize)
						obj2->mag_psf[j] = thepsfit->flux[j]>0.0?
												prefs.mag_zeropoint -2.5*log10(thepsfit->flux[j]) : 99.0;
				}
			}
		}

		// Astrometry 
		if( prefs.world_flag ) 
			ComputeAstrom(field, obj);
		// Edit min and max coordinates to follow the FITS conventions 
		obj->xmin += 1;
		obj->ymin += 1;
		obj->xmax += 1;
		obj->ymax += 1;

		// Go through each newly identified component 
		for (j=0; j<nsub; j++)
		{
			if (prefs.psf_flag && prefs.psfdisplay_type == PSFDISPLAY_SPLIT)
			{
				if (FLAG(obj2.x_psf)) obj2->x_psf[0] = thepsfit->x[j];
				if (FLAG(obj2.y_psf)) obj2->y_psf[0] = thepsfit->y[j];
				// ??????
				if (FLAG(obj2.flux_psf))
					obj2->flux_psf[0] = thepsfit->flux[j]>0.0? thepsfit->flux[j]:0.0; 

				if (FLAG(obj2.mag_psf))
					obj2->mag_psf[0] = thepsfit->flux[j]>0.0? prefs.mag_zeropoint -2.5*log10(thepsfit->flux[j]) : 99.0;
				// ??
				if (FLAG(obj2.magerr_psf))
					obj2->magerr_psf[0]= (thepsfit->flux[j]>0.0 && obj2->fluxerr_psf[j]>0.0) ? 
											1.086*obj2->fluxerr_psf[j]/thepsfit->flux[j] : 99.0;

				if (FLAG(obj2.fluxerr_psf)) obj2->fluxerr_psf[0]= obj2->fluxerr_psf[j];
				if( j ) obj->number = ++thecat.ntotal;
			}

//			FPRINTF(OUTPUT, "%8d %6.1f %6.1f %5.1f %5.1f %12g %c%c%c%c%c%c%c%c\n",
//						obj->number, obj->mx+1.0, obj->my+1.0,
//						obj->a, obj->b,
//						obj->flux,
//						obj->flag&OBJ_CROWDED?'C':'_',
//						obj->flag&OBJ_MERGED?'M':'_',
//						obj->flag&OBJ_SATUR?'S':'_',
//						obj->flag&OBJ_TRUNC?'T':'_',
//						obj->flag&OBJ_APERT_PB?'A':'_',
//						obj->flag&OBJ_ISO_PB?'I':'_',
//						obj->flag&OBJ_DOVERFLOW?'D':'_',
//						obj->flag&OBJ_OVERFLOW?'O':'_');

			// use this for merged object filter
//			if( !(obj->flag && OBJ_MERGED) )
			// use this to set elongation filter
//			if( obj2->elong < 2 )

				// gal fit ?
				unsigned char nObjType = 0;
				if( obj2->sprob < 0 )
					nObjType = SKY_OBJECT_TYPE_LIKE_A_RAY; // cosmic ray
				else if( obj2->sprob < 0.5 )
					nObjType = SKY_OBJECT_TYPE_LIKE_A_STAR; // star - the other way around ??
				else 
					nObjType = SKY_OBJECT_TYPE_LIKE_A_GALAXY; // galaxy

				///////////////////
				// here we add the detected object to CAstroImage object 
				// LARRY CHECK? :: m_pAstroImage->AddStar( obj->mx+0.49999, obj->my+0.49999,
				//double nXDiff = (double) ( obj->mx - (double) obj->peakx );
				//double nYDiff = (double) ( obj->my - (double) obj->peaky );

				// should probably use peakx/y instead  - actually save both
				m_pAstroImage->AddStar( obj->mx, obj->my, obj->peakx, obj->peaky,
										obj2->mag_auto, obj->a, obj->b, 
										obj->theta, obj2->kronfactor, (double) obj->fwhm,
										(double) obj2->sprob, nObjType );

			// larry commented this out
			// writecat(n, objlist);
		}
    }

	// Remove again from the image the object's pixels if BLANKing is on ... 
	// ... and free memory 
	if( prefs.blank_flag && obj->blank )
	{
		if( selecflag )
		{
			if( prefs.somfit_flag && (check=prefs.check[CHECK_MAPSOM]))
			{
				BlankCheck(check, obj->blank, obj->subw, obj->subh,
							obj->subx, obj->suby, (PIXTYPE)*(obj2->vector_somfit));
			}
		}

		BlankImage(field, obj->blank, obj->subw, obj->subh, obj->subx, obj->suby, (float) -BIG);
		free(obj->blank);

		if (obj->dblank)
		{
			BlankImage(dfield, obj->dblank, obj->subw, obj->subh,
						obj->subx, obj->suby, (float) -BIG);
			free(obj->dblank);
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		WriteError
// Class:		CSextractor
// Purpose:		Manage files in case of a catched error
// Input:		a character string, another character string
// Output:		RETURN_OK if everything went fine, RETURN_ERROR otherwise.
////////////////////////////////////////////////////////////////////
void CSextractor::WriteError( char *msg1, char *msg2 )
{
	char error[MAXCHAR];

	sprintf(error, "%s%s", msg1,msg2);
	if (prefs.xml_flag) WriteXmlError(prefs.xml_name, error);

	// Also close existing catalog */
	EndCat(error);

	EndXml();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Hmedian
// Class:		CSextractor
// Purpose:		Median using Heapsort algorithm (for float arrays) 
//				(based on Num.Rec algo.).Warning: changes the order of data!
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
float CSextractor::Hmedian( float *ra, int n )
{
	int		l, j, ir, i;
	float	rra;

	if( n<2 ) return *ra;

	ra--;
	for( l = ((ir=n)>>1)+1; ; )
	{
		if (l>1)
		{
			rra = ra[--l];

		} else
		{
			rra = ra[ir];
			ra[ir] = ra[1];
			if (--ir == 1)
			{
				ra[1] = rra;
				return n&1? ra[n/2+1] : (float)((ra[n/2]+ra[n/2+1])/2.0);
			}
		}

		for (j = (i=l)<<1; j <= ir;)
		{
			if( j < ir && ra[j] < ra[j+1] ) ++j;
			if( rra < ra[j] )
			{
				ra[i] = ra[j];
				j += (i=j);

			} else
				j = ir + 1;
		}

		ra[i] = rra;
	}

	// (the 'return' is inside the loop!!) 
}


