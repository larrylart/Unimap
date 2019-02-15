//============================================================================
//  plugin.cpp                                           fits.ImageFile.Plugin
//============================================================================
//
//  http://heasarc.nasa.gov/lheasoft/fitsio/fitsio.html
//  http://fits.gsfc.nasa.gov/
//  http://software.cfht.hawaii.edu/cfitsio/
//
//  FITS support provided by the CFITSIO library
//  Version 3.240 - January 26, 2010
//  Copyright (Unpublished-all rights reserved under the copyright laws of the United States), U.S.
//  Government as represented by the Administrator of the NASA, National Aeronautics and Space Administration.
//
//----------------------------------------------------------------------------
//  Copyright (C) 2006 by eLynx project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Library General Public License for more details.
//----------------------------------------------------------------------------
#include <time.h>

// eLynx.Core
#include <elx/core/CoreCore.h>
#include <elx/core/CoreFile.h>
#include <elx/core/IPlugin.h>
#include <elx/core/IPluginPackage.h>

// eLynx.Image
#include <elx/image/ImageVariant.h>
#include <elx/image/IImageFilePlugin.h>

// boost
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

// cfitsio
#include "cfitsio/fitsio.h"


#define fitsOpenFile          ffopen
#define fitsCloseFile         ffclos
#define fitsGetImageParam     ffgipr
#define fitsReadPixels        ffgpxv
#define fitsReadKey           ffgky


// UUID of fitsPlugin {9288618E-173E-4342-810F-EE626B2905D1}
elxDEFINE_UUID( UUID_fitsPlugin,
  0x9288618e, 0x173e, 0x4342, 0x81, 0xf, 0xee, 0x62, 0x6b, 0x29, 0x5, 0xd1);

