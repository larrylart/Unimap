////////////////////////////////////////////////////////////////////
// Larry:	well, screw it! I'll just have my own implementation to
//			rw/process fits images and headers
// Created:	01/08/2010
////////////////////////////////////////////////////////////////////

// wx headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
// other wx
#include <wx/string.h>

// local includes
#include "../../util/func.h"
#include "../astroimage.h"

// main header
#include "image_fits.h"

// Image type identification
struct AstroType { const char * name; EImageContent content; };

#define MAINTYPE_MAX 7
AstroType mainAstroType[MAINTYPE_MAX] =
{
  { "LIGHT",     IC_Light },
  { "OBJECT",    IC_Light },
  { "BIAS",      IC_Bias },
  { "OFFSET",    IC_Bias },
  { "DARK",      IC_Dark },
  { "FLAT",      IC_FlatField },
  { "FLATFIELD", IC_FlatField }
};

#define SUBTYPE_MAX 10
AstroType subAstroType[SUBTYPE_MAX] = 
{
  { "GREEN",     IC_Green },
  { "RED",       IC_Red },
  { "BLUE",      IC_Blue }, 
  { "LUMINANCE", IC_Luminance }, 
  { "CLEAR",     IC_Luminance }, 
  { "HA",        IC_Halpha }, 
  { "HALPHA",    IC_Halpha },
  { "HBETA",     IC_Hbeta },
  { "OIII",      IC_OIII },
  { "SII",       IC_SII }
};

////////////////////////////////////////////////////////////////////
CImgFmtFits::CImgFmtFits( )
{
	m_bHasHeader = 0;

	m_strObjectName = wxT("");
	m_strObservationType = wxT("");

	m_nFocalLength = 0.0;
	m_nApertureDiameter = 0.0;
	m_nApertureArea = 0.0;
	m_nShutter = 0.0;
	m_strFilter = wxT("");
	m_strTelescope = wxT("");
	m_strTelConf = wxT("");
	m_strTelTcs = wxT("");
	m_nTelRa = 0.0;
	m_nTelDec = 0.0;
	m_strInstrument = wxT("");
	m_strCamera = wxT("");
	m_nIsoSpeed = 0.0;
	m_nObsDatetime = 0;
	m_nTempSet = 0.0;
	m_nTempCcd = 0.0;
	m_nTargetRa = 0.0;
	m_nTargetDec = 0.0;
	m_nObjectRa = 0.0;
	m_nObjectDec = 0.0;
	m_nObjectAlt = 0.0;
	m_nObjectAz = 0.0;
	m_nObjectHa = 0.0;
	m_nSiteLat = 0.0;
	m_nSiteLon = 0.0;
	m_strObserverName = wxT("");
	m_strObservatoryName = wxT("");
	m_strNotes = wxT("");
	m_strSoftwareCre = wxT("");
	m_strSoftwareMod = wxT("");
	m_nExpTime = 0.0;

	// reset flags
	m_bHasTelRaDec = 0;
	m_bHasTargetRaDec = 0;
	m_bHasObjectRaDec = 0;
	m_bHasFocal = 0;
}

////////////////////////////////////////////////////////////////////
CImgFmtFits::~CImgFmtFits( )
{
	m_vectHeader.clear();
}

////////////////////////////////////////////////////////////////////
int CImgFmtFits::GetHint( double& ra, double& dec, double& focal )
{
	int bFound = 1;

	// get position
	if( m_bHasTelRaDec && ( m_nTelRa != 0.0 && m_nTelDec != 0.0 ) )
	{
		ra = m_nTelRa;
		dec = m_nTelDec;

	} else if( m_bHasTargetRaDec && ( m_nTargetRa != 0.0 && m_nTargetDec != 0.0 ) )
	{
		ra = m_nTargetRa;
		dec = m_nTargetDec;

	} else if( m_bHasObjectRaDec && ( m_nObjectRa != 0.0 && m_nObjectDec != 0.0 ) )
	{
		ra = m_nObjectRa;
		dec = m_nObjectDec;

	} else
		bFound = 0;

	if( m_bHasFocal && m_nFocalLength != 0.0 ) 
	{
		focal = m_nFocalLength;
		bFound += 2;
	} 

	return( bFound );
}

////////////////////////////////////////////////////////////////////
bool CImgFmtFits::ReadHeader( const wxString& strFileName )
{
	m_vectHeader.clear();

	char string[1024];
	float nVarFloat=0.0;
	char iprFilename[500];
	strcpy( iprFilename, strFileName.ToAscii() );

	int status = 0;
	fitsfile * paFitsFile = NULL;
	fitsOpenFile( &paFitsFile, iprFilename, READONLY, &status );
	if( status ) return false;

	m_bHasHeader = 1;

////////////////////////////////////

    int fstatus, nkeys, keypos, hdutype, ii, jj;
//    char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */
	char keyname[255];
	char keyval[255];
	char keycomm[255];

    /* attempt to move to next HDU, until we get an EOF error */
    for (ii = 1; !(fits_movabs_hdu(paFitsFile, ii, &hdutype, &status) ); ii++) 
    {
        // get no. of keywords 
        if( fits_get_hdrpos(paFitsFile, &nkeys, &keypos, &status) ) fstatus = 0;

//       printf("Header listing for HDU #%d:\n", ii);
        for (jj = 1; jj <= nkeys; jj++) 
		{
//           if( fits_read_record( paFitsFile, jj, card, &status ) ) fstatus = 0;
//			// print the keyword card 
//            printf("%s\n", card); 

			if( !fits_read_keyn( paFitsFile, jj, keyname, keyval, keycomm, &status ) )
			{
				AddHeaderKey( keyname, keyval, keycomm );
			}

        }
		// terminate listing with END 
        printf("END\n\n");  
    }

//    if (status == END_OF_FILE)   /* status values are defined in fitsioc.h */
//        status = 0;              /* got the expected EOF error; reset = 0  */
//    else
//       printerror( status );     /* got an unexpected error                */
//
//    if ( fits_close_file(fptr, &status) )
//         printerror( status );

////////////////////////////////////////////////////////////////

	fitsCloseFile( paFitsFile, &status );

	return( 1 );
}

