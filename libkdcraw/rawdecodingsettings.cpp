/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-12-09
 * @brief  Raw decoding settings
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2007-2008 by Guillaume Castagnino
 *         <a href="mailto:casta at xwing dot info">casta at xwing dot info</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#define OPTIONFIXCOLORSHIGHLIGHTSENTRY      "FixColorsHighlights"
#define OPTIONDECODESIXTEENBITENTRY         "SixteenBitsImage"
#define OPTIONWHITEBALANCEENTRY             "White Balance"
#define OPTIONCUSTOMWHITEBALANCEENTRY       "Custom White Balance"
#define OPTIONCUSTOMWBGREENENTRY            "Custom White Balance Green"
#define OPTIONFOURCOLORRGBENTRY             "Four Color RGB"
#define OPTIONUNCLIPCOLORSENTRY             "Unclip Color"
#define OPTIONDONTSTRETCHPIXELSENTRY        "Dont Stretch Pixels"
#define OPTIONNOISEREDUCTIONENTRY           "Use Noise Reduction"
#define OPTIONMEDIANFILTERPASSESENTRY       "Median Filter Passes"
#define OPTIONNRTHRESHOLDENTRY              "NR Threshold"
#define OPTIONUSECACORRECTIONENTRY          "EnableCACorrection"
#define OPTIONCAREDMULTIPLIERENTRY          "caRedMultiplier"
#define OPTIONCABLUEMULTIPLIERENTRY         "caBlueMultiplier"
#define OPTIONAUTOBRIGHTNESSENTRY           "AutoBrightness"
#define OPTIONDECODINGQUALITYENTRY          "Decoding Quality"
#define OPTIONINPUTCOLORSPACEENTRY          "Input Color Space"
#define OPTIONOUTPUTCOLORSPACEENTRY         "Output Color Space"
#define OPTIONINPUTCOLORPROFILEENTRY        "Input Color Profile"
#define OPTIONOUTPUTCOLORPROFILEENTRY       "Output Color Profile"
#define OPTIONBRIGHTNESSMULTIPLIERENTRY     "Brightness Multiplier"
#define OPTIONUSEBLACKPOINTENTRY            "Use Black Point"
#define OPTIONBLACKPOINTENTRY               "Black Point"
#define OPTIONUSEWHITEPOINTENTRY            "Use White Point"
#define OPTIONWHITEPOINTENTRY               "White Point"

//-- Extended demosaicing settings ----------------------------------------------------------

#define OPTIONDCBITERATIONSENTRY            "Dcb Iterations"
#define OPTIONDCBENHANCEFLENTRY             "Dcb Enhance Filter"
#define OPTIONFBDDNRENTRY                   "Fbdd Noise Reduction"
#define OPTIONEECIREFINEENTRY               "Eeci Refine"
#define OPTIONESMEDPASSESENTRY              "Es Median Filter Passes"
#define OPTIONAMAZECAREFINEENTRY            "Amaze CA Refine"

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

    enableNoiseReduction       = false;
    NRThreshold                = 100;

    enableCACorrection         = false;
    caMultiplier[0]            = 1.0;
    caMultiplier[1]            = 1.0;

    inputProfile               = QString();
    outputProfile              = QString();

    deadPixelMap               = QString();

    whiteBalanceArea           = QRect();

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations              = -1;
    dcbEnhanceFl               = false;
    fbddNR                     = 0;
    eeciRefine                 = false;
    esMedPasses                = 0;
    amazeCARefine              = false;
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
    enableNoiseReduction    = o.enableNoiseReduction;
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

    dcbIterations              = o.dcbIterations;
    dcbEnhanceFl               = o.dcbEnhanceFl;
    fbddNR                     = o.fbddNR;
    eeciRefine                 = o.eeciRefine;
    esMedPasses                = o.esMedPasses;
    amazeCARefine              = o.amazeCARefine;
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

        //-- Extended demosaicing settings ----------------------------------------------------------

        && dcbIterations           == o.dcbIterations
        && dcbEnhanceFl            == o.dcbEnhanceFl
        && fbddNR                  == o.fbddNR
        && eeciRefine              == o.eeciRefine
        && esMedPasses             == o.esMedPasses
        && amazeCARefine           == o.amazeCARefine
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

    enableNoiseReduction    = false;
    NRThreshold             = 100;

    enableCACorrection      = false;
    caMultiplier[0]         = 1.0;
    caMultiplier[1]         = 1.0;

    inputProfile            = QString();
    outputProfile           = QString();

    deadPixelMap            = QString();

    whiteBalanceArea        = QRect();

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = -1;
    dcbEnhanceFl            = false;
    fbddNR                  = 0;
    eeciRefine              = false;
    esMedPasses             = 0;
    amazeCARefine           = false;
}

