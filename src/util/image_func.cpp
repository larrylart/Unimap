////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

////////////////////
// eLynx
#include <elx/core/CoreMacros.h>
#include <elx/math/MathCore.h>
#include <elx/math/TransfertFunction.h>
//#include <elx/image/ImageVariant.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/image/IImageFilePlugin.h>
#include <elx/image/PixelIterator.h>

using namespace eLynx;
using namespace eLynx::Math;
using namespace eLynx::Image;

// local headers

// main header
#include "image_func.h"


boost::shared_ptr<ImageRGBub> _ELX_CreateRGBub( const ImageVariant& iImage,
											bool ibViewTransformed)
{
	// create an RGBub image from original image
	uint32 channelMask = CM_All;
	uint32 _channelMask = CM_All;
	bool _bBlendAlpha = true;
	EBayerToColorConversion _BCCMethod = BCC_Default;
	const AbstractTransfertFunction * prTransfertFunction = NULL;

//	if( ibViewTransformed ) prTransfertFunction = &_transfertFunction;

	switch( iImage.GetPixelMode() )
	{
		case PM_Color: channelMask = _channelMask;  break;
		case PM_Grey:
		case PM_Complex:
		default: break;
	}

	if( iImage.IsBayer() )
		return elxCreateImageRGBub( *iImage.GetImpl(), prTransfertFunction, _BCCMethod, iImage.GetBayerMatrix(), _channelMask );
	else
	{
		return elxCreateImageRGBub( *iImage.GetImpl(), prTransfertFunction, channelMask, _bBlendAlpha );
	}
}

////////////////////////////////////////////////////////////////////
wxString GetSupportedImageReadTypes( )
{
	int i=0, j=0;

	wxString strImgExtAll = wxT("");
	wxString strImgBlock = wxT("");
	wxString strTmp=wxT("");
	// build string of supported images
	for( i=0; i<g_nImageIOTypes; i++ )
	{
		// if no write skip
		if( !g_vectImageIOTypes[i].is_read ) continue;

		// build extensions
		wxString strImgExtPart=wxT("");
		for( j=0; j<g_vectImageIOTypes[i].nExt; j++ )
		{
			strImgExtPart += wxT("*.") + g_vectImageIOTypes[i].ext[j] + wxT(";");
		}
		strImgExtAll += strImgExtPart;
		strImgExtPart.RemoveLast();
		// build block 
		strTmp.Printf( wxT("|%s (%s)|%s"), g_vectImageIOTypes[i].desc, strImgExtPart, strImgExtPart );
		strImgBlock += strTmp;
	}
	// remove last char ";"
	strImgExtAll.RemoveLast();
	// build final formating
	strTmp.Printf( wxT("Image files (%s)|%s%s"), strImgExtAll, strImgExtAll, strImgBlock );

	return( strTmp );

	// from elynx - get supported type
//	const char * prIn = the_ImageFileManager.GetInputFileFormatList( true );
//	wxString strImageFormatListIn = wxString::FromAscii( prIn );
//	return( strImageFormatListIn );

/*
	"Image files (*.bmp;*.pgm;*.pnm;*.pbm;*.ppm;*.gif;*.jpg;*.jpeg;*.jpe;*.jfif;*.jfi;
	*.tga;*.png;*.psd;*.pef;*.ptx;*.dng;*.mrw;*.raf;*.dcr;*.kdc;*.kpdc;*.k25;*.rw2;*.arw;
	*.srf;*.sr2;*.pmp;*.x3f;*.crw;*.cr2;*.3fr;*.bmq;*.cine;*.cs1;*.dc2;*.erf;*.fff;*.hdr;
	*.ia;*.kc2;*.mef;*.mdc;*.mos;*.pxn;*.qtk;*.raw;*.rdc;*.sti;*.orf;*.nef;*.nrw;*.bay;
	*.dng;*.tiff;*.tif;*.j2k;*.j2c;*.jp2;*.jpt)
	|*.bmp;*.pgm;*.pnm;*.pbm;*.ppm;*.gif;*.jpg;*.jpeg;*.jpe;*.jfif;*.jfi;*.tga;*.png;*.psd;
	*.pef;*.ptx;*.dng;*.mrw;*.raf;*.dcr;*.kdc;*.kpdc;*.k25;*.rw2;*.arw;*.srf;*.sr2;*.pmp;
	*.x3f;*.crw;*.cr2;*.3fr;*.bmq;*.cine;*.cs1;*.dc2;*.erf;*.fff;*.hdr;*.ia;*.kc2;*.mef;
	*.mdc;*.mos;*.pxn;*.qtk;*.raw;*.rdc;*.sti;*.orf;*.nef;*.nrw;*.bay;*.dng;*.tiff;*.tif;
	*.j2k;*.j2c;*.jp2;*.jpt
	|Windows or OS2 Bitmap (*.bmp)|*.bmp
	|Portable Grey Map (*.pgm)|*.pgm
	|Portable aNy Map (*.pnm)|*.pnm
	|Portable Binary Map (*.pbm)|*.pbm
	|Portable Pixel Map (*.ppm)|*.ppm
	|CompuServe Graphics Interchange Format (*.gif)|*.gif
	|JPEG Joint Photographic Experts Group (*.jpg;*.jpeg;*.jpe;*.jfif;*.jfi)|*.jpg;*.jpeg;*.jpe;*.jfif;*.jfi
	|TARGA format by Truevision (*.tga)|*.tga
	|Portable Network Graphics (*.png)|*.png
	|Photoshop file format (*.psd)|*.psd

	|Pentax camera (*.pef;*.ptx;*.dng)|*.pef;*.ptx;*.dng
	|Minolta camera (*.mrw)|*.mrw
	|FujiFilm camera (*.raf)|*.raf
	|Kodak camera (*.dcr;*.kdc;*.kpdc;*.k25)|*.dcr;*.kdc;*.kpdc;*.k25
	|Panasonic camera (*.rw2)|*.rw2
	|Sony camera (*.arw;*.srf;*.sr2;*.pmp)|*.arw;*.srf;*.sr2;*.pmp
	|Foveon,Sigma camera (*.x3f)|*.x3f
	|Canon camera (*.crw;*.cr2)|*.crw;*.cr2

	|Raw file from camera (*.3fr;*.bmq;*.cine;*.cs1;*.dc2;*.erf;*.fff;*.hdr;*.ia;*.kc2;*.mef;*.mdc;*.mos;*.pxn;*.qtk;*.raw;*.rdc;*.sti)|*.3fr;*.bmq;*.cine;*.cs1;*.dc2;*.erf;*.fff;*.hdr;*.ia;*.kc2;*.mef;*.mdc;*.mos;*.pxn;*.qtk;*.raw;*.rdc;*.sti

	|Olympus camera (*.orf)|*.orf
	|Nikon camera (*.nef;*.nrw)|*.nef;*.nrw
	|Casio camera (*.bay)|*.bay
	|Digital Negative (*.dng)|*.dng

	|Tagged Image File Format (*.tiff;*.tif)|*.tiff;*.tif
	|Joint Photographic Experts Group 2000 (*.j2k;*.j2c;*.jp2;*.jpt)|*.j2k;*.j2c;*.jp2;*.jpt"

*/
}