// update header keys from base astro image
////////////////////////////////////////////////////////////////////
void CImgFmtFits::SetHeader( CAstroImage* pAstroImage )
{
	wxString strValue=wxT("");

	// focal
	strValue.Printf( wxT("%.6lf"), pAstroImage->m_nHintFocalLength );
	UpdateHeaderKey( wxT("FOCALLEN"), strValue, wxT("Focal length of telescope in mm"), TDOUBLE );
	// tel, object ra
	RaDegToSexagesimal( pAstroImage->m_nOrigRa, strValue, wxT(' ') );
	UpdateHeaderKey( wxT("RA"), strValue, wxT("[hms J2000] Target right ascension"), TSTRING );
	UpdateHeaderKey( wxT("OBJCTRA"), strValue, wxT("[hms J2000] Object right ascension"), TSTRING );
	UpdateHeaderKey( wxT("TELRA"), strValue, wxT("[hms J2000] Telescope right ascension"), TSTRING );
	// tel, object dec
	DecDegToSexagesimal( pAstroImage->m_nOrigDec, strValue, wxT(' ') );
	UpdateHeaderKey( wxT("DEC"), strValue, wxT("[dms +N J2000] Target declination"), TSTRING );
	UpdateHeaderKey( wxT("OBJCTDEC"), strValue, wxT("[dms +N J2000] Object declination"), TSTRING );
	UpdateHeaderKey( wxT("TELDEC"), strValue, wxT("[dms +N J2000] Telescope declination"), TSTRING );
	// SITELAT 
	DecDegToSexagesimal( pAstroImage->m_nObsLatitude, strValue, wxT(' ') );
	UpdateHeaderKey( wxT("SITELAT"), strValue, wxT("Latitude of the imaging location"), TSTRING );
	// SITELONG
	DecDegToSexagesimal( pAstroImage->m_nObsLongitude, strValue, wxT(' ') );
	UpdateHeaderKey( wxT("SITELONG"), strValue, wxT("Longitude of the imaging location"), TSTRING );
	// TELESCOP
	UpdateHeaderKey( wxT("TELESCOP"), pAstroImage->m_strTelescopeName, wxT("telescope used to acquire this image"), TSTRING );
	// INSTRUME
	UpdateHeaderKey( wxT("INSTRUME"), pAstroImage->m_strCameraName, wxT("instrument or camera used"), TSTRING );
	// OBSERVER
	UpdateHeaderKey( wxT("OBSERVER"), pAstroImage->m_strObsName, wxT("name of the observer"), TSTRING );
	// OBJECT
	UpdateHeaderKey( wxT("OBJECT"), pAstroImage->m_strTargetName, wxT("name or designation of object being imaged"), TSTRING );

	// SWMODIFY
	UpdateHeaderKey( wxT("SWMODIFY"), wxT("UniMap"), wxT("software that modified the file"), TSTRING );
}

////////////////////////////////////////////////////////////////////
void CImgFmtFits::UpdateHeaderKey( wxString strName, wxString strValue, wxString strComment, int type )
{
	int key_id = GetHeaderKey( strName );
	if( key_id >=0 ) 
	{
		m_vectHeader[key_id].value = strValue;
		m_vectHeader[key_id].comment = strComment;

	} else
	{
		DefRecFitsKey rec;
		rec.name = strName;
		rec.value = strValue;
		rec.comment = strComment;
		rec.type = type;

		m_vectHeader.push_back( rec );
	}
}

////////////////////////////////////////////////////////////////////
int CImgFmtFits::GetHeaderKey( wxString strName )
{
	int i=0;
	for( i=0; i<m_vectHeader.size(); i++ )
	{
		if( m_vectHeader[i].name.CmpNoCase( strName ) == 0 ) return( i );
	}
	return( -1 );
}

