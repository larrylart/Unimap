////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////
//  ImageHistogram.h  

#ifndef _IMAGE_HISTOGRAM_HEADER
#define _IMAGE_HISTOGRAM_HEADER

#include "wx/wxprec.h"

//#include <elx/core/CoreTypes.h>
//#include "ImageLib.h"

//namespace eLynx {
//namespace Image {

//class AbstractImage;

// type defines
typedef unsigned int    uint;

enum { HistogramSamples = 512 };

// Tool class defining a channel (grey) histogram.
class CChannelHistogram
{
public:
  CChannelHistogram( );
  CChannelHistogram( const CChannelHistogram& iHistogram );
  const CChannelHistogram& operator= (const CChannelHistogram& iHistogram);

  uint* GetMap();
  const uint * GetMap() const;
  uint GetMin() const;
  uint GetMax() const;
  void SetMin(uint iMin);
  void SetMax(uint iMax);

  void Reset();
  void UpdateMinMax();
  float ComputeDominance() const;

private:
  uint _map[512];
  uint _min;
  uint _max;
};

// Tool class defining an image histogram with multiple channel histograms.
class CImageHistogram
{
public:
  CImageHistogram(uint iChannelCount=4);
//  CImageHistogram( const ImageHistogram& iHistogram );

  uint GetChannelCount() const;
  uint GetMin() const;
  uint GetMax() const;

  CChannelHistogram& GetChannel(uint iChannel);
  const CChannelHistogram& GetChannel(uint iChannel) const;

  void Reset(uint iChannelCount=5);
  void UpdateMinMax();
  float ComputeDominance() const;

  // hist calculations
  bool ComputeHistogram( wxImage& rImage );
  bool ComputeHistogramAreaElps( wxImage& rImage, double x, double y, double nA, double nB, double ang );

private:
  CChannelHistogram _Channel[4+1]; // 4 planes + Luminosity for RGB or RGBA
  uint _nChannel;
  uint _min;
  uint _max;
};

//} // namespace Image
//} // namespace eLynx

#endif 

