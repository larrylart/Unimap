

-- PointToPoint.hpp

bool ImageVariant::AdjustBCG(
    double iBrightness, double iContrast, double iGamma,
    uint iChannelMask, ProgressNotifier& iNotifier)
{
  if (NULL == _spAbstractImpl.get()) return false;
  if (IsBayer()) iChannelMask = CM_All;

  return elxGetPointToPointHandler(*_spAbstractImpl).
    AdjustBCG(*_spAbstractImpl, iBrightness, iContrast, iGamma, iChannelMask, iNotifier);

} // AdjustBCG

------ BCG.hpp

template <typename Pixel>
bool ImagePointProcessingImpl<Pixel>::AdjustBCG(
    AbstractImage& ioImage,
    double iBrightness, double iContrast, double iGamma,
    uint iChannelMask,
    ProgressNotifier& iNotifier) const
{
  ImageImpl<Pixel>& image = elxDowncast<Pixel>(ioImage);
  return AdjustBCG(image, iBrightness,iContrast,iGamma, iChannelMask, iNotifier);

} // AdjustBCG

} // namespace Image
} // namespace eLynx


template <typename Pixel>
bool ImagePointProcessingImpl<Pixel>::AdjustBCG(
    ImageImpl< Pixel >& ioImage,
    double iBrightness, double iContrast, double iGamma,
    uint iChannelMask,
    ProgressNotifier& iNotifier)
{
  return elxAdjustBCG(ioImage, iBrightness,iContrast,iGamma,
      iChannelMask, iNotifier, PIXEL_LUT_TYPE);

} // AdjustBCG


//----------------------------------------------------------------------------
//  AdjustBCG # NoLutType
//----------------------------------------------------------------------------
template <class Pixel>
inline
bool elxAdjustBCG(
    ImageImpl< Pixel >& ioImage,
    double iBrightness, double iContrast, double iGamma,
    uint iChannelMask,
    ProgressNotifier& iNotifier,
    NonLutType)
{
  // optimize processing for neutral values
  const bool bBrightness = (elxBrightnessDefault != iBrightness);
  const bool bContrast   = (elxContrastDefault != iContrast);
  const bool bGamma      = (elxGammaDefault != iGamma);
  if ((!bBrightness) && (!bContrast) && (!bGamma)) return true;
  if (!ioImage.IsValid()) return false;

  bool bSuccess = true;
  if (bBrightness)
    bSuccess |= elxAdjustBrightness(ioImage, iBrightness, iChannelMask, iNotifier, PIXEL_LUT_TYPE);

  if (bContrast)
    bSuccess |= elxAdjustContrast(ioImage, iContrast, iChannelMask, iNotifier, PIXEL_LUT_TYPE);

  if (bGamma)
    bSuccess |= elxAdjustGamma(ioImage, iGamma, iChannelMask, iNotifier, PIXEL_LUT_TYPE);

  return bSuccess;

} // AdjustBCG