////////////////////////////////////////////////////////////////////
bool CImgFmtFits::WriteHeader( fitsfile *fptr )
{
	if( !fptr ) return(0);

	int i=0, status=0;
	for( i=0; i<m_vectHeader.size(); i++ )
	{
		char svalue[255];
		char comment[255];
		int type =  m_vectHeader[i].type;
		strcpy( svalue,  m_vectHeader[i].value.ToAscii() );
		strcpy( comment,  m_vectHeader[i].comment.ToAscii() );

		if( type == TDOUBLE )
		{
			double fvalue = wxAtof( m_vectHeader[i].value );
			::fits_update_key(fptr, TDOUBLE, m_vectHeader[i].name.ToAscii(), &fvalue, comment, &status);

		} else
		{
			::fits_update_key(fptr, TSTRING, m_vectHeader[i].name.ToAscii(), svalue, comment, &status);
		}
	}
	// update hi-low keys
	double lo = 0.0;
	double hi = 1.0;
	::fits_update_key(fptr, TDOUBLE, "MIPS-LO", &lo, "Lower visualization cutoff ", &status);
	::fits_update_key(fptr, TDOUBLE, "MIPS-HI", &hi, "Upper visualization cutoff ", &status);

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CImgFmtFits::AddHeaderKey( char* name, char* value, char* comment )
{
	int elem_type = TSTRING;

	wxString strName = wxString(name,wxConvUTF8);
	wxString strComment = wxString(comment,wxConvUTF8);

	// clear out the spaces
	strName.Trim(0).Trim(1);
	if( strName.IsEmpty() ) return;
	// now process the value
	if( value[0] == '\'' ) value[0] = ' ';
	if( value[strlen(value)-1] == '\'' ) value[strlen(value)-1] = ' ';
	wxString strVal = wxString(value,wxConvUTF8);
	strVal.Trim(0).Trim(1);
	if( strVal.IsEmpty() ) return;
	// also trim the comment
	strComment.Trim(0).Trim(1);

	// set custom variables
	if( strName.CmpNoCase( wxT("OBJECT") ) == 0 ) 
		m_strObjectName = strVal;
	else if( strName.CmpNoCase( wxT("OBSTYPE") ) == 0 )
		m_strObservationType = strVal;
	else if( strName.CmpNoCase( wxT("FOCALLEN") ) == 0 )
	{
		m_nFocalLength = wxAtof(strVal);
		m_bHasFocal = 1;
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("APTDIA") ) == 0 )
	{
		m_nApertureDiameter = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("APTAREA") ) == 0 )
	{
		m_nApertureArea = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("EXPTIME") ) == 0 )
	{
		m_nShutter = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("FILTER") ) == 0 )
		m_strFilter = strVal;
	else if( strName.CmpNoCase( wxT("TELESCOP") ) == 0 )
		m_strTelescope = strVal;
	else if( strName.CmpNoCase( wxT("TELCONF") ) == 0 )
		m_strTelConf = strVal;
	else if( strName.CmpNoCase( wxT("TELCONF") ) == 0 )
		m_strTelConf = strVal;
	else if( strName.CmpNoCase( wxT("TELTCS") ) == 0 )
		m_strTelTcs = strVal;
	else if( strName.CmpNoCase( wxT("TELRA") ) == 0 )
	{
		if( ConvertRaToDeg( strVal, m_nTelRa ) ) m_bHasTelRaDec = 1;

	} else if( strName.CmpNoCase( wxT("TELDEC") ) == 0 )
	{
		if( ConvertDecToDeg( strVal, m_nTelDec ) ) m_bHasTelRaDec  = 1;

	} else if( strName.CmpNoCase( wxT("DATE-OBS") ) == 0 )
	{
		// to be decoded : '2006-12-12T23:31:31.3'
/*
		int year,month,day,hour,min,sec;
		wxSscanf( strVal, wxT("%d-%d-%dT%d:%d:%d"), &year, &month, &day, &hour, &min, &sec );
		tm t;
		t.tm_year = year-1900;  // years since 1900 
		t.tm_mon  = month-1;    // months since January - [0,11]
		t.tm_mday = day-1;      // day of the month - [1,31]
		t.tm_hour = hour;       // hours since midnight - [0,23]
		t.tm_min  = min;        // minutes after the hour - [0,59]
		t.tm_sec  = sec;        // seconds after the minute - [0,59]
	*/
		wxDateTime mydtime;
		mydtime.ParseFormat( strVal, wxT("%Y-%m-%dT%H:%M:%S") );
		// if not valid try only the date
		if( !mydtime.IsValid() ) mydtime.ParseFormat( strVal, wxT("%Y-%m-%d") );
		// if valid set my ticks
		if( mydtime.IsValid() ) m_nObsDatetime = mydtime.GetTicks(); //mktime(&t);

	} else if( strName.CmpNoCase( wxT("ISOSPEED") ) == 0 )
	{
		m_nIsoSpeed = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("EXPOSURE") ) == 0 )
	{
		m_nExpTime = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( m_nExpTime != 0.0 && strName.CmpNoCase( wxT("EXPTIME") ) == 0 )
	{
		m_nExpTime = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("SET-TEMP") ) == 0 )
	{
		m_nTempSet = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("CCD-TEMP") ) == 0 )
	{
		m_nTempCcd = wxAtof(strVal);
		elem_type = TDOUBLE;

	} else if( strName.CmpNoCase( wxT("RA") ) == 0 )
	{
		if( ConvertRaToDeg( strVal, m_nTargetRa ) ) m_bHasTargetRaDec = 1;

	} else if( strName.CmpNoCase( wxT("DEC") ) == 0 )
	{
		if( ConvertDecToDeg( strVal, m_nTargetDec ) ) m_bHasTargetRaDec = 1;

	} else if( strName.CmpNoCase( wxT("OBJCTRA") ) == 0 )
	{
		if( ConvertRaToDeg( strVal, m_nObjectRa ) ) m_bHasObjectRaDec = 1;

	} else if( strName.CmpNoCase( wxT("OBJCTDEC") ) == 0 )
	{
		if( ConvertDecToDeg( strVal, m_nObjectDec ) ) m_bHasObjectRaDec = 1;

	} else if( strName.CmpNoCase( wxT("OBJCTALT") ) == 0 )
		m_nObjectAlt = (double) wxAtof(strVal);
	else if( strName.CmpNoCase( wxT("OBJCTAZ") ) == 0 )
		m_nObjectAz = (double) wxAtof(strVal);
	else if( strName.CmpNoCase( wxT("OBJCTHA") ) == 0 )
		m_nObjectHa = (double) wxAtof(strVal);
	else if( strName.CmpNoCase( wxT("SITELAT") ) == 0 )
	{
		ConvertDecToDeg( strVal, m_nSiteLat );

	} else if( strName.CmpNoCase( wxT("SITELONG") ) == 0 )
	{
		ConvertDecToDeg( strVal, m_nSiteLon );

	} else if( strName.CmpNoCase( wxT("INSTRUME") ) == 0 )
		m_strInstrument = strVal;
	else if( strName.CmpNoCase( wxT("OBSERVER") ) == 0 )
		m_strObserverName = strVal;
	else if( strName.CmpNoCase( wxT("OBSERVAT") ) == 0 )
		m_strObservatoryName = strVal;
	else if( strName.CmpNoCase( wxT("NOTES") ) == 0 )
		m_strNotes = strVal;
	else if( strName.CmpNoCase( wxT("SWCREATE") ) == 0 )
		m_strSoftwareCre = strVal;
	else if( strName.CmpNoCase( wxT("SWMODIFY") ) == 0 )
		m_strSoftwareMod = strVal;

	////////////////////////////////
	// push in my vector vals
	DefRecFitsKey rec;
	rec.name = strName;
	rec.value = strVal;
	rec.comment = strComment;
	rec.type = elem_type;

	m_vectHeader.push_back( rec );
}

