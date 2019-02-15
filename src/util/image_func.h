////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_FUNC_H
#define _IMAGE_FUNC_H

// wx
#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/textctrl.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// elynx headers
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// image file type
typedef struct
{
	wxString ext[20];
	int nExt;
	wxString desc;
	// support read/write
	bool is_read;
	int read_lib_id;
	bool is_write;
	int write_lib_id;

} DefImageFileType;

// define images libraries read/write ids
#define IMG_IO_LIB_OWN			0
#define IMG_IO_LIB_WX			1
#define IMG_IO_LIB_ELYNX		2

//define image output types
static int g_nImageIOTypes = 25;
static DefImageFileType g_vectImageIOTypes[] = 
{ 
	// BMP
	{ { wxT("bmp") }, 1, wxT("Windows or OS2 Bitmap"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },									// 0
	// PGM
	{ { wxT("pgm") }, 1, wxT("Portable Grey Map"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },										// 1
	// PNM
	{ { wxT("pnm") }, 1, wxT("Portable aNy Map"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },										// 2
	// PPM
	{ { wxT("ppm"), wxT("pbm") }, 2, wxT("Portable Pixel Map"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },							// 3
	// JPEG/J2K
	{ { wxT("jpg"), wxT("jpeg"), wxT("jpe"), 
		wxT("jfif"), wxT("jfi") }, 5, wxT("JPEG Joint Photographic Experts Group"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },		// 4
	// J2K
	{ { wxT("j2k"), wxT("jp2"), wxT("jpt"), wxT("j2c") }, 4, 
		wxT("Joint Photographic Experts Group 2000"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },									// 5
	// PNG
	{ { wxT("png") }, 1, wxT("Portable Network Graphics"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },								// 6
	// TIFF
	{ { wxT("tif"), wxT("tiff") }, 2, wxT("Tagged Image File Format"), 1, IMG_IO_LIB_ELYNX, 1, IMG_IO_LIB_ELYNX },					// 7
	// FITS
	{ { wxT("fit"), wxT("fits"), wxT("fts") }, 3, wxT("Flexible Image Transport System"), 1, IMG_IO_LIB_OWN, 1, IMG_IO_LIB_OWN },	// 8
	// RAW 
	{ { wxT("raw"), wxT("3fr"), wxT("bmq"), wxT("cine"), wxT("cs1"), wxT("dc2"), 
		wxT("erf"), wxT("fff"), wxT("hdr"), wxT("ia"), wxT("kc2"), 
		wxT("mef"), wxT("mdc"), wxT("mos"), wxT("pxn"), wxT("qtk"), 
		wxT("rdc"), wxT("sti") }, 18, wxT("Raw camera decoder"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },						// 9
	// PSD
	{ { wxT("psd") }, 1, wxT("Photoshop file format"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },									// 10
	// GIF
	{ { wxT("gif") }, 1, wxT("CompuServe Graphics Interchange Format"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },					// 11
	// TGA
	{ { wxT("tga") }, 1, wxT("TARGA format by Truevision"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_WX },								// 12
	// Pentax camera
	{ { wxT("pef"), wxT("ptx"), wxT("dng") }, 3, wxT("Pentax camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },					// 13
	// Minolta camera
	{ { wxT("mrw") }, 1, wxT("Minolta camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },											// 14
	// FujiFilm camera
	{ { wxT("raf") }, 1, wxT("FujiFilm camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },										// 15
	// Kodak camera
	{ { wxT("dcr"), wxT("kdc"), wxT("kpdc"), wxT("k25") }, 4, wxT("Kodak camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },		// 16
	// Panasonic camera
	{ { wxT("rw2") }, 1, wxT("Panasonic camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },										// 17
	// Sony camera
	{ { wxT("arw"), wxT("srf"), wxT("sr2"), wxT("pmp") }, 4, wxT("Sony camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },		// 18
	// Foveon,Sigma camera
	{ { wxT("x3f") }, 1, wxT("Foveon,Sigma camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },									// 19
	// Canon camera
	{ { wxT("crw"), wxT("cr2") }, 2, wxT("Canon camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },								// 20
	// Olympus camera
	{ { wxT("orf") }, 1, wxT("Olympus camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },											// 21
	// Nikon camera
	{ { wxT("nef"), wxT("nrw") }, 2, wxT("Nikon camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },								// 22
	// Casio camera
	{ { wxT("bay") }, 1, wxT("Casio camera"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },											// 23
	// Digital Negative
	{ { wxT("dng") }, 1, wxT("Digital Negative"), 1, IMG_IO_LIB_ELYNX, 0, IMG_IO_LIB_ELYNX },										// 24

};

using namespace eLynx;
using namespace eLynx::Image;

boost::shared_ptr<ImageRGBub> _ELX_CreateRGBub( const ImageVariant& iImage, bool ibViewTransformed);

/////////////////////////////////
// Image global functions
wxString GetSupportedImageReadTypes( );
wxString GetSupportedImageWriteTypes( );
wxString GetImageTypeDescByExt( wxString strExt );

#endif
