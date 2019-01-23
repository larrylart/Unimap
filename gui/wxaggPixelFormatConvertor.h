#ifndef WX_AGG_PIXEL_FORMAT_CONVERTOR_H
#define WX_AGG_PIXEL_FORMAT_CONVERTOR_H

#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"


    /// Given a particular combination of channel type, bits per pixel and 
    /// channel indices, return the AGG format that matches.
    /// The actual template specializations that follow give the actual types, 
    /// and using a combination of parameters that are not listed will give 
    /// a compile-time error.
    template <typename Channel, int bitsPerPixel, int r, int g, int b, int a> 
    struct wxWidgetsToAGGHelper {
        //empty
    };

    /// 24-bit RGB 
    template <> struct wxWidgetsToAGGHelper<unsigned char, 24, 0, 1, 2, -1> {
        typedef agg::pixfmt_rgb24 format;
    };

    /// 24-bit BGR
    template <> struct wxWidgetsToAGGHelper<unsigned char, 24, 2, 1, 0, -1> {
        typedef agg::pixfmt_bgr24 format;
    };

    /// 32-bit RGB, alpha unused but stored as ARGB. 
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 1, 2, 3, -1> {
        typedef agg::pixfmt_argb32 format;
    };

    /// 32-bit RGB, alpha unused but stored as RGBA.
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 0, 1, 2, -1> {
        typedef agg::pixfmt_rgba32 format;
    };

    /// 32-bit BGR, alpha unused but stored as ABGR. 
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 3, 2, 1, -1> {
        typedef agg::pixfmt_abgr32 format;
    };

    /// 32-bit BGR, alpha unused but stored as BGRA.
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 2, 1, 0, -1> {
        typedef agg::pixfmt_bgra32 format;
    };

    /// 32-bit RGBA
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 0, 1, 2, 3> {
        typedef agg::pixfmt_rgba32 format;
    };

    /// 32-bit BGRA
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 2, 1, 0, 3> {
        typedef agg::pixfmt_bgra32 format;
    };

    /// 32-bit ARGB
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 1, 2, 3, 0> {
        typedef agg::pixfmt_argb32 format;
    };

    /// 32-bit ABGR
    template <> struct wxWidgetsToAGGHelper<unsigned char, 32, 3, 2, 1, 0> {
        typedef agg::pixfmt_abgr32 format;
    };


    /// Convert between a wxWidgets pixel format class and an AGG pixel format class.
    /// Usage examples: 
    /// PixelFormatConvertor<wxNativePixelFormat>::AGGType or 
    /// PixelFormatConvertor<wxAlphaPixelFormat>::AGGType.
    template <typename wxWidgetsPixelFormat>
    class PixelFormatConvertor {
    public:
        typedef wxWidgetsPixelFormat wxWidgetsType;

        // Break out the wxWidgets parameters and feed to the helper class.
        typedef typename wxWidgetsToAGGHelper<typename wxWidgetsPixelFormat::ChannelType, 
                                              wxWidgetsPixelFormat::BitsPerPixel,
                                              wxWidgetsPixelFormat::RED,
                                              wxWidgetsPixelFormat::GREEN,
                                              wxWidgetsPixelFormat::BLUE,
                                              wxWidgetsPixelFormat::ALPHA>::format AGGType;
    };


#endif