////////////////////////////////////////////////////////////////////
//  GetContent from a string
////////////////////////////////////////////////////////////////////
EImageContent CImgFmtFits::GetContent( const char * iprString )
{
  if (NULL == iprString) return IC_Unknown;

  char string[elxPATH_MAX];
  ::strcpy(string, iprString);
  elxToUpper(string);

  // search for main type
  EImageContent content = IC_Unknown;
  for (uint32 i=0; i<MAINTYPE_MAX; i++)
    if (NULL != ::strstr(string, mainAstroType[i].name))
    {
      content = mainAstroType[i].content;
      break;
    }
  
  if ((content == IC_Unknown) || (content == IC_Light))
  {
    // search for subtype
    for (uint32 i=0; i<SUBTYPE_MAX; i++)
      if (NULL != ::strstr(string, subAstroType[i].name))
      {
        content = subAstroType[i].content;
        break;
      }
  }
  return content;

}

////////////////////////////////////////////////////////////////////
//  IsSupported
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::IsSupported( const char * iprFilename, ImageFileInfo& oInfo, bool ibThumbnail ) 
{
  if( !elxIsFileExist(iprFilename) ) return false;

  int status = 0;
  char string[1024];

  fitsfile * paFitsFile = NULL;
  fitsOpenFile( &paFitsFile, iprFilename, READONLY, &status);
  if (status) return false;

  // get header info
  int bitpix = 0;
  int naxis = 0;
  long naxes[3] = { 0, 0, 0 };
  fitsGetImageParam(paFitsFile, 3, &bitpix, &naxis, naxes, &status);
  if (0 == status)
  {
    oInfo.SetDimension(naxes[0], naxes[1]);

    EResolution r = RT_Undefined;
    switch (bitpix)
    {
      case 8:   r = RT_UINT8; break;
      case 16:  r = RT_UINT16; break;
      case 32:  r = RT_INT32; break;
      case -32: r = RT_Float; break;
      case -64: r = RT_Double; break;
      default: break;
    }
    if (r != RT_Undefined)
    oInfo.SetResolution(r);

    uint32 compo = naxes[2];
    EPixelType pt = PT_Undefined;
    if (compo <= 1)      pt = PT_L;
    else if (compo == 3) pt = PT_RGB;
    if (pt != PT_Undefined)
      oInfo.SetPixelType(pt);
  }

  EImageContent contentFromInfo = IC_Unknown;
  paFitsFile->HDUposition=0;
  fitsReadKey(paFitsFile, TSTRING, "IMAGETYP", &string, NULL, &status);
  if (0 == status)
  {
    contentFromInfo = GetContent(string);

    // for light type, get sub type
    if (contentFromInfo == IC_Light)
    {
      paFitsFile->HDUposition=0;
      fitsReadKey(paFitsFile, TSTRING, "FILTER", &string, NULL, &status);
      if (0 == status)
        contentFromInfo = GetContent(string);
    }
  }

  // compare contents to get final content
  EImageContent contentFromName = GetContent(iprFilename);
  EImageContent content = IC_Unknown;
  if (contentFromName == contentFromInfo)
    content = contentFromName;
  else if (IC_Unknown == contentFromInfo)
    content = contentFromName;
  else if (IC_Unknown == contentFromName)
    content = contentFromInfo;
  else // trust in file name
    content = contentFromName;

  // update content only if detected
  if (content != IC_Unknown)
    oInfo.SetContent(content);


  fitsCloseFile(paFitsFile, &status);
  paFitsFile = NULL;
  return true;

}

////////////////////////////////////////////////////////////////////
bool CImgFmtFits::Load( ImageVariant& oImage, const char * iprFilename, 
						ImageFileInfo& oInfo, ProgressNotifier& iNotifier )
{ 
    if( NULL == iprFilename ) return( false );

    shared_ptr<AbstractImage> psAbstract = LoadFile( iprFilename, oInfo, iNotifier );
    oImage.Assign(psAbstract);

    return( NULL != psAbstract );
}

