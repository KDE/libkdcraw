/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : Raw decoding settings
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// Local includes

#include "rawdecodingsettings.h"

namespace KDcrawIface
{

RawDecodingSettings::RawDecodingSettings()
{
    autoBrightness             = true;
    sixteenBitsImage           = false;
    brightness                 = 1.0;
    RAWQuality                 = BILINEAR;
    inputColorSpace            = NOINPUTCS;
    outputColorSpace           = SRGB;
    RGBInterpolate4Colors      = false;
    dontStretchPixels          = false;
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
}

bool RawDecodingSettings::operator==(const RawDecodingSettings &o) const
{
    return autoBrightness          == o.autoBrightness
        && sixteenBitsImage        == o.sixteenBitsImage
        && brightness              == o.brightness
        && RAWQuality              == o.RAWQuality
        && inputColorSpace         == o.inputColorSpace
        && outputColorSpace        == o.outputColorSpace
        && RGBInterpolate4Colors   == o.RGBInterpolate4Colors
        && dontStretchPixels       == o.dontStretchPixels
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
}

RawDecodingSettings::~RawDecodingSettings()
{
}

void RawDecodingSettings::optimizeTimeLoading()
{
    autoBrightness          = true;
    sixteenBitsImage        = true;
    brightness              = 1.0;
    RAWQuality              = BILINEAR;
    inputColorSpace         = NOINPUTCS;
    outputColorSpace        = SRGB;
    RGBInterpolate4Colors   = false;
    dontStretchPixels       = false;
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

    enableNoiseReduction    = false;
    NRThreshold             = 100;

    enableCACorrection      = false;
    caMultiplier[0]         = 1.0;
    caMultiplier[1]         = 1.0;

    inputProfile            = QString();
    outputProfile           = QString();

    deadPixelMap            = QString();

    whiteBalanceArea        = QRect();
}

QDebug operator<<(QDebug dbg, const RawDecodingSettings& s)
{
    dbg.nospace() << "RawDecodingSettings::autoBrightness: "          << s.autoBrightness   << ", ";
    dbg.nospace() << "RawDecodingSettings::sixteenBitsImage: "        << s.sixteenBitsImage << ", ";
    dbg.nospace() << "RawDecodingSettings::brightness: "              << s.brightness << ", ";
    dbg.nospace() << "RawDecodingSettings::RAWQuality: "              << s.RAWQuality << ", ";
    dbg.nospace() << "RawDecodingSettings::inputColorSpace: "         << s.inputColorSpace << ", ";
    dbg.nospace() << "RawDecodingSettings::outputColorSpace: "        << s.outputColorSpace << ", ";
    dbg.nospace() << "RawDecodingSettings::RGBInterpolate4Colors: "   << s.RGBInterpolate4Colors << ", ";
    dbg.nospace() << "RawDecodingSettings::dontStretchPixels: "       << s.dontStretchPixels << ", ";
    dbg.nospace() << "RawDecodingSettings::unclipColors: "            << s.unclipColors << ", ";
    dbg.nospace() << "RawDecodingSettings::whiteBalance: "            << s.whiteBalance << ", ";
    dbg.nospace() << "RawDecodingSettings::customWhiteBalance: "      << s.customWhiteBalance << ", ";
    dbg.nospace() << "RawDecodingSettings::customWhiteBalanceGreen: " << s.customWhiteBalanceGreen << ", ";
    dbg.nospace() << "RawDecodingSettings::halfSizeColorImage: "      << s.halfSizeColorImage << ", ";
    dbg.nospace() << "RawDecodingSettings::enableBlackPoint: "        << s.enableBlackPoint << ", ";
    dbg.nospace() << "RawDecodingSettings::blackPoint: "              << s.blackPoint << ", ";
    dbg.nospace() << "RawDecodingSettings::enableWhitePoint: "        << s.enableWhitePoint << ", ";
    dbg.nospace() << "RawDecodingSettings::whitePoint: "              << s.whitePoint << ", ";
    dbg.nospace() << "RawDecodingSettings::enableNoiseReduction: "    << s.enableNoiseReduction << ", ";
    dbg.nospace() << "RawDecodingSettings::NRThreshold: "             << s.NRThreshold << ", ";
    dbg.nospace() << "RawDecodingSettings::enableCACorrection: "      << s.enableCACorrection << ", ";
    dbg.nospace() << "RawDecodingSettings::caMultiplier: "            << s.caMultiplier << ", ";
    dbg.nospace() << "RawDecodingSettings::medianFilterPasses: "      << s.medianFilterPasses << ", ";
    dbg.nospace() << "RawDecodingSettings::inputProfile: "            << s.inputProfile << ", ";
    dbg.nospace() << "RawDecodingSettings::outputProfile: "           << s.outputProfile << ", ";
    dbg.nospace() << "RawDecodingSettings::deadPixelMap: "            << s.deadPixelMap << ", ";
    dbg.nospace() << "RawDecodingSettings::whiteBalanceArea: "        << s.whiteBalanceArea;
    return dbg.space();
}

}  // namespace KDcrawIface