void RawDecodingSettings::readSettings(KConfigGroup& group)
{
    RawDecodingSettings defaultPrm;

    fixColorsHighlights     = group.readEntry(OPTIONFIXCOLORSHIGHLIGHTSENTRY,   defaultPrm.fixColorsHighlights);
    sixteenBitsImage        = group.readEntry(OPTIONDECODESIXTEENBITENTRY,      defaultPrm.sixteenBitsImage);
    whiteBalance            = (WhiteBalance)
                              group.readEntry(OPTIONWHITEBALANCEENTRY,          (int)defaultPrm.whiteBalance);
    customWhiteBalance      = group.readEntry(OPTIONCUSTOMWHITEBALANCEENTRY,    defaultPrm.customWhiteBalance);
    customWhiteBalanceGreen = group.readEntry(OPTIONCUSTOMWBGREENENTRY,         defaultPrm.customWhiteBalanceGreen);
    RGBInterpolate4Colors   = group.readEntry(OPTIONFOURCOLORRGBENTRY,          defaultPrm.RGBInterpolate4Colors);
    unclipColors            = group.readEntry(OPTIONUNCLIPCOLORSENTRY,          defaultPrm.unclipColors);
    DontStretchPixels       = group.readEntry(OPTIONDONTSTRETCHPIXELSENTRY,     defaultPrm.DontStretchPixels);
    enableNoiseReduction    = group.readEntry(OPTIONNOISEREDUCTIONENTRY,        defaultPrm.enableNoiseReduction);
    brightness              = group.readEntry(OPTIONBRIGHTNESSMULTIPLIERENTRY,  defaultPrm.brightness);
    enableBlackPoint        = group.readEntry(OPTIONUSEBLACKPOINTENTRY,         defaultPrm.enableBlackPoint);
    blackPoint              = group.readEntry(OPTIONBLACKPOINTENTRY,            defaultPrm.blackPoint);
    enableWhitePoint        = group.readEntry(OPTIONUSEWHITEPOINTENTRY,         defaultPrm.enableWhitePoint);
    whitePoint              = group.readEntry(OPTIONWHITEPOINTENTRY,            defaultPrm.whitePoint);
    medianFilterPasses      = group.readEntry(OPTIONMEDIANFILTERPASSESENTRY,    defaultPrm.medianFilterPasses);
    NRThreshold             = group.readEntry(OPTIONNRTHRESHOLDENTRY,           defaultPrm.NRThreshold);
    enableCACorrection      = group.readEntry(OPTIONUSECACORRECTIONENTRY,       defaultPrm.enableCACorrection);
    caMultiplier[0]         = group.readEntry(OPTIONCAREDMULTIPLIERENTRY,       defaultPrm.caMultiplier[0]);
    caMultiplier[1]         = group.readEntry(OPTIONCABLUEMULTIPLIERENTRY,      defaultPrm.caMultiplier[1]);
    RAWQuality              = (DecodingQuality)
                              group.readEntry(OPTIONDECODINGQUALITYENTRY,       (int)defaultPrm.RAWQuality);
    outputColorSpace        = (OutputColorSpace)
                              group.readEntry(OPTIONOUTPUTCOLORSPACEENTRY,      (int)defaultPrm.outputColorSpace);
    autoBrightness          = group.readEntry(OPTIONAUTOBRIGHTNESSENTRY,        defaultPrm.autoBrightness);

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = group.readEntry(OPTIONDCBITERATIONSENTRY,         defaultPrm.dcbIterations);
    dcbEnhanceFl            = group.readEntry(OPTIONDCBENHANCEFLENTRY,          defaultPrm.dcbEnhanceFl);
    fbddNR                  = group.readEntry(OPTIONFBDDNRENTRY,                defaultPrm.fbddNR);
    eeciRefine              = group.readEntry(OPTIONEECIREFINEENTRY,            defaultPrm.eeciRefine);
    esMedPasses             = group.readEntry(OPTIONESMEDPASSESENTRY,           defaultPrm.esMedPasses);
    amazeCARefine           = group.readEntry(OPTIONAMAZECAREFINEENTRY,         defaultPrm.amazeCARefine);
}

void RawDecodingSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry(OPTIONFIXCOLORSHIGHLIGHTSENTRY,  fixColorsHighlights);
    group.writeEntry(OPTIONDECODESIXTEENBITENTRY,     sixteenBitsImage);
    group.writeEntry(OPTIONWHITEBALANCEENTRY,         (int)whiteBalance);
    group.writeEntry(OPTIONCUSTOMWHITEBALANCEENTRY,   customWhiteBalance);
    group.writeEntry(OPTIONCUSTOMWBGREENENTRY,        customWhiteBalanceGreen);
    group.writeEntry(OPTIONFOURCOLORRGBENTRY,         RGBInterpolate4Colors);
    group.writeEntry(OPTIONUNCLIPCOLORSENTRY,         unclipColors);
    group.writeEntry(OPTIONDONTSTRETCHPIXELSENTRY,    DontStretchPixels);
    group.writeEntry(OPTIONNOISEREDUCTIONENTRY,       enableNoiseReduction);
    group.writeEntry(OPTIONBRIGHTNESSMULTIPLIERENTRY, brightness);
    group.writeEntry(OPTIONUSEBLACKPOINTENTRY,        enableBlackPoint);
    group.writeEntry(OPTIONBLACKPOINTENTRY,           blackPoint);
    group.writeEntry(OPTIONUSEWHITEPOINTENTRY,        enableWhitePoint);
    group.writeEntry(OPTIONWHITEPOINTENTRY,           whitePoint);
    group.writeEntry(OPTIONMEDIANFILTERPASSESENTRY,   medianFilterPasses);
    group.writeEntry(OPTIONNRTHRESHOLDENTRY,          NRThreshold);
    group.writeEntry(OPTIONUSECACORRECTIONENTRY,      enableCACorrection);
    group.writeEntry(OPTIONCAREDMULTIPLIERENTRY,      caMultiplier[0]);
    group.writeEntry(OPTIONCABLUEMULTIPLIERENTRY,     caMultiplier[1]);
    group.writeEntry(OPTIONDECODINGQUALITYENTRY,      (int)RAWQuality);
    group.writeEntry(OPTIONOUTPUTCOLORSPACEENTRY,     (int)outputColorSpace);
    group.writeEntry(OPTIONAUTOBRIGHTNESSENTRY,       autoBrightness);

    //-- Extended demosaicing settings ----------------------------------------------------------

    group.writeEntry(OPTIONDCBITERATIONSENTRY,        dcbIterations);
    group.writeEntry(OPTIONDCBENHANCEFLENTRY,         dcbEnhanceFl);
    group.writeEntry(OPTIONFBDDNRENTRY,               fbddNR);
    group.writeEntry(OPTIONEECIREFINEENTRY,           eeciRefine);
    group.writeEntry(OPTIONESMEDPASSESENTRY,          esMedPasses);
    group.writeEntry(OPTIONAMAZECAREFINEENTRY,        amazeCARefine);
}