////////////////////////////////////////////////////////////////////
//  LoadFile
////////////////////////////////////////////////////////////////////
shared_ptr<AbstractImage> CImgFmtFits::LoadFile( const char * iprFilename,
													ImageFileInfo& oInfo,
													ProgressNotifier& iNotifier )
{
	// check & fill file infos
	if( !IsSupported(iprFilename, oInfo, false) ) return shared_ptr<AbstractImage>();

	shared_ptr<AbstractImage> spAbstractImage;
	int status = 0;

	fitsfile * paFitsFile = NULL;
	fitsOpenFile( &paFitsFile, iprFilename, READONLY, &status);
	// error
	if( status ) return shared_ptr<AbstractImage>();

	// get header info
	int bitpix = 0;
	int naxis = 0;
	long naxes[3] = { 0, 0, 0 };

	fitsGetImageParam(paFitsFile, 3, &bitpix, &naxis, naxes, &status);
	if( status )
	{
		// error, close file
		fitsCloseFile( paFitsFile, &status );
		paFitsFile = NULL;
		return( shared_ptr<AbstractImage>() );
	}

	uint32 sizeX = naxes[0];
	uint32 sizeY = naxes[1];
	uint32 compo = naxes[2];

	uint32 size = sizeX*sizeY;
	if( 0 != compo ) size *= compo;

	int zero = 0;
	long origin[3] = { 1L, 1L, 1L };
	paFitsFile->HDUposition=0;

	if( size <= 0 )
	{
		// invalid file
		fitsCloseFile(paFitsFile, &status);
		paFitsFile = NULL;
		return( shared_ptr<AbstractImage>() );
	}

	//////////////////////////
	// by number of plane
	if( compo <= 1 )
	{
		//////////////////////////////////////////////////////
		//                      1 plane => Grey
		//////////////////////////////////////////////////////
		if( bitpix == 8 )
		{
			// uint8
			shared_ptr<ImageLub> spImage( new ImageLub(sizeX, sizeY) );
			uint8 * prData = spImage->GetSamples();

			fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, prData, &zero, &status);
//			CHECK_READ();

			spAbstractImage = spImage;

		} else if( bitpix == 16 )
		{
			// uint16
			shared_ptr<ImageLus> spImage( new ImageLus(sizeX, sizeY) );
			uint16 * prData = spImage->GetSamples();

			status = 0;
			fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, prData, &zero, &status);
			//fitsReadPixels(paFitsFile, TINT, origin, size, &zero, prData, &zero, &status);
			//      CHECK_READ();

			int bzero = 0;
			fitsReadKey(paFitsFile, TINT, "BZERO", &bzero, NULL, &status);
			if ((0 == status) && (0 != bzero))
			{
				uint16 * prEnd = spImage->GetSamplesEnd();
				do {*prData += bzero; } while (++prData != prEnd);
			}

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 32 )
		{
			// int
			shared_ptr<ImageLi> spImage( new ImageLi(sizeX, sizeY) );
			int * prData = spImage->GetSamples();

			fitsReadPixels(paFitsFile, TINT, origin, size, &zero, prData, &zero, &status);
			//      CHECK_READ();

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -32 )
		{
			// float
			shared_ptr<ImageLf> spImage( new ImageLf(sizeX, sizeY) );
			float * prData = spImage->GetSamples();

			fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, prData, &zero, &status);
			//      CHECK_READ();

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -64 )
		{
			// double
			shared_ptr<ImageLd> spImage( new ImageLd(sizeX, sizeY) );
			double * prData = spImage->GetSamples();

			fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, prData, &zero, &status);
			//      CHECK_READ();

			// aggregation transfert
			spAbstractImage = spImage; 

		} else
		{
			// unknown format
		}

	///////////////////////////////////////////////////////////////
	//                 2 color planes => LA
	//////////////////////////////////////////////////////////////
	} else if( compo == 2 )
	{
		const uint32 pixels = naxes[0] * naxes[1];
		if( bitpix == 8 )
		{
			// uint8
			scoped_array<uint8> spBuffer( new uint8 [size] );
			fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageLAub> spImage( new ImageLAub(sizeX, sizeY) );
			const uint8 * prL = spBuffer.get();
			const uint8 * prA = prL + pixels;

			PixelLAub * prDst = spImage->GetPixel();
			PixelLAub * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_luminance = *prL++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 16 )
		{
			// uint16
			scoped_array<uint16> spBuffer( new uint16 [size] );
			fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageLAus> spImage( new ImageLAus(sizeX, sizeY) );
			const uint16 * prL = spBuffer.get();
			const uint16 * prA = prL + pixels;

			PixelLAus * prDst = spImage->GetPixel();
			PixelLAus * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_luminance = *prL++;
				prDst->_alpha = *prA++;

			} while (++prDst != prEnd);

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 32 )
		{
			// int
			scoped_array<int> spBuffer( new int [size] );
			fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageLAi> spImage( new ImageLAi(sizeX, sizeY) );
			const int * prL = spBuffer.get();
			const int * prA = prL + pixels;

			PixelLAi * prDst = spImage->GetPixel();
			PixelLAi * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_luminance = *prL++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -32 )
		{
			// float
			scoped_array<float> spBuffer( new float [size] );
			fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageLAf> spImage( new ImageLAf(sizeX, sizeY) );
			const float * prL = spBuffer.get();
			const float * prA = prL + pixels;

			PixelLAf * prDst = spImage->GetPixel();
			PixelLAf * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_luminance = *prL++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -64 )
		{
			// double
			scoped_array<double> spBuffer( new double [size] );
			fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageLAd> spImage( new ImageLAd(sizeX, sizeY) );
			const double * prL = spBuffer.get();
			const double * prA = prL + pixels;

			PixelLAd * prDst = spImage->GetPixel();
			PixelLAd * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_luminance = *prL++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 
		}

	//////////////////////////////////////////////////////////////
	//                 3 color planes => RGB
	//////////////////////////////////////////////////////////////
	} else if( compo == 3 )
	{
		const uint32 pixels = naxes[0] * naxes[1];
		if( bitpix == 8 )
		{
			// uint8
			scoped_array<uint8> spBuffer( new uint8 [size] );
			fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBub> spImage( new ImageRGBub(sizeX, sizeY) );
			const uint8 * prR = spBuffer.get();
			const uint8 * prG = prR + pixels;
			const uint8 * prB = prG + pixels;

			PixelRGBub * prDst = spImage->GetPixel();
			PixelRGBub * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;

			} while (++prDst != prEnd);

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 16 )
		{
			// uint16      
			scoped_array<uint16> spBuffer( new uint16 [size] );
			fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBus> spImage( new ImageRGBus(sizeX, sizeY) );
			const uint16 * prR = spBuffer.get();
			const uint16 * prG = prR + pixels;
			const uint16 * prB = prG + pixels;

			PixelRGBus * prDst = spImage->GetPixel();
			PixelRGBus * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 32 )
		{
			// int
			scoped_array<int> spBuffer( new int [size] );
			fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBi> spImage( new ImageRGBi(sizeX, sizeY) );
			const int * prR = spBuffer.get();
			const int * prG = prR + pixels;
			const int * prB = prG + pixels;

			PixelRGBi * prDst = spImage->GetPixel();
			PixelRGBi * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -32 )
		{
			// float
			scoped_array<float> spBuffer( new float [size] );
			fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBf> spImage( new ImageRGBf(sizeX, sizeY) );
			const float * prR = spBuffer.get();
			const float * prG = prR + pixels;
			const float * prB = prG + pixels;

			PixelRGBf * prDst = spImage->GetPixel();
			PixelRGBf * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -64 )
		{
			// double
			scoped_array<double> spBuffer( new double [size] );
			fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBd> spImage( new ImageRGBd(sizeX, sizeY) );
			const double * prR = spBuffer.get();
			const double * prG = prR + pixels;
			const double * prB = prG + pixels;

			PixelRGBd * prDst = spImage->GetPixel();
			PixelRGBd * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 
		}

    //////////////////////////////////////////////////////////////
    //                 4 color planes => RGBA
    //////////////////////////////////////////////////////////////
	} else if( compo == 4 )
	{
		const uint32 pixels = naxes[0] * naxes[1];
		if( bitpix == 8 )
		{
			// uint8
			scoped_array<uint8> spBuffer( new uint8 [size] );
			fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageRGBAub> spImage( new ImageRGBAub(sizeX, sizeY) );
			const uint8 * prR = spBuffer.get();
			const uint8 * prG = prR + pixels;
			const uint8 * prB = prG + pixels;
			const uint8 * prA = prB + pixels;

			PixelRGBAub * prDst = spImage->GetPixel();
			PixelRGBAub * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 16 )
		{
			// uint16      
			scoped_array<uint16> spBuffer( new uint16 [size] );
			fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageRGBAus> spImage( new ImageRGBAus(sizeX, sizeY) );
			const uint16 * prR = spBuffer.get();
			const uint16 * prG = prR + pixels;
			const uint16 * prB = prG + pixels;
			const uint16 * prA = prB + pixels;

			PixelRGBAus * prDst = spImage->GetPixel();
			PixelRGBAus * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == 32 )
		{
			// int
			scoped_array<int> spBuffer( new int [size] );
			fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageRGBAi> spImage( new ImageRGBAi(sizeX, sizeY) );
			const int * prR = spBuffer.get();
			const int * prG = prR + pixels;
			const int * prB = prG + pixels;
			const int * prA = prB + pixels;

			PixelRGBAi * prDst = spImage->GetPixel();
			PixelRGBAi * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else if( bitpix == -32 )
		{
			// float
			scoped_array<float> spBuffer( new float [size] );
			fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
			//     CHECK_READ2();

			shared_ptr<ImageRGBAf> spImage( new ImageRGBAf(sizeX, sizeY) );
			const float * prR = spBuffer.get();
			const float * prG = prR + pixels;
			const float * prB = prG + pixels;
			const float * prA = prB + pixels;

			PixelRGBAf * prDst = spImage->GetPixel();
			PixelRGBAf * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage;

		} else if( bitpix == -64 )
		{
			// double
			scoped_array<double> spBuffer( new double [size] );
			fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
			//      CHECK_READ2();

			shared_ptr<ImageRGBAd> spImage( new ImageRGBAd(sizeX, sizeY) );
			const double * prR = spBuffer.get();
			const double * prG = prR + pixels;
			const double * prB = prG + pixels;
			const double * prA = prB + pixels;

			PixelRGBAd * prDst = spImage->GetPixel();
			PixelRGBAd * prEnd = spImage->GetPixelEnd();
			do 
			{ 
				prDst->_red   = *prR++;
				prDst->_green = *prG++;
				prDst->_blue  = *prB++;
				prDst->_alpha = *prA++;

			} while( ++prDst != prEnd );

			// aggregation transfert
			spAbstractImage = spImage; 

		} else
		{
			// unknown format
		}
	}

#if 0
	uint32 lo;
	fitsReadKey(paFitsFile, TUSHORT, "MIPSHI", &hi, NULL, &status);
	if( status ) lo = 0;

	uint32 hi;
	fitsReadKey(paFitsFile, TUSHORT, "MIPSLO", &lo, NULL, &status);
	if( status ) hi = 0;
	//unsigned short mini; 
	//unsigned short maxi; 
#endif

	fitsCloseFile(paFitsFile, &status);
	paFitsFile = NULL;

	return( spAbstractImage );
}



