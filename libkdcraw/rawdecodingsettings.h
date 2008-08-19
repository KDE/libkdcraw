/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : Raw decoding settings
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2008 by Guillaume Castagnino <casta at xwing dot info>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RAW_DECODING_SETTINGS_H
#define RAW_DECODING_SETTINGS_H

// Qt includes.

#include <qrect.h>
#include <qstring.h>

// Local includes.

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT RawDecodingSettings
{

public:

    /** RAW decoding Interpolation methods

        Bilinear: use high-speed but low-quality bilinear
                  interpolation (default - for slow computer). In this method,
                  the red value of a non-red pixel is computed as the average of
                  the adjacent red pixels, and similar for blue and green.
        VNG:      use Variable Number of Gradients interpolation.
                  This method computes gradients near the pixel of interest and uses
                  the lower gradients (representing smoother and more similar parts
                  of the image) to make an estimate.
        PPG:      use Patterned Pixel Grouping interpolation.
                  Pixel Grouping uses assumptions about natural scenery in making estimates.
                  It has fewer color artifacts on natural images than the Variable Number of
                  Gradients method.
        AHD:      use Adaptive Homogeneity-Directed interpolation.
                  This method selects the direction of interpolation so as to
                  maximize a homogeneity metric, thus typically minimizing color artifacts.
    */
    enum DecodingQuality
    {
        BILINEAR = 0,
        VNG      = 1,
        PPG      = 2,
        AHD      = 3
    };

    /** White balances alternatives
        NONE:     no white balance used : dcraw reverts to standard daylight D65 WB.
        CAMERA:   Use the camera embeded WB if available. Reverts to NONE if not.
        AUTO:     Averages an auto WB on the entire image.
        CUSTOM:   Let use set it's own temperature and green factor (later converted to RGBG factors).
        AERA:     Let use an aera from image to average white balance (see whiteBalanceArea for details).
    */
    enum WhiteBalance
    {
        NONE    = 0,
        CAMERA  = 1,
        AUTO    = 2,
        CUSTOM  = 3,
        AERA    = 4
    };

    /** Input color profile used to decoded image 
        NOINPUTCS:     No input color profile.
        EMBEDDED:      Use the camera profile embedded in RAW file if exist.
        CUSTOMINPUTCS: Use a custom input color space profile.
    */
    enum InputColorSpace 
    {
        NOINPUTCS = 0,
        EMBEDDED,
        CUSTOMINPUTCS
    };

    /** Output RGB color space used to decoded image 
        RAWCOLOR:       No output color profile (Linear RAW).
        SRGB:           Use standard sRGB color space.
        ADOBERGB:       Use standard Adobe RGB color space.
        WIDEGAMMUT:     Use standard RGB Wide Gamut color space.
        PROPHOTO:       Use standard RGB Pro Photo color space.
        CUSTOMOUTPUTCS: Use a custom workspace color profile.
    */
    enum OutputColorSpace
    {
        RAWCOLOR = 0,
        SRGB,
        ADOBERGB,
        WIDEGAMMUT,
        PROPHOTO,
        CUSTOMOUTPUTCS
    };

    /** Standard constructor with default settings */
    RawDecodingSettings()
    {
        sixteenBitsImage           = false;
        brightness                 = 1.0;
        RAWQuality                 = BILINEAR;
        inputColorSpace            = NOINPUTCS;
        outputColorSpace           = SRGB;
        RGBInterpolate4Colors      = false;
        DontStretchPixels          = false;
        unclipColors               = 0;
        whiteBalance               = CAMERA;
        customWhiteBalance         = 6500;
        customWhiteBalanceGreen    = 1.0;
        medianFilterPasses         = 0;

        halfSizeColorImage         = false;

        enableBlackPoint           = false;
        blackPoint                 = 0;

        enableWhitePoint           = false;
        whitePoint                 = 0;

        enableNoiseReduction       = false;
        NRThreshold                = 100;

        enableCACorrection         = false;
        caMultiplier[0]            = 1.0;
        caMultiplier[1]            = 1.0;

        inputProfile               = QString();
        outputProfile              = QString();

        deadPixelMap               = QString();

        whiteBalanceArea           = QRect();
    };

    /** Compare for equality */
    bool operator==(const RawDecodingSettings &o) const
    {
        return sixteenBitsImage        == o.sixteenBitsImage
            && brightness              == o.brightness
            && RAWQuality              == o.RAWQuality
            && inputColorSpace         == o.inputColorSpace
            && outputColorSpace        == o.outputColorSpace
            && RGBInterpolate4Colors   == o.RGBInterpolate4Colors
            && DontStretchPixels       == o.DontStretchPixels
            && unclipColors            == o.unclipColors
            && whiteBalance            == o.whiteBalance
            && customWhiteBalance      == o.customWhiteBalance
            && customWhiteBalanceGreen == o.customWhiteBalanceGreen
            && halfSizeColorImage      == o.halfSizeColorImage
            && enableBlackPoint        == o.enableBlackPoint
            && blackPoint              == o.blackPoint
            && enableWhitePoint        == o.enableWhitePoint
            && whitePoint              == o.whitePoint
            && enableNoiseReduction    == o.enableNoiseReduction
            && NRThreshold             == o.NRThreshold
            && enableCACorrection      == o.enableCACorrection
            && caMultiplier[0]         == o.caMultiplier[0]
            && caMultiplier[1]         == o.caMultiplier[1]
            && medianFilterPasses      == o.medianFilterPasses
            && inputProfile            == o.inputProfile
            && outputProfile           == o.outputProfile
            && deadPixelMap            == o.deadPixelMap
            && whiteBalanceArea        == o.whiteBalanceArea
          ;
    };

    /** Standard destructor */
    virtual ~RawDecodingSettings(){};

    /** Method to use a settings to optimize time loading, for exemple to compute image histogram */
    void optimizeTimeLoading(void)
    {
        sixteenBitsImage           = true;
        brightness                 = 1.0;
        RAWQuality                 = BILINEAR;
        inputColorSpace            = NOINPUTCS;
        outputColorSpace           = SRGB;
        RGBInterpolate4Colors      = false;
        DontStretchPixels          = false;
        unclipColors               = 0;
        whiteBalance               = CAMERA;
        customWhiteBalance         = 6500;
        customWhiteBalanceGreen    = 1.0;
        halfSizeColorImage         = true;
        medianFilterPasses         = 0;

        enableBlackPoint           = false;
        blackPoint                 = 0;

        enableWhitePoint           = false;
        whitePoint                 = 0;

        enableNoiseReduction       = false;
        NRThreshold                = 100;

        enableCACorrection         = false;
        caMultiplier[0]            = 1.0;
        caMultiplier[1]            = 1.0;

        inputProfile               = QString();
        outputProfile              = QString();

        deadPixelMap               = QString();

        whiteBalanceArea           = QRect();
    };

public:

    /** If true, decode RAW file in 16 bits per color per pixel else 8 bits.
    */
    bool sixteenBitsImage;

    /** Half-size color image decoding (twice as fast as "enableRAWQuality").
        Use this option to reduce time loading to render histogram for example,
        no to render an image to screen.
    */
    bool halfSizeColorImage;

    /** White balance type to use. See WhiteBalance values for detail
    */
    WhiteBalance whiteBalance;

    /** The temperature and the green multiplier of the custom white balance
    */
    int customWhiteBalance;
    double customWhiteBalanceGreen;

    /** RAW file decoding using RGB interpolation as four colors.
    */
    bool RGBInterpolate4Colors;

    /** For cameras with non-square pixels, do not stretch the image to its 
        correct aspect ratio. In any case, this option guarantees that each 
        output pixel corresponds to one RAW pixel.
    */
    bool DontStretchPixels;

    /** Unclip Highlight color level:
        0   = Clip all highlights to solid white.
        1   = Leave highlights unclipped in various shades of pink.
        2   = Blend clipped and unclipped values together for a gradual
              fade to white.
        3-9 = Reconstruct highlights. Low numbers favor whites; high numbers
              favor colors.
    */
    int unclipColors;

    /** RAW quality decoding factor value. See DecodingQuality values
        for details.
    */
    DecodingQuality RAWQuality;

    /** After interpolation, clean up color artifacts by repeatedly applying 
        a 3x3 median filter to the R-G and B-G channels.
    */
    int medianFilterPasses;

    /** Use wavelets to erase noise while preserving real detail.
    */
    bool enableNoiseReduction;

    /** Noise reduction threshold value.
        The best threshold should be somewhere between 100 and 1000.
    */
    int NRThreshold;

    /** Use red and blue layer magnification to reduce chromatic aberrations
    */
    bool enableCACorrection;

    /** Magnification factor for Red and Blue layers
        - caMultiplier[0] = red multiplier
        - caMultiplier[1] = blue multiplier
    */
    double caMultiplier[2];

    /** Brightness of output image.
    */
    float brightness;

    /** Set on the black point setting to decode RAW image.
    */
    bool enableBlackPoint;

    /** Black Point value of output image.
    */
    int blackPoint;

    /** Set on the white point setting to decode RAW image.
    */
    bool enableWhitePoint;

    /** White Point value of output image.
    */
    int whitePoint;

    /** The input color profile used to decoded RAW data. See OutputColorProfile
        values for details.
    */
    InputColorSpace inputColorSpace;

    /** Path to custom input ICC profile to define the camera's raw colorspace.
    */
    QString inputProfile;

    /** The output color profile used to decoded RAW data. See OutputColorProfile
        values for details.
    */
    OutputColorSpace outputColorSpace;

    /** Path to custom output ICC profile to define the color workspace.
    */
    QString outputProfile;

    /** Path to text file including dead pixel list.
    */
    QString deadPixelMap;

    /** Rectangle used to calculate the white balance by averaging the region of image.
    */
    QRect whiteBalanceArea;
};

}  // namespace KDcrawIface

#endif /* RAW_DECODING_SETTINGS_H */
