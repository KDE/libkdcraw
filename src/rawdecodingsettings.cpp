/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2013 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
    SPDX-FileCopyrightText: 2007-2008 Guillaume Castagnino <casta at xwing dot info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rawdecodingsettings.h"

namespace KDcrawIface
{

RawDecodingSettings::RawDecodingSettings()
{
    fixColorsHighlights        = false;
    autoBrightness             = true;
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

    NRType                     = NONR;
    NRThreshold                = 0;

    enableCACorrection         = false;
    caMultiplier[0]            = 0.0;
    caMultiplier[1]            = 0.0;

    inputProfile               = QString();
    outputProfile              = QString();

    deadPixelMap               = QString();

    whiteBalanceArea           = QRect();

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations              = -1;
    dcbEnhanceFl               = false;
    eeciRefine                 = false;
    esMedPasses                = 0;
    NRChroThreshold            = 0;
    expoCorrection             = false;
    expoCorrectionShift        = 1.0;
    expoCorrectionHighlight    = 0.0;
}

RawDecodingSettings::~RawDecodingSettings()
{
}

RawDecodingSettings& RawDecodingSettings::operator=(const RawDecodingSettings& o)
{
    fixColorsHighlights     = o.fixColorsHighlights;
    autoBrightness          = o.autoBrightness;
    sixteenBitsImage        = o.sixteenBitsImage;
    brightness              = o.brightness;
    RAWQuality              = o.RAWQuality;
    inputColorSpace         = o.inputColorSpace;
    outputColorSpace        = o.outputColorSpace;
    RGBInterpolate4Colors   = o.RGBInterpolate4Colors;
    DontStretchPixels       = o.DontStretchPixels;
    unclipColors            = o.unclipColors;
    whiteBalance            = o.whiteBalance;
    customWhiteBalance      = o.customWhiteBalance;
    customWhiteBalanceGreen = o.customWhiteBalanceGreen;
    halfSizeColorImage      = o.halfSizeColorImage;
    enableBlackPoint        = o.enableBlackPoint;
    blackPoint              = o.blackPoint;
    enableWhitePoint        = o.enableWhitePoint;
    whitePoint              = o.whitePoint;
    NRType                  = o.NRType;
    NRThreshold             = o.NRThreshold;
    enableCACorrection      = o.enableCACorrection;
    caMultiplier[0]         = o.caMultiplier[0];
    caMultiplier[1]         = o.caMultiplier[1];
    medianFilterPasses      = o.medianFilterPasses;
    inputProfile            = o.inputProfile;
    outputProfile           = o.outputProfile;
    deadPixelMap            = o.deadPixelMap;
    whiteBalanceArea        = o.whiteBalanceArea;

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = o.dcbIterations;
    dcbEnhanceFl            = o.dcbEnhanceFl;
    eeciRefine              = o.eeciRefine;
    esMedPasses             = o.esMedPasses;
    NRChroThreshold         = o.NRChroThreshold;
    expoCorrection          = o.expoCorrection;
    expoCorrectionShift     = o.expoCorrectionShift;
    expoCorrectionHighlight = o.expoCorrectionHighlight;

    return *this;
}

bool RawDecodingSettings::operator==(const RawDecodingSettings& o) const
{
    return fixColorsHighlights     == o.fixColorsHighlights
        && autoBrightness          == o.autoBrightness
        && sixteenBitsImage        == o.sixteenBitsImage
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
        && NRType                  == o.NRType
        && NRThreshold             == o.NRThreshold
        && enableCACorrection      == o.enableCACorrection
        && caMultiplier[0]         == o.caMultiplier[0]
        && caMultiplier[1]         == o.caMultiplier[1]
        && medianFilterPasses      == o.medianFilterPasses
        && inputProfile            == o.inputProfile
        && outputProfile           == o.outputProfile
        && deadPixelMap            == o.deadPixelMap
        && whiteBalanceArea        == o.whiteBalanceArea

        //-- Extended demosaicing settings ----------------------------------------------------------

        && dcbIterations           == o.dcbIterations
        && dcbEnhanceFl            == o.dcbEnhanceFl
        && eeciRefine              == o.eeciRefine
        && esMedPasses             == o.esMedPasses
        && NRChroThreshold         == o.NRChroThreshold
        && expoCorrection          == o.expoCorrection
        && expoCorrectionShift     == o.expoCorrectionShift
        && expoCorrectionHighlight == o.expoCorrectionHighlight
        ;
}

void RawDecodingSettings::optimizeTimeLoading()
{
    fixColorsHighlights     = false;
    autoBrightness          = true;
    sixteenBitsImage        = true;
    brightness              = 1.0;
    RAWQuality              = BILINEAR;
    inputColorSpace         = NOINPUTCS;
    outputColorSpace        = SRGB;
    RGBInterpolate4Colors   = false;
    DontStretchPixels       = false;
    unclipColors            = 0;
    whiteBalance            = CAMERA;
    customWhiteBalance      = 6500;
    customWhiteBalanceGreen = 1.0;
    halfSizeColorImage      = true;
    medianFilterPasses      = 0;

    enableBlackPoint        = false;
    blackPoint              = 0;

    enableWhitePoint        = false;
    whitePoint              = 0;

    NRType                  = NONR;
    NRThreshold             = 0;

    enableCACorrection      = false;
    caMultiplier[0]         = 0.0;
    caMultiplier[1]         = 0.0;

    inputProfile            = QString();
    outputProfile           = QString();

    deadPixelMap            = QString();

    whiteBalanceArea        = QRect();

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = -1;
    dcbEnhanceFl            = false;
    eeciRefine              = false;
    esMedPasses             = 0;
    NRChroThreshold         = 0;
    expoCorrection          = false;
    expoCorrectionShift     = 1.0;
    expoCorrectionHighlight = 0.0;
}

QDebug operator<<(QDebug dbg, const RawDecodingSettings& s)
{
    dbg.nospace() << '\n';
    dbg.nospace() << "-- RAW DECODING SETTINGS --------------------------------" << '\n';
    dbg.nospace() << "-- autoBrightness:          " << s.autoBrightness          << '\n';
    dbg.nospace() << "-- sixteenBitsImage:        " << s.sixteenBitsImage        << '\n';
    dbg.nospace() << "-- brightness:              " << s.brightness              << '\n';
    dbg.nospace() << "-- RAWQuality:              " << s.RAWQuality              << '\n';
    dbg.nospace() << "-- inputColorSpace:         " << s.inputColorSpace         << '\n';
    dbg.nospace() << "-- outputColorSpace:        " << s.outputColorSpace        << '\n';
    dbg.nospace() << "-- RGBInterpolate4Colors:   " << s.RGBInterpolate4Colors   << '\n';
    dbg.nospace() << "-- DontStretchPixels:       " << s.DontStretchPixels       << '\n';
    dbg.nospace() << "-- unclipColors:            " << s.unclipColors            << '\n';
    dbg.nospace() << "-- whiteBalance:            " << s.whiteBalance            << '\n';
    dbg.nospace() << "-- customWhiteBalance:      " << s.customWhiteBalance      << '\n';
    dbg.nospace() << "-- customWhiteBalanceGreen: " << s.customWhiteBalanceGreen << '\n';
    dbg.nospace() << "-- halfSizeColorImage:      " << s.halfSizeColorImage      << '\n';
    dbg.nospace() << "-- enableBlackPoint:        " << s.enableBlackPoint        << '\n';
    dbg.nospace() << "-- blackPoint:              " << s.blackPoint              << '\n';
    dbg.nospace() << "-- enableWhitePoint:        " << s.enableWhitePoint        << '\n';
    dbg.nospace() << "-- whitePoint:              " << s.whitePoint              << '\n';
    dbg.nospace() << "-- NoiseReductionType:      " << s.NRType                  << '\n';
    dbg.nospace() << "-- NoiseReductionThreshold: " << s.NRThreshold             << '\n';
    dbg.nospace() << "-- enableCACorrection:      " << s.enableCACorrection      << '\n';
    dbg.nospace() << "-- caMultiplier:            " << s.caMultiplier[0]
                  << ", "                           << s.caMultiplier[1]         << '\n';
    dbg.nospace() << "-- medianFilterPasses:      " << s.medianFilterPasses      << '\n';
    dbg.nospace() << "-- inputProfile:            " << s.inputProfile            << '\n';
    dbg.nospace() << "-- outputProfile:           " << s.outputProfile           << '\n';
    dbg.nospace() << "-- deadPixelMap:            " << s.deadPixelMap            << '\n';
    dbg.nospace() << "-- whiteBalanceArea:        " << s.whiteBalanceArea        << '\n';

    //-- Extended demosaicing settings ----------------------------------------------------------

    dbg.nospace() << "-- dcbIterations:           " << s.dcbIterations           << '\n';
    dbg.nospace() << "-- dcbEnhanceFl:            " << s.dcbEnhanceFl            << '\n';
    dbg.nospace() << "-- eeciRefine:              " << s.eeciRefine              << '\n';
    dbg.nospace() << "-- esMedPasses:             " << s.esMedPasses             << '\n';
    dbg.nospace() << "-- NRChrominanceThreshold:  " << s.NRChroThreshold         << '\n';
    dbg.nospace() << "-- expoCorrection:          " << s.expoCorrection          << '\n';
    dbg.nospace() << "-- expoCorrectionShift:     " << s.expoCorrectionShift     << '\n';
    dbg.nospace() << "-- expoCorrectionHighlight: " << s.expoCorrectionHighlight << '\n';
    dbg.nospace() << "---------------------------------------------------------" << '\n';

    return dbg.space();
}

}  // namespace KDcrawIface