////////////////////////////////////////////////////////////////////
// Save fits image
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::Save( const ImageVariant& iImage, const char* iprFilename,
						ProgressNotifier& iNotifier, const ImageFileOptions* iprOptions )
{
	if( iprFilename == NULL ) return false;
	if( !iImage.IsValid() ) return false;

	wxString strFileName(iprFilename, wxConvUTF8);
	// check if file exists then delete first
	if(  wxFileExists(strFileName) ) wxRemoveFile( strFileName );

	const EPixelFormat PixelFormat = iImage.GetPixelFormat();
	switch (PixelFormat)
	{
		case PF_Lub:
		case PF_Lus:
		case PF_Li:
		case PF_Lf:
		case PF_Ld:
			return SaveL(iImage, iprFilename); break;

		case PF_LAub:
		case PF_LAus:
		case PF_LAi:
		case PF_LAf:
		case PF_LAd:
			return SaveLA(iImage, iprFilename); break;

		case PF_RGBub:
		case PF_RGBus:
		case PF_RGBi:
		case PF_RGBf:
		case PF_RGBd:
			return SaveRGB(iImage, iprFilename); break;

		case PF_RGBAub:
		case PF_RGBAus:
		case PF_RGBAi:
		case PF_RGBAf:
		case PF_RGBAd:
			return SaveRGBA(iImage, iprFilename); break;

		default:
			// image format NOT expected
			return false;
	}

	return( true );
}

