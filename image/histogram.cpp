//  ImageHistogram.cpp   

// local headers
#include "../util/func.h"

// main header
#include "histogram.h"

// class: ChannelHistogram
CChannelHistogram::CChannelHistogram( )
{
	_min = 0;
	_max = 0;
	for( uint i=0; i<HistogramSamples; i++ ) _map[i] = 0;
}

// destructor
CChannelHistogram::CChannelHistogram( const CChannelHistogram& iHistogram )
{
	_min = iHistogram._min;
	_max = iHistogram._max;
	::memcpy(_map, iHistogram._map, HistogramSamples*sizeof(int));
}

// operator = 
const CChannelHistogram& CChannelHistogram::operator= (const CChannelHistogram& iHistogram)
{
  _min = iHistogram._min;
  _max = iHistogram._max;
  ::memcpy( _map, iHistogram._map, HistogramSamples*sizeof(int) );
  return *this;
}

// Reset
void CChannelHistogram::Reset( )
{
  _min = _max = 0;
  for (uint i=0; i<HistogramSamples; i++) _map[i] = 0;
}
// UpdateMinMax
void CChannelHistogram::UpdateMinMax()
{
  _min = _max = _map[0];

  for (uint i=1; i<HistogramSamples; i++)
  {
    if      (_map[i] > _max) _max = _map[i];
    else if (_map[i] < _min) _min = _map[i];
  }
}

// ComputeDominance
float CChannelHistogram::ComputeDominance() const
{
  // compute max index
  uint maxIdx = 0;
  uint max = _map[0];
  for (uint i=1; i<HistogramSamples; i++)
  {
    if (_map[i] > max) 
    {
      max = _map[i];
      maxIdx = i;
    }
  }

  uint first = 0;
  while ((first < HistogramSamples) && (0 == _map[first])) first++;
  uint last = HistogramSamples-1;
  while ((last != 0) && (0 == _map[last])) last--;

  uint d = last - first;
  if (0 == d)
    return -1.0;

  const float s = 1.0f / d;
  return s*(maxIdx-first);

} 

//  get status functions
uint * CChannelHistogram::GetMap()             { return _map;}
const uint * CChannelHistogram::GetMap() const { return _map;}
uint CChannelHistogram::GetMin() const         { return _min;}
uint CChannelHistogram::GetMax() const         { return _max;}
void CChannelHistogram::SetMin(uint iMin)      { _min = iMin; }
void CChannelHistogram::SetMax(uint iMax)      { _max = iMax; }


// class:	ImageHistogram
////////////////////////////////////////////////////////////////////
CImageHistogram::CImageHistogram( uint iChannelCount )
{
	_nChannel = iChannelCount;
	_min = 0;
	_max = 0;
	for( uint i=0; i<_nChannel; i++ ) _Channel[i].Reset( );
	_min = 0;_max = 0;
}
//----------------------------------------------------------------------------
/*CImageHistogram::CImageHistogram(const ImageHistogram& iHistogram) :
  _nChannel(iHistogram._nChannel),
  _min(iHistogram._min),
  _max(iHistogram._max)
{
  for (uint i=0; i<_nChannel; i++) 
    _Channel[i] = iHistogram._Channel[i];
} 
*/
//----------------------------------------------------------------------------
void CImageHistogram::UpdateMinMax()
{ 
	for (uint i=0; i<_nChannel; i++) GetChannel(i).UpdateMinMax();

	_min = _Channel[0].GetMin();
	_max = _Channel[0].GetMax();

	for (uint i=1; i<_nChannel; i++)
	{
		if( _Channel[i].GetMin() < _min ) 
			_min = _Channel[i].GetMin();
		else if( _Channel[i].GetMax() > _max )
			_max = _Channel[i].GetMax();
	}
}
//----------------------------------------------------------------------------
CChannelHistogram& CImageHistogram::GetChannel(uint iChannel)
{
//  elxASSERT(iChannel<=5);
  return _Channel[iChannel];
}
//----------------------------------------------------------------------------
const CChannelHistogram& CImageHistogram::GetChannel(uint iChannel) const
{
//  elxASSERT(iChannel<=5);
  return _Channel[iChannel];
}
//----------------------------------------------------------------------------
void CImageHistogram::Reset(uint iChannelCount)
{
//  elxASSERT(iChannelCount<=5);
	_nChannel = iChannelCount;
	for( uint i=0; i<_nChannel; i++ ) _Channel[i].Reset();
}

// some quick get methods
uint CImageHistogram::GetChannelCount() const { return _nChannel;}
uint CImageHistogram::GetMin() const { return _min;}
uint CImageHistogram::GetMax() const { return _max;}
float CImageHistogram::ComputeDominance() const { return _Channel[0].ComputeDominance(); }