////////////////////////////////////////////////////////////////////
wxString GetSupportedImageWriteTypes( )
{
	int i=0;

	wxString strImgExtAll = wxT("");
	wxString strImgBlock = wxT("");
	wxString strTmp=wxT("");
	// build string of supported images
	for( i=0; i<g_nImageIOTypes; i++ )
	{
		// if no write skip
		if( !g_vectImageIOTypes[i].is_write ) continue;

		// build extensions
		strImgExtAll += wxT("*.") + g_vectImageIOTypes[i].ext[0] + wxT(";");

		// build block 
		strTmp.Printf( wxT("|%s (*\.%s)|*\.%s"), g_vectImageIOTypes[i].desc, 
											g_vectImageIOTypes[i].ext[0], 
											g_vectImageIOTypes[i].ext[0] );
		strImgBlock += strTmp;
	}
	// remove last char ";"
	strImgExtAll.RemoveLast();
	// build final formating
	strTmp.Printf( wxT("Image files (%s)|%s%s"), strImgExtAll, strImgExtAll, strImgBlock );

	return( strTmp );
}

////////////////////////////////////////////////////////////////////
wxString GetImageTypeDescByExt( wxString strExt )
{
	int i=0, j=0;

	// go through all images/extensions
	for( i=0; i<g_nImageIOTypes; i++ )
	{
		for( j=0; j<g_vectImageIOTypes[i].nExt; j++ )
		{
			if( strExt.CmpNoCase( g_vectImageIOTypes[i].ext[j] ) == 0 )
				return( g_vectImageIOTypes[i].desc );
		}
	}

	return( wxT("Unknown Image Format") );
}