////////////////////////////////////////////////////////////////////
//  SaveL
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::SaveL( const ImageVariant& iImage, const char* iprFilename )
{
	int status = 0;

	// create new fits file 
	fitsfile * paFitsFile = NULL;
	::fits_create_file( &paFitsFile, iprFilename, &status);

	// create an fits image
	const uint32 naxis = 2;
	long naxes[2];
	naxes[0] = iImage.GetWidth();
	naxes[1] = iImage.GetHeight();
	int bitpix = iImage.GetBitsPerPixel();

	const EResolution resolution = iImage.GetResolution();
	if ((RT_Float == resolution) || (RT_Double == resolution))
	bitpix = -bitpix;

	::fits_create_img(paFitsFile, bitpix, naxis, naxes, &status);

	// write fits image
	const uint32 size = naxes[0] * naxes[1];
	long origin[3] = { 1L, 1L, 1L };

	const void * prSrc = NULL;
	int type = TBYTE;

	switch( resolution )
	{
		case RT_UINT8:    
		{
			PixelIterator<PixelLub const> begin = elxConstDowncast<PixelLub>(iImage.Begin());
			prSrc = &begin->_channel[0];
			type = TBYTE;   
			break;
		}

		case RT_UINT16:
		{
			PixelIterator<PixelLus const> begin = elxConstDowncast<PixelLus>(iImage.Begin());
			prSrc = &begin->_channel[0];
			type = TSHORT;
			break;
		}

		case RT_INT32:
		{
			PixelIterator<PixelLi const> begin = elxConstDowncast<PixelLi>(iImage.Begin());
			prSrc = &begin->_channel[0];
			type = TINT;   
			break;
		}

		case RT_Float:
		{
			PixelIterator<PixelLf const> begin = elxConstDowncast<PixelLf>(iImage.Begin());
			prSrc = &begin->_channel[0];
			type = TFLOAT;   
			break;
		}

		case RT_Double:
		{
			PixelIterator<PixelLd const> begin = elxConstDowncast<PixelLd>(iImage.Begin());
			prSrc = &begin->_channel[0];
			type = TDOUBLE;   
			break;
		}

		default:
			type = TBYTE;   
			break;
	}

	::fits_write_pix(paFitsFile, type, origin, size, (void*)prSrc, &status);

	// update fits header keys
	WriteHeader( paFitsFile );

	// close the file
	::fits_close_file(paFitsFile, &status);
	paFitsFile = NULL;

	return( true );
} 