namespace eLynx {
namespace Image {

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//                            ImageFilePluginImpl
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class ExportedByPlugin ImageFilePluginImpl : public IImageFilePlugin
{
public:
  // IPlugin implementation

  // Does it a internal plugins
  virtual bool IsPublic() const { return true; }

  // Get plugin implementation ID
  virtual const UUID& GetID() const { return UUID_fitsPlugin; }

  // Get plugin family type
  virtual const UUID& GetFamilyType() const { return UUID_IImageFilePlugin; }

  // Get version of implementation
  virtual size_t GetVersion() const { return 1; }

  // Get string description of plugin
  virtual const char * GetDescription(size_t iIndex=0) const
  {
    switch(iIndex)
    {
      case 0: return "fits";                            // plugin name
      case 1: return "NASA, eLynx Team";                // plugin copyright
      case 2: return "Flexible Image Transport System"; // long description
      case 3: return "CFITSIO v3.24 of January 26, 2010";
    }
    return NULL;
  }

  // IImageFileFormat implementation
  virtual size_t GetInputExtCount() const;
  virtual const char * GetInputExt(size_t iIndex) const;
  virtual size_t GetOutputExtCount() const;
  virtual const char * GetOutputExt(size_t iIndex) const;
  virtual size_t GetOutputImageFormatCount() const;
  virtual EPixelFormat GetOutputPixelFormat(size_t iIndex) const ;

  virtual bool IsSupported(const char * iprFilename, ImageFileInfo& oInfo, bool ibThumbnail) const;
  
  virtual bool GeneratePreview(
    const char * iprFilenameIn, 
    const char * iprFilenameOut,
    ProgressNotifier& iNotifier) const
  {
    return false;
  }

  virtual bool Import(
    ImageVariant& oImage, 
    const char * iprFilename, 
    ImageFileInfo& oInfo,
    ProgressNotifier& iNotifier)
  { 
    if (NULL == iprFilename)
      return false;
    shared_ptr<AbstractImage> psAbstract = LoadFile(iprFilename, oInfo, iNotifier);
    oImage.Assign(psAbstract);
    return (NULL != psAbstract);
  }
  shared_ptr<AbstractImage> LoadFile(const char *,ImageFileInfo&, ProgressNotifier&);

  virtual bool Export(
    const ImageVariant& iImage, 
    const char * iprFilename, 
    ProgressNotifier& iNotifier,
    const ImageFileOptions * iprOptions);

  bool SaveL(const ImageVariant&, const char *);
  bool SaveLA(const ImageVariant&, const char *);
  bool SaveRGB(const ImageVariant&, const char *);
  bool SaveRGBA(const ImageVariant&, const char *);
};

static ImageFilePluginImpl s_ImageFilePluginImpl;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//                               PluginPackageImpl
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class ExportedByPlugin ImageFilePluginPackageImpl : public IPluginPackage
{
public:
  // IPluginPackage implementation

  // Get the plugin family type.
  virtual const UUID& GetFamilyType() const { return UUID_IImageFilePlugin; }

  // Get the package ID.
  virtual const UUID& GetID() const { return UUID_fitsPlugin; }

  // Returns the number of plugin available in the package.
  virtual size_t GetPluginCount() const { return 1; }

  // Returns a reference pointer on the ith plugin of this package.
  virtual const IPlugin * GetPlugin(size_t=0) const { return &s_ImageFilePluginImpl; }
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//                         IImageFileFormat implementation
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define CHECK_READ()                    \
  if (status)                           \
  {                                     \
    spImage.reset();                    \
    fitsCloseFile(paFitsFile, &status); \
    paFitsFile = NULL;                  \
    return shared_ptr<AbstractImage>(); \
  }                                     \

#define CHECK_READ2()                   \
  if (status)                           \
  {                                     \
    fitsCloseFile(paFitsFile, &status); \
    paFitsFile = NULL;                  \
    return shared_ptr<AbstractImage>();	\
  }                                     \

//----------------------------------------------------------------------------
size_t ImageFilePluginImpl::GetInputExtCount() const { return 3; }

//----------------------------------------------------------------------------
const char * ImageFilePluginImpl::GetInputExt(size_t iIndex) const
{
  static const char * ms_Ext[] = { "fit", "fits", "fts" };
  if (iIndex < GetInputExtCount())
    return ms_Ext[iIndex];
  return NULL;
}

//----------------------------------------------------------------------------
//  GetOutputExtCount
//----------------------------------------------------------------------------
size_t ImageFilePluginImpl::GetOutputExtCount() const 
{
  return 3; 
}

//----------------------------------------------------------------------------
//  GetOutputExt
//----------------------------------------------------------------------------
const char * ImageFilePluginImpl::GetOutputExt(size_t iIndex) const 
{  
  static const char * ms_Ext[] = { "fit", "fits", "fts" };
  if (iIndex < GetInputExtCount())
    return ms_Ext[iIndex];
  return NULL;
}

//----------------------------------------------------------------------------
//  GetOutputImageFormatCount
//----------------------------------------------------------------------------
size_t ImageFilePluginImpl::GetOutputImageFormatCount() const 
{ 
  return 20; 
}

//----------------------------------------------------------------------------
//  GetOutputPixelFormat
//----------------------------------------------------------------------------
EPixelFormat ImageFilePluginImpl::GetOutputPixelFormat(size_t iIndex) const 
{ 
  static EPixelFormat ms_PixelFormat[20] =
  {
    PF_Lub, PF_Lus, PF_Li, PF_Lf, PF_Ld, 
    PF_LAub, PF_LAus, PF_LAi, PF_LAf, PF_LAd, 
    PF_RGBub, PF_RGBus, PF_RGBi, PF_RGBf, PF_RGBd,
    PF_RGBAub, PF_RGBAus, PF_RGBAi, PF_RGBAf, PF_RGBAd,
  };
  if (iIndex < GetOutputImageFormatCount())
    return ms_PixelFormat[iIndex];
  return PF_Undefined;
}

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

//----------------------------------------------------------------------------
//  GetContent from a string
//----------------------------------------------------------------------------
EImageContent GetContent(const char * iprString)
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

} // GetContent


//----------------------------------------------------------------------------
//  IsSupported
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::IsSupported(
    const char * iprFilename, 
    ImageFileInfo& oInfo, 
    bool ibThumbnail) const 
{
  if (!elxIsFileExist(iprFilename)) return false;

  int status = 0;
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

  float focalLength;
  paFitsFile->HDUposition=0;
  fitsReadKey(paFitsFile, TFLOAT, "FOCALLEN", &focalLength, NULL, &status);
  if (0 == status)
    oInfo.SetFocalLength(focalLength);

  float shutter;
  paFitsFile->HDUposition=0;
  fitsReadKey(paFitsFile, TFLOAT, "EXPTIME", &shutter, NULL, &status);
  if (0 == status)
    oInfo.SetShutter(shutter);

  char string[1024];
  paFitsFile->HDUposition=0;
  fitsReadKey(paFitsFile, TSTRING, "TELESCOP", &string, NULL, &status);
  if (0 == status)
  {
    oInfo.SetModel(string);
    //oInfo.SetMaker();
  }

  paFitsFile->HDUposition=0;
  fitsReadKey(paFitsFile, TSTRING, "DATE-OBS", &string, NULL, &status);
  if (0 == status)
  {
    // to be decoded : '2006-12-12T23:31:31.3'
    int year,month,day,hour,min,sec;
    ::sscanf(string, "%d-%d-%dT%d:%d:%d",
                &year,&month,&day,&hour,&min,&sec);
    tm t;
    t.tm_year = year-1900;  // years since 1900 
    t.tm_mon  = month-1;    // months since January - [0,11]
    t.tm_mday = day-1;      // day of the month - [1,31]
    t.tm_hour = hour;       // hours since midnight - [0,23]
    t.tm_min  = min;        // minutes after the hour - [0,59]
    t.tm_sec  = sec;        // seconds after the minute - [0,59]

    time_t timeStamp = mktime(&t);
    oInfo.SetTimeStamp(timeStamp);
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

} // IsSupported


//----------------------------------------------------------------------------
//  LoadFile
//----------------------------------------------------------------------------
shared_ptr<AbstractImage> ImageFilePluginImpl::LoadFile(
    const char * iprFilename,
    ImageFileInfo& oInfo,
    ProgressNotifier& iNotifier)
{
  // check & fill file infos
  if (!IsSupported(iprFilename, oInfo, false))
    return shared_ptr<AbstractImage>();

  shared_ptr<AbstractImage> spAbstractImage;
  int status = 0;

  fitsfile * paFitsFile = NULL;
  fitsOpenFile( &paFitsFile, iprFilename, READONLY, &status);
  // error
  if (status)
    return shared_ptr<AbstractImage>();

  // get header info
  int bitpix = 0;
  int naxis = 0;
  long naxes[3] = { 0, 0, 0 };

  fitsGetImageParam(paFitsFile, 3, &bitpix, &naxis, naxes, &status);
  if (status)
  {
    // error, close file
    fitsCloseFile(paFitsFile, &status);
    paFitsFile = NULL;
    return shared_ptr<AbstractImage>();
  }

  uint32 sizeX = naxes[0];
  uint32 sizeY = naxes[1];
  uint32 compo = naxes[2];

  uint32 size = sizeX*sizeY;
  if (0 != compo)
    size *= compo;

  int zero = 0;
  long origin[3] = { 1L, 1L, 1L };
  paFitsFile->HDUposition=0;

  if (size <= 0)
  {
    // invalid file
    fitsCloseFile(paFitsFile, &status);
    paFitsFile = NULL;
    return shared_ptr<AbstractImage>();
  }

  if (compo <= 1)
  {
    //============================================================
    //                      1 plane => Grey
    //============================================================
    if (8 == bitpix)
    {
      // uint8
      shared_ptr<ImageLub> spImage( new ImageLub(sizeX, sizeY) );
      uint8 * prData = spImage->GetSamples();

      fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, prData, &zero, &status);
      CHECK_READ();

      spAbstractImage = spImage;
    }
    else if (16 == bitpix)
    {
      // uint16
      shared_ptr<ImageLus> spImage( new ImageLus(sizeX, sizeY) );
      uint16 * prData = spImage->GetSamples();

      status = 0;
      fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, prData, &zero, &status);
      CHECK_READ();

      int bzero = 0;
      fitsReadKey(paFitsFile, TINT, "BZERO", &bzero, NULL, &status);
      if ((0 == status) && (0 != bzero))
      {
        uint16 * prEnd = spImage->GetSamplesEnd();
        do {*prData += bzero; } while (++prData != prEnd);
      }

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (32 == bitpix)
    {
      // int
      shared_ptr<ImageLi> spImage( new ImageLi(sizeX, sizeY) );
      int * prData = spImage->GetSamples();

      fitsReadPixels(paFitsFile, TINT, origin, size, &zero, prData, &zero, &status);
      CHECK_READ();

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (-32 == bitpix)
    {
      // float
      shared_ptr<ImageLf> spImage( new ImageLf(sizeX, sizeY) );
      float * prData = spImage->GetSamples();

      fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, prData, &zero, &status);
      CHECK_READ();

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (-64 == bitpix)
    {
      // double
      shared_ptr<ImageLd> spImage( new ImageLd(sizeX, sizeY) );
      double * prData = spImage->GetSamples();

      fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, prData, &zero, &status);
      CHECK_READ();

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else
    {
      // unknown format
    }
  }
  else if (2 == compo)
  {
    //============================================================
    //                 2 color planes => LA
    //============================================================
    const uint32 pixels = naxes[0] * naxes[1];
    if (8 == bitpix)
    {
      // uint8
      scoped_array<uint8> spBuffer( new uint8 [size] );
      fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

      shared_ptr<ImageLAub> spImage( new ImageLAub(sizeX, sizeY) );
      const uint8 * prL = spBuffer.get();
      const uint8 * prA = prL + pixels;

      PixelLAub * prDst = spImage->GetPixel();
      PixelLAub * prEnd = spImage->GetPixelEnd();
      do 
      { 
        prDst->_luminance = *prL++;
        prDst->_alpha = *prA++;
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (16 == bitpix)
    {
      // uint16
      scoped_array<uint16> spBuffer( new uint16 [size] );
      fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

      shared_ptr<ImageLAus> spImage( new ImageLAus(sizeX, sizeY) );
      const uint16 * prL = spBuffer.get();
      const uint16 * prA = prL + pixels;

      PixelLAus * prDst = spImage->GetPixel();
      PixelLAus * prEnd = spImage->GetPixelEnd();
      do 
      { 
        prDst->_luminance = *prL++;
        prDst->_alpha = *prA++;
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (32 == bitpix)
    {
      // int
      scoped_array<int> spBuffer( new int [size] );
      fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

      shared_ptr<ImageLAi> spImage( new ImageLAi(sizeX, sizeY) );
      const int * prL = spBuffer.get();
      const int * prA = prL + pixels;

      PixelLAi * prDst = spImage->GetPixel();
      PixelLAi * prEnd = spImage->GetPixelEnd();
      do 
      { 
        prDst->_luminance = *prL++;
        prDst->_alpha = *prA++;
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (-32 == bitpix)
    {
      // float
      scoped_array<float> spBuffer( new float [size] );
      fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

      shared_ptr<ImageLAf> spImage( new ImageLAf(sizeX, sizeY) );
      const float * prL = spBuffer.get();
      const float * prA = prL + pixels;

      PixelLAf * prDst = spImage->GetPixel();
      PixelLAf * prEnd = spImage->GetPixelEnd();
      do 
      { 
        prDst->_luminance = *prL++;
        prDst->_alpha = *prA++;
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
     }
    else if (-64 == bitpix)
    {
      // double
      scoped_array<double> spBuffer( new double [size] );
      fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

      shared_ptr<ImageLAd> spImage( new ImageLAd(sizeX, sizeY) );
      const double * prL = spBuffer.get();
      const double * prA = prL + pixels;

      PixelLAd * prDst = spImage->GetPixel();
      PixelLAd * prEnd = spImage->GetPixelEnd();
      do 
      { 
        prDst->_luminance = *prL++;
        prDst->_alpha = *prA++;
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
  }
  else if (3 == compo)
  {
    //============================================================
    //                 3 color planes => RGB
    //============================================================
    const uint32 pixels = naxes[0] * naxes[1];
    if (8 == bitpix)
    {
      // uint8
      scoped_array<uint8> spBuffer( new uint8 [size] );
      fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (16 == bitpix)
    {
      // uint16      
      scoped_array<uint16> spBuffer( new uint16 [size] );
      fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (32 == bitpix)
    {
      // int
      scoped_array<int> spBuffer( new int [size] );
      fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (-32 == bitpix)
    {
      // float
      scoped_array<float> spBuffer( new float [size] );
      fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
     }
    else if (-64 == bitpix)
    {
      // double
      scoped_array<double> spBuffer( new double [size] );
      fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
  }
  else if (4 == compo)
  {
    //============================================================
    //                 4 color planes => RGBA
    //============================================================
    const uint32 pixels = naxes[0] * naxes[1];
    if (8 == bitpix)
    {
      // uint8
      scoped_array<uint8> spBuffer( new uint8 [size] );
      fitsReadPixels(paFitsFile, TBYTE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (16 == bitpix)
    {
      // uint16      
      scoped_array<uint16> spBuffer( new uint16 [size] );
      fitsReadPixels(paFitsFile, TSHORT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (32 == bitpix)
    {
      // int
      scoped_array<int> spBuffer( new int [size] );
      fitsReadPixels(paFitsFile, TINT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else if (-32 == bitpix)
    {
      // float
      scoped_array<float> spBuffer( new float [size] );
      fitsReadPixels(paFitsFile, TFLOAT, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
     }
    else if (-64 == bitpix)
    {
      // double
      scoped_array<double> spBuffer( new double [size] );
      fitsReadPixels(paFitsFile, TDOUBLE, origin, size, &zero, spBuffer.get(), &zero, &status);
      CHECK_READ2();

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
      } 
      while (++prDst != prEnd);

      // aggregation transfert
      spAbstractImage = spImage; 
    }
    else
    {
      // unknown format
    }
  }

#if 0
  uint32 lo;
  fitsReadKey(paFitsFile, TUSHORT, "MIPSHI", &hi, NULL, &status);
  if (status)
  {
    lo = 0;
  }

  uint32 hi;
  fitsReadKey(paFitsFile, TUSHORT, "MIPSLO", &lo, NULL, &status);
  if (status)
    hi = 0;
//unsigned short mini; 
//unsigned short maxi; 

#endif

  fitsCloseFile(paFitsFile, &status);
  paFitsFile = NULL;

  return spAbstractImage;

} // LoadFile


//----------------------------------------------------------------------------
//  SaveL
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::SaveL(
    const ImageVariant& iImage,
    const char * iprFilename)
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
  switch (resolution)
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

  // update some keys
  double lo = 0.0;
  double hi = 1.0;
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-LO", &lo, "Lower visualization cutoff ", &status);
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-HI", &hi, "Upper visualization cutoff ", &status);

  // close the file
  ::fits_close_file(paFitsFile, &status);
  paFitsFile = NULL;

  return true;

} // SaveL

//----------------------------------------------------------------------------
//  SaveLA
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::SaveLA(
    const ImageVariant& iImage,
    const char * iprFilename)
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);
      
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

      ::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
      elxSAFE_DELETE_LIST(plBuffer);
      break;
    }

    default:          
      type = TBYTE;   
      break;
  }

  // update some keys
  double lo = 0.0;
  double hi = 1.0;
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-LO", &lo, "Lower visualization cutoff ", &status);
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-HI", &hi, "Upper visualization cutoff ", &status);

  // close the file
  ::fits_close_file(paFitsFile, &status);
  paFitsFile = NULL;

  return true;

} // SaveLA

//----------------------------------------------------------------------------
//  SaveRGB
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::SaveRGB(
    const ImageVariant& iImage,
    const char * iprFilename)
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);
      
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

      ::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
      elxSAFE_DELETE_LIST(plBuffer);
      break;
    }

    default:          
      type = TBYTE;   
      break;
  }

  // update some keys
  double lo = 0.0;
  double hi = 1.0;
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-LO", &lo, "Lower visualization cutoff ", &status);
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-HI", &hi, "Upper visualization cutoff ", &status);

  // close the file
  ::fits_close_file(paFitsFile, &status);
  paFitsFile = NULL;

  return true;

} // SaveRGB

//----------------------------------------------------------------------------
//  SaveRGBA
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::SaveRGBA(
    const ImageVariant& iImage,
    const char * iprFilename)
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);
      
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
      }
      while (prSrc != prEnd);

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
      }
      while (prSrc != prEnd);

      ::fits_write_pix(paFitsFile, type, origin, size, plBuffer, &status);
      elxSAFE_DELETE_LIST(plBuffer);
      break;
    }

    default:          
      type = TBYTE;   
      break;
  }

  // update some keys
  double lo = 0.0;
  double hi = 1.0;
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-LO", &lo, "Lower visualization cutoff ", &status);
  ::fits_update_key(paFitsFile, TDOUBLE, "MIPS-HI", &hi, "Upper visualization cutoff ", &status);

  // close the file
  ::fits_close_file(paFitsFile, &status);
  paFitsFile = NULL;

  return true;

} // SaveRGBA

//----------------------------------------------------------------------------
//  Export
//----------------------------------------------------------------------------
bool ImageFilePluginImpl::Export(
    const ImageVariant& iImage,
    const char * iprFilename,
    ProgressNotifier& iNotifier,
    const ImageFileOptions * iprOptions)
{
  if (NULL == iprFilename)
    return false;

  if (!iImage.IsValid())
    return false;

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

  return true;

} // Export

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//                              plugin startup
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
IMPLEMENT_PLUGIN_PACKAGE( ImageFilePluginPackageImpl );

} // namespace Image
} // namespace eLynx
