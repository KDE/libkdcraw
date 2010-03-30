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

class RawDecodingSettingsPriv
{
public:

    RawDecodingSettingsPriv() :
        optionDecodeSixteenBitEntry("SixteenBitsImage"),
        optionWhiteBalanceEntry("White Balance"),
        optionCustomWhiteBalanceEntry("Custom White Balance"),
        optionCustomWBGreenEntry("Custom White Balance Green"),
        optionFourColorRGBEntry("Four Color RGB"),
        optionUnclipColorsEntry("Unclip Color"),
        optionDontStretchPixelsEntry("Dont Stretch Pixels"),
        optionNoiseReductionEntry("Use Noise Reduction"),
        optionMedianFilterPassesEntry("Median Filter Passes"),
        optionNRThresholdEntry("NR Threshold"),
        optionUseCACorrectionEntry("EnableCACorrection"),
        optionCARedMultiplierEntry("caRedMultiplier"),
        optionCABlueMultiplierEntry("caBlueMultiplier"),
        optionAutoBrightnessEntry("AutoBrightness"),
        optionDecodingQualityEntry("Decoding Quality"),
        optionInputColorSpaceEntry("Input Color Space"),
        optionOutputColorSpaceEntry("Output Color Space"),
        optionInputColorProfileEntry("Input Color Profile"),
        optionOutputColorProfileEntry("Output Color Profile"),
        optionBrightnessMultiplierEntry("Brightness Multiplier"),
        optionUseBlackPointEntry("Use Black Point"),
        optionBlackPointEntry("Black Point"),
        optionUseWhitePointEntry("Use White Point"),
        optionWhitePointEntry("White Point")
    {}

    const QString optionDecodeSixteenBitEntry;
    const QString optionWhiteBalanceEntry;
    const QString optionCustomWhiteBalanceEntry;
    const QString optionCustomWBGreenEntry;
    const QString optionFourColorRGBEntry;
    const QString optionUnclipColorsEntry;
    const QString optionDontStretchPixelsEntry;
    const QString optionNoiseReductionEntry;
    const QString optionMedianFilterPassesEntry;
    const QString optionNRThresholdEntry;
    const QString optionUseCACorrectionEntry;
    const QString optionCARedMultiplierEntry;
    const QString optionCABlueMultiplierEntry;
    const QString optionAutoBrightnessEntry;
    const QString optionDecodingQualityEntry;
    const QString optionInputColorSpaceEntry;
    const QString optionOutputColorSpaceEntry;
    const QString optionInputColorProfileEntry;
    const QString optionOutputColorProfileEntry;
    const QString optionBrightnessMultiplierEntry;
    const QString optionUseBlackPointEntry;
    const QString optionBlackPointEntry;
    const QString optionUseWhitePointEntry;
    const QString optionWhitePointEntry;
};