////////////////////////////////////////////////////////////////////
//  SaveLA
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::SaveLA( const ImageVariant& iImage, const char* iprFilename )
{
	int status = 0;

	// create new fits file
	fitsfile * paFitsFile = NULL;
	::fits_create_file( &paFitsFile, iprFilename, &status);

	// create an fits image
	const uint32 naxis = 3;
	long naxes[3];
	naxes[0] = iImage.GetWidth();
	naxes[1] = iImage.GetHeight();
	naxes[2] = 2;
	int bitpix = iImage.GetBitsPerPixel() / naxes[2];

	const EResolution resolution = iImage.GetResolution();
	if( (RT_Float == resolution) || (RT_Double == resolution) ) bitpix = -bitpix;

	::fits_create_img(paFitsFile, bitpix, naxis, naxes, &status);

	// write fits image
	long origin[3] = { 1L, 1L, 1L };
	const uint32 pixels = naxes[0] * naxes[1];
	const uint32 size =  pixels * naxes[2];

	int type = TBYTE;
  
	switch (resolution)
	{
		case RT_UINT8:
		{
			type = TBYTE;
			PixelIterator<PixelLAub const> begin = elxConstDowncast<PixelLAub>(iImage.Begin());
			const uint8 * prSrc = &begin->_channel[0];
			const uint8 * prEnd = prSrc + size;

			uint8 * plBuffer = new uint8 [size];
			uint8 * prL = plBuffer;
			uint8 * prA = prL + pixels;

			do
			{
				*prL++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_UINT16:
		{
			type = TSHORT;
			PixelIterator<PixelLAus const> begin = elxConstDowncast<PixelLAus>(iImage.Begin());
			const uint16 * prSrc = &begin->_channel[0];
			const uint16 * prEnd = prSrc + size;

			uint16 * plBuffer = new uint16 [size];
			uint16 * prL = plBuffer;
			uint16 * prA = prL + pixels;

			do
			{
				*prL++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_INT32:    
		{
			type = TINT;
			PixelIterator<PixelLAi const> begin = elxConstDowncast<PixelLAi>(iImage.Begin());
			const int * prSrc = &begin->_channel[0];
			const int * prEnd = prSrc + size;

			int * plBuffer = new int [size];
			int * prL = plBuffer;
			int * prA = prL + pixels;

			do
			{
				*prL++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );
      
			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Float:    
		{
			type = TFLOAT;
			PixelIterator<PixelLAf const> begin = elxConstDowncast<PixelLAf>(iImage.Begin());

			const float * prSrc = &begin->_channel[0];
			const float * prEnd = prSrc + size;

			float * plBuffer = new float [size];
			float * prL = plBuffer;
			float * prA = prL + pixels;

			do
			{
				*prL++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Double:
		{
			type = TDOUBLE;
			PixelIterator<PixelLAd const> begin = elxConstDowncast<PixelLAd>(iImage.Begin());
			const double * prSrc = &begin->_channel[0];
			const double * prEnd = prSrc + size;

			double * plBuffer = new double [size];
			double * prL = plBuffer;
			double * prA = prL + pixels;

			do
			{
				*prL++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		default:          
			type = TBYTE;   
			break;
	}

	// update fits header keys
	WriteHeader( paFitsFile );

	// close the file
	::fits_close_file(paFitsFile, &status);
	paFitsFile = NULL;

	return( true );
}

////////////////////////////////////////////////////////////////////
//  SaveRGB
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::SaveRGB( const ImageVariant& iImage, const char* iprFilename )
{
	int status = 0;

	// create new fits file
	fitsfile * paFitsFile = NULL;
	::fits_create_file( &paFitsFile, iprFilename, &status);

	// create an fits image
	const uint32 naxis = 3;
	long naxes[3];
	naxes[0] = iImage.GetWidth();
	naxes[1] = iImage.GetHeight();
	naxes[2] = 3;
	int bitpix = iImage.GetBitsPerPixel() / naxes[2];

	const EResolution resolution = iImage.GetResolution();
	if ((RT_Float == resolution) || (RT_Double == resolution))
	bitpix = -bitpix;

	::fits_create_img(paFitsFile, bitpix, naxis, naxes, &status);

	// write fits image
	long origin[3] = { 1L, 1L, 1L };
	const uint32 pixels = naxes[0] * naxes[1];
	const uint32 size =  pixels * naxes[2];

	int type = TBYTE;
  
	switch (resolution)
	{
		case RT_UINT8:
		{
			type = TBYTE;
			PixelIterator<PixelRGBub const> begin = elxConstDowncast<PixelRGBub>(iImage.Begin());
			const uint8 * prSrc = &begin->_channel[0];
			const uint8 * prEnd = prSrc + size;

			uint8 * plBuffer = new uint8 [size];
			uint8 * prR = plBuffer;
			uint8 * prG = prR + pixels;
			uint8 * prB = prG + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_UINT16:
		{
			type = TSHORT;
			PixelIterator<PixelRGBus const> begin = elxConstDowncast<PixelRGBus>(iImage.Begin());
			const uint16 * prSrc = &begin->_channel[0];
			const uint16 * prEnd = prSrc + size;

			uint16 * plBuffer = new uint16 [size];
			uint16 * prR = plBuffer;
			uint16 * prG = prR + pixels;
			uint16 * prB = prG + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_INT32:    
		{
			type = TINT;
			PixelIterator<PixelRGBi const> begin = elxConstDowncast<PixelRGBi>(iImage.Begin());
			const int * prSrc = &begin->_channel[0];
			const int * prEnd = prSrc + size;

			int * plBuffer = new int [size];
			int * prR = plBuffer;
			int * prG = prR + pixels;
			int * prB = prG + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;

			} while( prSrc != prEnd );
      
			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Float:    
		{
			type = TFLOAT;
			PixelIterator<PixelRGBf const> begin = elxConstDowncast<PixelRGBf>(iImage.Begin());

			const float * prSrc = &begin->_channel[0];
			const float * prEnd = prSrc + size;

			float * plBuffer = new float [size];
			float * prR = plBuffer;
			float * prG = prR + pixels;
			float * prB = prG + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Double:
		{
			type = TDOUBLE;
			PixelIterator<PixelRGBd const> begin = elxConstDowncast<PixelRGBd>(iImage.Begin());
			const double * prSrc = &begin->_channel[0];
			const double * prEnd = prSrc + size;

			double * plBuffer = new double [size];
			double * prR = plBuffer;
			double * prG = prR + pixels;
			double * prB = prG + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		default:          
			type = TBYTE;   
			break;
	}

	// update fits header keys
	WriteHeader( paFitsFile );

	// close the file
	::fits_close_file(paFitsFile, &status);
	paFitsFile = NULL;

	return( true );

}

////////////////////////////////////////////////////////////////////
//  SaveRGBA
////////////////////////////////////////////////////////////////////
bool CImgFmtFits::SaveRGBA( const ImageVariant& iImage, const char* iprFilename )
{
	int status = 0;

	// create new fits file
	fitsfile * paFitsFile = NULL;
	::fits_create_file( &paFitsFile, iprFilename, &status);

	// create an fits image
	const uint32 naxis = 3;
	long naxes[3];
	naxes[0] = iImage.GetWidth();
	naxes[1] = iImage.GetHeight();
	naxes[2] = 4;
	int bitpix = iImage.GetBitsPerPixel() / naxes[2];

	const EResolution resolution = iImage.GetResolution();
	if ((RT_Float == resolution) || (RT_Double == resolution))
	bitpix = -bitpix;

	::fits_create_img(paFitsFile, bitpix, naxis, naxes, &status);

	// write fits image
	long origin[3] = { 1L, 1L, 1L };
	const uint32 pixels = naxes[0] * naxes[1];
	const uint32 size =  pixels * naxes[2];

	int type = TBYTE;
  
	switch (resolution)
	{
		case RT_UINT8:
		{
			type = TBYTE;
			PixelIterator<PixelRGBAub const> begin = elxConstDowncast<PixelRGBAub>(iImage.Begin());
			const uint8 * prSrc = &begin->_channel[0];
			const uint8 * prEnd = prSrc + size;

			uint8 * plBuffer = new uint8 [size];
			uint8 * prR = plBuffer;
			uint8 * prG = prR + pixels;
			uint8 * prB = prG + pixels;
			uint8 * prA = prB + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_UINT16:
		{
			type = TSHORT;
			PixelIterator<PixelRGBAus const> begin = elxConstDowncast<PixelRGBAus>(iImage.Begin());
			const uint16 * prSrc = &begin->_channel[0];
			const uint16 * prEnd = prSrc + size;

			uint16 * plBuffer = new uint16 [size];
			uint16 * prR = plBuffer;
			uint16 * prG = prR + pixels;
			uint16 * prB = prG + pixels;
			uint16 * prA = prB + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_INT32:    
		{
			type = TINT;
			PixelIterator<PixelRGBAi const> begin = elxConstDowncast<PixelRGBAi>(iImage.Begin());
			const int * prSrc = &begin->_channel[0];
			const int * prEnd = prSrc + size;

			int * plBuffer = new int [size];
			int * prR = plBuffer;
			int * prG = prR + pixels;
			int * prB = prG + pixels;
			int * prA = prB + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );
      
			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Float:    
		{
			type = TFLOAT;
			PixelIterator<PixelRGBAf const> begin = elxConstDowncast<PixelRGBAf>(iImage.Begin());

			const float * prSrc = &begin->_channel[0];
			const float * prEnd = prSrc + size;

			float * plBuffer = new float [size];
			float * prR = plBuffer;
			float * prG = prR + pixels;
			float * prB = prG + pixels;
			float * prA = prB + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		case RT_Double:
		{
			type = TDOUBLE;
			PixelIterator<PixelRGBAd const> begin = elxConstDowncast<PixelRGBAd>(iImage.Begin());
			const double * prSrc = &begin->_channel[0];
			const double * prEnd = prSrc + size;

			double * plBuffer = new double [size];
			double * prR = plBuffer;
			double * prG = prR + pixels;
			double * prB = prG + pixels;
			double * prA = prB + pixels;

			do
			{
				*prR++ = *prSrc++;
				*prG++ = *prSrc++;
				*prB++ = *prSrc++;
				*prA++ = *prSrc++;

			} while( prSrc != prEnd );

			::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
			elxSAFE_DELETE_LIST(plBuffer);
			break;
		}

		default:          
			type = TBYTE;   
			break;
	}

	// update fits header keys
	WriteHeader( paFitsFile );

	// close the file
	::fits_close_file(paFitsFile, &status);
	paFitsFile = NULL;

	return( true );
}