QDebug operator<<(QDebug dbg, const RawDecodingSettings& s)
{
    dbg.nospace() << "RawDecodingSettings::autoBrightness: "          << s.autoBrightness          << ", ";
    dbg.nospace() << "RawDecodingSettings::sixteenBitsImage: "        << s.sixteenBitsImage        << ", ";
    dbg.nospace() << "RawDecodingSettings::brightness: "              << s.brightness              << ", ";
    dbg.nospace() << "RawDecodingSettings::RAWQuality: "              << s.RAWQuality              << ", ";
    dbg.nospace() << "RawDecodingSettings::inputColorSpace: "         << s.inputColorSpace         << ", ";
    dbg.nospace() << "RawDecodingSettings::outputColorSpace: "        << s.outputColorSpace        << ", ";
    dbg.nospace() << "RawDecodingSettings::RGBInterpolate4Colors: "   << s.RGBInterpolate4Colors   << ", ";
    dbg.nospace() << "RawDecodingSettings::DontStretchPixels: "       << s.DontStretchPixels       << ", ";
    dbg.nospace() << "RawDecodingSettings::unclipColors: "            << s.unclipColors            << ", ";
    dbg.nospace() << "RawDecodingSettings::whiteBalance: "            << s.whiteBalance            << ", ";
    dbg.nospace() << "RawDecodingSettings::customWhiteBalance: "      << s.customWhiteBalance      << ", ";
    dbg.nospace() << "RawDecodingSettings::customWhiteBalanceGreen: " << s.customWhiteBalanceGreen << ", ";
    dbg.nospace() << "RawDecodingSettings::halfSizeColorImage: "      << s.halfSizeColorImage      << ", ";
    dbg.nospace() << "RawDecodingSettings::enableBlackPoint: "        << s.enableBlackPoint        << ", ";
    dbg.nospace() << "RawDecodingSettings::blackPoint: "              << s.blackPoint              << ", ";
    dbg.nospace() << "RawDecodingSettings::enableWhitePoint: "        << s.enableWhitePoint        << ", ";
    dbg.nospace() << "RawDecodingSettings::whitePoint: "              << s.whitePoint              << ", ";
    dbg.nospace() << "RawDecodingSettings::enableNoiseReduction: "    << s.enableNoiseReduction    << ", ";
    dbg.nospace() << "RawDecodingSettings::NRThreshold: "             << s.NRThreshold             << ", ";
    dbg.nospace() << "RawDecodingSettings::enableCACorrection: "      << s.enableCACorrection      << ", ";
    dbg.nospace() << "RawDecodingSettings::caMultiplier: "            << s.caMultiplier            << ", ";
    dbg.nospace() << "RawDecodingSettings::medianFilterPasses: "      << s.medianFilterPasses      << ", ";
    dbg.nospace() << "RawDecodingSettings::inputProfile: "            << s.inputProfile            << ", ";
    dbg.nospace() << "RawDecodingSettings::outputProfile: "           << s.outputProfile           << ", ";
    dbg.nospace() << "RawDecodingSettings::deadPixelMap: "            << s.deadPixelMap            << ", ";
    dbg.nospace() << "RawDecodingSettings::whiteBalanceArea: "        << s.whiteBalanceArea        << ", ";

    //-- Extended demosaicing settings ----------------------------------------------------------

    dbg.nospace() << "RawDecodingSettings::dcbIterations: "           << s.dcbIterations           << ", ";
    dbg.nospace() << "RawDecodingSettings::dcbEnhanceFl: "            << s.dcbEnhanceFl            << ", ";
    dbg.nospace() << "RawDecodingSettings::fbddNR: "                  << s.fbddNR                  << ", ";
    dbg.nospace() << "RawDecodingSettings::eeciRefine: "              << s.eeciRefine              << ", ";
    dbg.nospace() << "RawDecodingSettings::esMedPasses: "             << s.esMedPasses             << ", ";
    dbg.nospace() << "RawDecodingSettings::amazeCARefine: "           << s.amazeCARefine;

    return dbg.space();
}

}  // namespace KDcrawIface