////////////////////////////////////////////////////////////////////
// Method:		ComputeHistogram
// Class:		CImageHistogram
// Purpose:		compute histogram for RGBub
// Input:		image reference
// Output:		status
////////////////////////////////////////////////////////////////////
bool CImageHistogram::ComputeHistogram( wxImage& rImage )
{
	if( !rImage.IsOk() ) return false;

	Reset( 4 );
	uint * prMap0 = GetChannel(0).GetMap();
	uint * prMap1 = GetChannel(1).GetMap();
	uint * prMap2 = GetChannel(2).GetMap();
	uint * prMap3 = GetChannel(3).GetMap();

	// get image details & ref
	unsigned char* srcImg = rImage.GetData();
	int w = rImage.GetWidth();
	int h = rImage.GetHeight();
	unsigned int size = w*h*3;

	uint r, g, b, idx, i;

	for( i=0; i<size; i+=3 )
	{ 
		r = (uint) srcImg[i];
		g = (uint) srcImg[i+1];
		b = (uint) srcImg[i+2];

		idx = (r*2989UL + g*5866UL + b*1145UL)/5000UL; 
		if( idx>=HistogramSamples ) idx = HistogramSamples-1;

		prMap0[idx]++;
		prMap1[r]++;
		prMap2[g]++;
		prMap3[b]++;
	}
 
	// map 256 into 512 values
	uint k = 255, j = 511;
	for( uint i=0; i<256; i++, j--, k-- )
	{
		r = prMap1[k]; g = prMap2[k]; b = prMap3[k];
		prMap1[j] = r; prMap2[j] = g; prMap3[j] = b;
		j--;
		prMap1[j] = r; prMap2[j] = g; prMap3[j] = b;
	}

	UpdateMinMax();

	return true;
} 

////////////////////////////////////////////////////////////////////
// Method:		ComputeHistogramAreaElps
// Class:		CImageHistogram
// Purpose:		compute histogram for RGBub
// Input:		image reference
// Output:		status
////////////////////////////////////////////////////////////////////
bool CImageHistogram::ComputeHistogramAreaElps( wxImage& rImage, double x, double y, double nA, double nB, double ang )
{
	if( !rImage.IsOk() ) return false;

	Reset( 4 );
	uint * prMap0 = GetChannel(0).GetMap();
	uint * prMap1 = GetChannel(1).GetMap();
	uint * prMap2 = GetChannel(2).GetMap();
	uint * prMap3 = GetChannel(3).GetMap();

	// get image details & ref
	unsigned char* srcImg = rImage.GetData();
	int w = rImage.GetWidth();
	int h = rImage.GetHeight();
	//unsigned int size = w*h*3;
	uint r, g, b, idx, i, u, v;

	// some presets
	double ang_rad, sin_a, cos_a, ap, ap2, bp, kDist, nDistSq, nDist;
	//double nSR = nA < nB ? nA : nB;
	int nw = nA > nB ? nA : nB;
	ang_rad = PI*(180.0-ang)/180.0;
	double nA2 = nA*nA;
	double nB2 = nB*nB;

	// for all points in the window
	for( v=y-nw; v<=y+nw; v++ )
	{
		for( u=x-nw; u<=x+nw; u++ )
		{
			// safety check
			if( v<0 || v>=h || u<0 || u>=w ) continue;

			nDistSq = (double) sqr((double)u-x) + (double) sqr((double)v-y);
			//nDist = sqrt( nDistSq );

			// now here we do the ellipse heavy stuff
			sin_a = sin( ang_rad );
			cos_a = cos( ang_rad );
			ap = (double) y*( (1.0-cos_a*cos_a)/sin_a ) - (double)(x-(double)u)*cos_a - (double) v*sin_a;
			ap2 = ap*ap;
			bp = sqrt( nDistSq - ap2 );
			// and now calc my fact
			kDist = ap2/nA2 + (bp*bp)/nB2;

			if( kDist > 1 ) continue;
			
			i = (v*w+u)*3;
			r = (uint) srcImg[i];
			g = (uint) srcImg[i+1];
			b = (uint) srcImg[i+2];

			idx = (r*2989UL + g*5866UL + b*1145UL)/5000UL; 
			if( idx>=HistogramSamples ) idx = HistogramSamples-1;

			prMap0[idx]++;
			prMap1[r]++;
			prMap2[g]++;
			prMap3[b]++;
		}
	}

	// map 256 into 512 values
	uint k = 255, j = 511;
	for( i=0; i<256; i++, j--, k-- )
	{
		r = prMap1[k]; g = prMap2[k]; b = prMap3[k];
		prMap1[j] = r; prMap2[j] = g; prMap3[j] = b;
		j--;
		prMap1[j] = r; prMap2[j] = g; prMap3[j] = b;
	}

	UpdateMinMax();

	return true;
} 