RawDecodingSettings::RawDecodingSettings() :
    d(new RawDecodingSettingsPriv)
{
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

RawDecodingSettings::~RawDecodingSettings()
{
    delete d;
}

RawDecodingSettings& RawDecodingSettings::operator=(const RawDecodingSettings& prm)
{
    d = prm.d;
    return *this;
}

bool RawDecodingSettings::operator==(const RawDecodingSettings& o) const
{
    return autoBrightness          == o.autoBrightness
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

void RawDecodingSettings::optimizeTimeLoading()
{
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

void RawDecodingSettings::readSettings(KConfigGroup& group)
{
    RawDecodingSettings defaultPrm;

    sixteenBitsImage        = group.readEntry(d->optionDecodeSixteenBitEntry, defaultPrm.sixteenBitsImage);
    whiteBalance            = (WhiteBalance)group.readEntry(d->optionWhiteBalanceEntry, (int)defaultPrm.whiteBalance);
    customWhiteBalance      = group.readEntry(d->optionCustomWhiteBalanceEntry, defaultPrm.customWhiteBalance);
    customWhiteBalanceGreen = group.readEntry(d->optionCustomWBGreenEntry, defaultPrm.customWhiteBalanceGreen);
    RGBInterpolate4Colors   = group.readEntry(d->optionFourColorRGBEntry, defaultPrm.RGBInterpolate4Colors);
    unclipColors            = group.readEntry(d->optionUnclipColorsEntry, defaultPrm.unclipColors);
    DontStretchPixels       = group.readEntry(d->optionDontStretchPixelsEntry, defaultPrm.DontStretchPixels);
    enableNoiseReduction    = group.readEntry(d->optionNoiseReductionEntry, defaultPrm.enableNoiseReduction);
    brightness              = group.readEntry(d->optionBrightnessMultiplierEntry, defaultPrm.brightness);
    enableBlackPoint        = group.readEntry(d->optionUseBlackPointEntry, defaultPrm.enableBlackPoint);
    blackPoint              = group.readEntry(d->optionBlackPointEntry, defaultPrm.blackPoint);
    enableWhitePoint        = group.readEntry(d->optionUseWhitePointEntry, defaultPrm.enableWhitePoint);
    whitePoint              = group.readEntry(d->optionWhitePointEntry, defaultPrm.whitePoint);
    medianFilterPasses      = group.readEntry(d->optionMedianFilterPassesEntry, defaultPrm.medianFilterPasses);
    NRThreshold             = group.readEntry(d->optionNRThresholdEntry, defaultPrm.NRThreshold);
    enableCACorrection      = group.readEntry(d->optionUseCACorrectionEntry, defaultPrm.enableCACorrection);
    caMultiplier[0]         = group.readEntry(d->optionCARedMultiplierEntry, defaultPrm.caMultiplier[0]);
    caMultiplier[1]         = group.readEntry(d->optionCABlueMultiplierEntry, defaultPrm.caMultiplier[1]);
    RAWQuality              = (DecodingQuality)group.readEntry(d->optionDecodingQualityEntry, (int)defaultPrm.RAWQuality);
    outputColorSpace        = (OutputColorSpace)group.readEntry(d->optionOutputColorSpaceEntry, (int)defaultPrm.outputColorSpace);
    autoBrightness          = group.readEntry(d->optionAutoBrightnessEntry, defaultPrm.autoBrightness);
}

void RawDecodingSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry(d->optionDecodeSixteenBitEntry,     sixteenBitsImage);
    group.writeEntry(d->optionWhiteBalanceEntry,         (int)whiteBalance);
    group.writeEntry(d->optionCustomWhiteBalanceEntry,   customWhiteBalance);
    group.writeEntry(d->optionCustomWBGreenEntry,        customWhiteBalanceGreen);
    group.writeEntry(d->optionFourColorRGBEntry,         RGBInterpolate4Colors);
    group.writeEntry(d->optionUnclipColorsEntry,         unclipColors);
    group.writeEntry(d->optionDontStretchPixelsEntry,    DontStretchPixels);
    group.writeEntry(d->optionNoiseReductionEntry,       enableNoiseReduction);
    group.writeEntry(d->optionBrightnessMultiplierEntry, brightness);
    group.writeEntry(d->optionUseBlackPointEntry,        enableBlackPoint);
    group.writeEntry(d->optionBlackPointEntry,           blackPoint);
    group.writeEntry(d->optionUseWhitePointEntry,        enableWhitePoint);
    group.writeEntry(d->optionWhitePointEntry,           whitePoint);
    group.writeEntry(d->optionMedianFilterPassesEntry,   medianFilterPasses);
    group.writeEntry(d->optionNRThresholdEntry,          NRThreshold);
    group.writeEntry(d->optionUseCACorrectionEntry,      enableCACorrection);
    group.writeEntry(d->optionCARedMultiplierEntry,      caMultiplier[0]);
    group.writeEntry(d->optionCABlueMultiplierEntry,     caMultiplier[1]);
    group.writeEntry(d->optionDecodingQualityEntry,      (int)RAWQuality);
    group.writeEntry(d->optionOutputColorSpaceEntry,     (int)outputColorSpace);
    group.writeEntry(d->optionAutoBrightnessEntry,       autoBrightness);  
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
    dbg.nospace() << "RawDecodingSettings::DontStretchPixels: "       << s.DontStretchPixels << ", ";
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
