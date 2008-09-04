/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-13
 * Description : dcraw settings widgets
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

// Qt includes.

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QWhatsThis>
#include <QToolTip>

// KDE includes.

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <kurllabel.h>
#include <ktoolinvocation.h>

// Local includes.

#include "rnuminput.h"
#include "rcombobox.h"
#include "dcrawsettingswidget.h"
#include "dcrawsettingswidget.moc"

namespace KDcrawIface
{

class DcrawSettingsWidgetPriv
{
public:

    DcrawSettingsWidgetPriv()
    {
        sixteenBitsImage               = 0;
        fourColorCheckBox              = 0;
        brightnessLabel                = 0;
        brightnessSpinBox              = 0;
        blackPointCheckBox             = 0;
        blackPointSpinBox              = 0;
        whitePointCheckBox             = 0;
        whitePointSpinBox              = 0;
        whiteBalanceComboBox           = 0;
        whiteBalanceLabel              = 0;
        customWhiteBalanceSpinBox      = 0;
        customWhiteBalanceLabel        = 0;
        customWhiteBalanceGreenSpinBox = 0;
        customWhiteBalanceGreenLabel   = 0;
        unclipColorLabel               = 0;
        dontStretchPixelsCheckBox      = 0;
        RAWQualityComboBox             = 0;
        RAWQualityLabel                = 0;
        enableNoiseReduction           = 0;
        NRThresholdSpinBox             = 0;
        NRThresholdLabel               = 0;
        enableCACorrection             = 0;
        caRedMultSpinBox               = 0;
        caBlueMultSpinBox              = 0;
        caRedMultLabel                 = 0;
        caBlueMultLabel                = 0;
        unclipColorComboBox            = 0;
        reconstructLabel               = 0;
        reconstructSpinBox             = 0;
        outputColorSpaceLabel          = 0;
        outputColorSpaceComboBox       = 0;
        demosaicingSettings            = 0;
        whiteBalanceSettings           = 0;
        correctionsSettings            = 0;
        colormanSettings               = 0;
        medianFilterPassesSpinBox      = 0;
        medianFilterPassesLabel        = 0;
        inIccUrlEdit                   = 0;
        outIccUrlEdit                  = 0;
        inputColorSpaceLabel           = 0;
        inputColorSpaceComboBox        = 0;
    }

    QWidget         *demosaicingSettings;
    QWidget         *whiteBalanceSettings;
    QWidget         *correctionsSettings;
    QWidget         *colormanSettings;

    QLabel          *whiteBalanceLabel;
    QLabel          *customWhiteBalanceLabel;
    QLabel          *customWhiteBalanceGreenLabel;
    QLabel          *brightnessLabel;
    QLabel          *RAWQualityLabel;
    QLabel          *NRThresholdLabel;
    QLabel          *caRedMultLabel;
    QLabel          *caBlueMultLabel;
    QLabel          *unclipColorLabel;
    QLabel          *reconstructLabel;
    QLabel          *inputColorSpaceLabel;
    QLabel          *outputColorSpaceLabel;
    QLabel          *medianFilterPassesLabel;

    QCheckBox       *blackPointCheckBox;
    QCheckBox       *whitePointCheckBox;
    QCheckBox       *sixteenBitsImage;
    QCheckBox       *fourColorCheckBox;
    QCheckBox       *dontStretchPixelsCheckBox;
    QCheckBox       *enableNoiseReduction;
    QCheckBox       *enableCACorrection;

    KUrlRequester   *inIccUrlEdit;
    KUrlRequester   *outIccUrlEdit;

    RComboBox       *whiteBalanceComboBox;
    RComboBox       *RAWQualityComboBox;
    RComboBox       *unclipColorComboBox;
    RComboBox       *inputColorSpaceComboBox;
    RComboBox       *outputColorSpaceComboBox;

    RIntNumInput    *customWhiteBalanceSpinBox;
    RIntNumInput    *reconstructSpinBox;
    RIntNumInput    *blackPointSpinBox;
    RIntNumInput    *whitePointSpinBox;
    RIntNumInput    *NRThresholdSpinBox;
    RIntNumInput    *medianFilterPassesSpinBox;

    RDoubleNumInput *customWhiteBalanceGreenSpinBox;
    RDoubleNumInput *caRedMultSpinBox;
    RDoubleNumInput *caBlueMultSpinBox;
    RDoubleNumInput *brightnessSpinBox;
};

DcrawSettingsWidget::DcrawSettingsWidget(QWidget *parent, bool sixteenBitsOption, 
                                         bool outputColorSpaceOption, 
                                         bool postProcessingOptions)
                   : QToolBox(parent)
{
    d = new DcrawSettingsWidgetPriv;

    // ---------------------------------------------------------------
    // DEMOSAICING Settings panel

    d->demosaicingSettings         = new QWidget(this);
    QGridLayout* demosaicingLayout = new QGridLayout(d->demosaicingSettings);

    int line = 0;

    d->sixteenBitsImage = new QCheckBox(i18n("16 bits color depth"), d->demosaicingSettings);
    d->sixteenBitsImage->setWhatsThis(i18n("<p>If enabled, all RAW files will be decoded in 16-bit "
                                           "color depth using a linear gamma curve. To prevent dark "
                                           "picture rendering in the editor, it is recommended to use "
                                           "Color Management in this mode.<p>"
                                           "If disabled, all RAW files will be decoded in 8-bit "
                                           "color depth with a BT.709 gamma curve and a 99th-percentile "
                                           "white point. This mode is faster than 16-bit decoding."));
    demosaicingLayout->addWidget(d->sixteenBitsImage, 0, 0, 1, 1);

    if (sixteenBitsOption)
    {
        d->sixteenBitsImage->show();
        line = 1;
    }
    else
    {
        d->sixteenBitsImage->hide();
    }

    d->fourColorCheckBox = new QCheckBox(i18n("Interpolate RGB as four colors"), d->demosaicingSettings);
    d->fourColorCheckBox->setWhatsThis(i18n("<p><b>Interpolate RGB as four colors</b><p>"
                                            "The default is to assume that all green "
                                            "pixels are the same. If even-row green "
                                            "pixels are more sensitive to ultraviolet light "
                                            "than odd-row this difference causes a mesh "
                                            "pattern in the output; using this option solves "
                                            "this problem with minimal loss of detail.<p>"
                                            "To resume, this option blurs the image "
                                            "a little, but it eliminates false 2x2 mesh patterns "
                                            "with VNG quality method or mazes with AHD quality method."));
    demosaicingLayout->addWidget(d->fourColorCheckBox, line, 0, 1, 2);
    line++;

    KUrlLabel *dcrawVersion = new KUrlLabel("http://www.libraw.org", QString("libraw %1")
                                  /*.arg( FIXME DcrawBinary::internalVersion())*/, d->demosaicingSettings);
    dcrawVersion->setAlignment(Qt::AlignRight);
    dcrawVersion->setToolTip(i18n("Visit dcraw project website"));
    demosaicingLayout->addWidget(dcrawVersion, 0, 2, 1, 1);

    d->dontStretchPixelsCheckBox = new QCheckBox(i18n("Do not stretch or rotate pixels"), d->demosaicingSettings);
    d->dontStretchPixelsCheckBox->setWhatsThis(i18n("<p><b>Do not stretch or rotate pixels</b><p>"
                                                    "For Fuji Super CCD cameras, show the image tilted 45 "
                                                    "degrees. For cameras with non-square pixels, do not "
                                                    "stretch the image to its correct aspect ratio. In any "
                                                    "case, this option guarantees that each output pixel "
                                                    "corresponds to one RAW pixel.<p>"));
    demosaicingLayout->addWidget(d->dontStretchPixelsCheckBox, line, 0, 1, 2);
    line++;

    d->RAWQualityLabel    = new QLabel(i18n("Quality:"), d->demosaicingSettings);
    d->RAWQualityComboBox = new RComboBox(d->demosaicingSettings);
    d->RAWQualityComboBox->insertItem(RawDecodingSettings::BILINEAR, i18n("Bilinear"));
    d->RAWQualityComboBox->insertItem(RawDecodingSettings::VNG,      i18n("VNG"));
    d->RAWQualityComboBox->insertItem(RawDecodingSettings::PPG,      i18n("PPG"));
    d->RAWQualityComboBox->insertItem(RawDecodingSettings::AHD,      i18n("AHD"));
    d->RAWQualityComboBox->setDefaultIndex(RawDecodingSettings::BILINEAR);
    d->RAWQualityComboBox->setWhatsThis(i18n("<p><b>Quality (interpolation)</b><p>"
                "Select here the demosaicing RAW images decoding "
                "interpolation method. A demosaicing algorithm is a digital image process used to "
                "interpolate a complete image from the partial raw data received from the color-filtered "
                "image sensor internal to many digital cameras in form of a matrix of colored pixels. "
                "Also known as CFA interpolation or color reconstruction, another common spelling "
                "is demosaicing. There are 4 methods to demosaicing RAW images:<p>"
                "<b>Bilinear</b>: use high-speed but low-quality bilinear "
                "interpolation (default - for slow computer). In this method, "
                "the red value of a non-red pixel is computed as the average of "
                "the adjacent red pixels, and similar for blue and green.<p>"
                "<b>VNG</b>: use Variable Number of Gradients interpolation. "
                "This method computes gradients near the pixel of interest and uses "
                "the lower gradients (representing smoother and more similar parts "
                "of the image) to make an estimate.<p>"
                "<b>PPG</b>: use Patterned Pixel Grouping interpolation. "
                "Pixel Grouping uses assumptions about natural scenery in making estimates. "
                "It has fewer color artifacts on natural images than the Variable Number of "
                "Gradients method.<p>"
                "<b>AHD</b>: use Adaptive Homogeneity-Directed interpolation. "
                "This method selects the direction of interpolation so as to "
                "maximize a homogeneity metric, thus typically minimizing color artifacts.<p>"));
    demosaicingLayout->addWidget(d->RAWQualityLabel,    line, 0, 1, 1);
    demosaicingLayout->addWidget(d->RAWQualityComboBox, line, 1, 1, 2);
    line++;

    d->medianFilterPassesSpinBox = new RIntNumInput(d->demosaicingSettings);
    d->medianFilterPassesSpinBox->setRange(0, 10, 1);
    d->medianFilterPassesSpinBox->setDefaultValue(0);
    d->medianFilterPassesSpinBox->setSliderEnabled(true);
    d->medianFilterPassesLabel   = new QLabel(i18n("Median:"), d->whiteBalanceSettings);
    d->medianFilterPassesSpinBox->setWhatsThis( i18n("<p><b>Median Filter</b><p>"
                                                     "Set here the passes used by median filter applied after "
                                                     "interpolation to Red-Green and Blue-Green channels."));
    demosaicingLayout->addWidget(d->medianFilterPassesLabel,   line, 0, 1, 1);
    demosaicingLayout->addWidget(d->medianFilterPassesSpinBox, line, 1, 1, 2);

    insertItem(DEMOSAICING, d->demosaicingSettings, i18n("Demosaicing"));

    // ---------------------------------------------------------------
    // WHITE BALANCE Settings Panel

    d->whiteBalanceSettings         = new QWidget(this);
    QGridLayout* whiteBalanceLayout = new QGridLayout(d->whiteBalanceSettings);

    d->whiteBalanceLabel    = new QLabel(i18n("Method"), d->whiteBalanceSettings);
    d->whiteBalanceComboBox = new RComboBox(d->whiteBalanceSettings);
    d->whiteBalanceComboBox->insertItem(RawDecodingSettings::NONE,   i18n("Default D65"));
    d->whiteBalanceComboBox->insertItem(RawDecodingSettings::CAMERA, i18n("Camera"));
    d->whiteBalanceComboBox->insertItem(RawDecodingSettings::AUTO,   i18n("Automatic"));
    d->whiteBalanceComboBox->insertItem(RawDecodingSettings::CUSTOM, i18n("Manual"));
    d->whiteBalanceComboBox->setDefaultIndex(RawDecodingSettings::CAMERA);
    d->whiteBalanceComboBox->setWhatsThis(i18n("<p><b>White Balance</b><p>"
                                          "Configure the raw white balance :<p>"
                                          "<b>Default D65</b>: Use a standard daylight D65 white balance (dcraw defaults)<p>"
                                          "<b>Camera</b>: Use the white balance specified by the camera. "
                                          "If not available, reverts to default neutral white balance<p>"
                                          "<b>Automatic</b>: Calculates an automatic white balance "
                                          "averaging the entire image<p>"
                                          "<b>Manual</b>: Set a custom temperature and green level values"));

    d->customWhiteBalanceSpinBox = new RIntNumInput(d->whiteBalanceSettings);
    d->customWhiteBalanceSpinBox->setRange(2000, 12000, 10);
    d->customWhiteBalanceSpinBox->setDefaultValue(6500);
    d->customWhiteBalanceSpinBox->setSliderEnabled(true);
    d->customWhiteBalanceLabel   = new QLabel(i18n("T(K):"), d->whiteBalanceSettings);
    d->customWhiteBalanceSpinBox->setWhatsThis( i18n("<p><b>Temperature</b><p>"
                                                     "Set here the color temperature in Kelvin."));

    d->customWhiteBalanceGreenSpinBox = new RDoubleNumInput(d->whiteBalanceSettings);
    d->customWhiteBalanceGreenSpinBox->setDecimals(2);
    d->customWhiteBalanceGreenSpinBox->setRange(0.2, 2.5, 0.01);
    d->customWhiteBalanceGreenSpinBox->setDefaultValue(1.0);
    d->customWhiteBalanceGreenLabel   = new QLabel(i18n("Green:"), d->whiteBalanceSettings);
    d->customWhiteBalanceGreenSpinBox->setWhatsThis(i18n("<p>Set here the green component to set magenta color "
                                                         "cast removal level."));

    d->unclipColorLabel    = new QLabel(i18n("Highlights:"), d->whiteBalanceSettings);
    d->unclipColorComboBox = new RComboBox(d->whiteBalanceSettings);
    d->unclipColorComboBox->insertItem(0, i18n("Solid white"));
    d->unclipColorComboBox->insertItem(1, i18n("Unclip"));
    d->unclipColorComboBox->insertItem(2, i18n("Blend")); 
    d->unclipColorComboBox->insertItem(3, i18n("Rebuild"));
    d->unclipColorComboBox->setDefaultIndex(0);
    d->unclipColorComboBox->setWhatsThis(i18n("<p><b>Highlights</b><p>"
                                              "Select here the highlight clipping method:<p>"
                                              "<b>Solid white</b>: clip all highlights to solid white<p>"
                                              "<b>Unclip</b>: leave highlights unclipped in various "
                                              "shades of pink<p>"
                                              "<b>Blend</b>:Blend clipped and unclipped values together "
                                              "for a gradual fade to white<p>"
                                              "<b>Rebuild</b>: reconstruct highlights using a "
                                              "level value"));

    d->reconstructLabel   = new QLabel(i18n("Level:"), d->whiteBalanceSettings);
    d->reconstructSpinBox = new RIntNumInput(d->whiteBalanceSettings);
    d->reconstructSpinBox->setRange(0, 6, 1);
    d->reconstructSpinBox->setDefaultValue(0);
    d->reconstructSpinBox->setSliderEnabled(true);
    d->reconstructSpinBox->setWhatsThis(i18n("<p><b>Level</b><p>"
                                             "Specify the reconstruct highlight level. "
                                             "Low values favor whites and high values favor colors."));

    d->brightnessLabel   = new QLabel(i18n("Brightness:"), d->whiteBalanceSettings);
    d->brightnessSpinBox = new RDoubleNumInput(d->whiteBalanceSettings);
    d->brightnessSpinBox->setDecimals(2);
    d->brightnessSpinBox->setRange(0.0, 10.0, 0.01);
    d->brightnessSpinBox->setDefaultValue(1.0);
    d->brightnessSpinBox->setWhatsThis(i18n("<p><b>Brighness</b><p>"
                                            "Specify the brightness level of output image."
                                            "The default value is 1.0 (works in 8-bit mode only).<p>"));

    if (!postProcessingOptions)
    {
        d->brightnessLabel->hide();
        d->brightnessSpinBox->hide();
    }

    d->blackPointCheckBox = new QCheckBox(i18n("Black:"), d->whiteBalanceSettings);
    d->blackPointCheckBox->setWhatsThis(i18n("<p><b>Black point</b><p>"
                                             "Use a specific black point value to decode RAW pictures. "
                                             "If you set this option to off, the Black Point value will be "
                                             "automatically computed.<p>"));
    d->blackPointSpinBox = new RIntNumInput(d->whiteBalanceSettings);
    d->blackPointSpinBox->setRange(0, 1000, 1);
    d->blackPointSpinBox->setDefaultValue(0);
    d->blackPointSpinBox->setSliderEnabled(true);
    d->blackPointSpinBox->setWhatsThis(i18n("<p><b>Black point value</b><p>"
                                            "Specify specific black point value of the output image.<p>"));

    d->whitePointCheckBox = new QCheckBox(i18n("White:"), d->whiteBalanceSettings);
    d->whitePointCheckBox->setWhatsThis(i18n("<p><b>White point</b><p>"
                                             "Use a specific white point value to decode RAW pictures. "
                                             "If you set this option to off, the White Point value will be "
                                             "automatically computed.<p>"));
    d->whitePointSpinBox = new RIntNumInput(d->whiteBalanceSettings);
    d->whitePointSpinBox->setRange(0, 20000, 1);
    d->whitePointSpinBox->setDefaultValue(0);
    d->whitePointSpinBox->setSliderEnabled(true);
    d->whitePointSpinBox->setWhatsThis(i18n("<p><b>White point value</b><p>"
                                            "Specify specific white point value of the output image.<p>"));

    whiteBalanceLayout->addWidget(d->whiteBalanceLabel,              0, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->whiteBalanceComboBox,           0, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->customWhiteBalanceLabel,        1, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->customWhiteBalanceSpinBox,      1, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->customWhiteBalanceGreenLabel,   2, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->customWhiteBalanceGreenSpinBox, 2, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->unclipColorLabel,               3, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->unclipColorComboBox,            3, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->reconstructLabel,               4, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->reconstructSpinBox,             4, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->brightnessLabel,                5, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->brightnessSpinBox,              5, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->blackPointCheckBox,             6, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->blackPointSpinBox,              6, 1, 1, 2);
    whiteBalanceLayout->addWidget(d->whitePointCheckBox,             7, 0, 1, 1);
    whiteBalanceLayout->addWidget(d->whitePointSpinBox,              7, 1, 1, 2);
    whiteBalanceLayout->setSpacing(KDialog::spacingHint());
    whiteBalanceLayout->setMargin(KDialog::spacingHint());

    insertItem(WHITEBALANCE, d->whiteBalanceSettings, i18n("White Balance"));

    // ---------------------------------------------------------------
    // CORRECTIONS Settings panel

    d->correctionsSettings         = new QWidget(this);
    QGridLayout* correctionsLayout = new QGridLayout(d->correctionsSettings);

    d->enableNoiseReduction = new QCheckBox(i18n("Enable noise reduction"), d->correctionsSettings);
    d->enableNoiseReduction->setWhatsThis(i18n("<p><b>Enable Noise Reduction</b><p>"
                     "Use wavelets to erase noise while preserving real detail.<p>"));

    d->NRThresholdSpinBox = new RIntNumInput(d->correctionsSettings);
    d->NRThresholdSpinBox->setRange(10, 1000, 1);
    d->NRThresholdSpinBox->setDefaultValue(100);
    d->NRThresholdSpinBox->setSliderEnabled(true);
    d->NRThresholdLabel   = new QLabel(i18n("Threshold:"), d->correctionsSettings);
    d->NRThresholdSpinBox->setWhatsThis(i18n("<p><b>Threshold</b><p>"
                     "Set here the noise reduction threshold value to use."));

    d->enableCACorrection = new QCheckBox(i18n("Enable Chromatic Aberration correction"), d->correctionsSettings);
    d->enableCACorrection->setWhatsThis(i18n("<p><b>Enable Chromatic Aberration correction</b><p>"
                     "Enlarge the raw red and blue layers by the given factors, "
                     "typically 0.999 to 1.001, to correct chromatic aberration.<p>"));

    d->caRedMultLabel   = new QLabel(i18n("Red:"), d->correctionsSettings);
    d->caRedMultSpinBox = new RDoubleNumInput(d->correctionsSettings);
    d->caRedMultSpinBox->setDecimals(5);
    d->caRedMultSpinBox->setRange(0.00001, 2.0, 0.001);
    d->caRedMultSpinBox->setDefaultValue(1.0);
    d->caRedMultSpinBox->setWhatsThis(i18n("<p><b>Red multiplier</b><p>"
                         "Set here the magnification factor of the red layer"));

    d->caBlueMultLabel   = new QLabel(i18n("Blue:"), d->correctionsSettings);
    d->caBlueMultSpinBox = new RDoubleNumInput(d->correctionsSettings);
    d->caBlueMultSpinBox->setDecimals(5);
    d->caBlueMultSpinBox->setRange(0.00001, 2.0, 0.001);
    d->caBlueMultSpinBox->setDefaultValue(1.0);
    d->caBlueMultSpinBox->setWhatsThis(i18n("<p><b>Blue multiplier</b><p>"
                          "Set here the magnification factor of the blue layer"));

    correctionsLayout->addWidget(d->enableNoiseReduction, 0, 0, 1, 3);
    correctionsLayout->addWidget(d->NRThresholdLabel,     1, 0, 1, 1);
    correctionsLayout->addWidget(d->NRThresholdSpinBox,   1, 1, 1, 2);
    correctionsLayout->addWidget(d->enableCACorrection,   2, 0, 1, 3);
    correctionsLayout->addWidget(d->caRedMultLabel,       3, 0, 1, 1);
    correctionsLayout->addWidget(d->caRedMultSpinBox,     3, 1, 1, 2);
    correctionsLayout->addWidget(d->caBlueMultLabel,      4, 0, 1, 1);
    correctionsLayout->addWidget(d->caBlueMultSpinBox,    4, 1, 1, 2);
    correctionsLayout->setRowStretch(5, 10);
    correctionsLayout->setSpacing(KDialog::spacingHint());
    correctionsLayout->setMargin(KDialog::spacingHint());

    insertItem(CORRECTIONS, d->correctionsSettings, i18n("Corrections"));

    // ---------------------------------------------------------------
    // COLOR MANAGEMENT Settings panel

    d->colormanSettings         = new QWidget(this);
    QGridLayout* colormanLayout = new QGridLayout(d->colormanSettings);

    d->inputColorSpaceLabel    = new QLabel(i18n("Camera Profile:"), d->colormanSettings);
    d->inputColorSpaceComboBox = new RComboBox(d->colormanSettings);
    d->inputColorSpaceComboBox->insertItem(RawDecodingSettings::NOINPUTCS,     i18n("None"));
    d->inputColorSpaceComboBox->insertItem(RawDecodingSettings::EMBEDDED,      i18n("Embedded"));
    d->inputColorSpaceComboBox->insertItem(RawDecodingSettings::CUSTOMINPUTCS, i18n("Custom"));
    d->inputColorSpaceComboBox->setDefaultIndex(RawDecodingSettings::NOINPUTCS);
    d->inputColorSpaceComboBox->setWhatsThis(i18n("<p><b>Camera Profile</b><p>"
                "Select here the input color space used to decode RAW data.<p>"
                "<b>None</b>: no input color profile is used during RAW decoding.<p>"
                "<b>Embedded</b>: use embedded color profile from RAW file if exist.<p>"
                "<b>Custom</b>: use a custom input color space profile."));

    d->inIccUrlEdit = new KUrlRequester(d->colormanSettings);
    d->inIccUrlEdit->setMode(KFile::LocalOnly | KFile::ExistingOnly | KFile::File);
    d->inIccUrlEdit->setFilter("*.icc *.icm|"+i18n("ICC Files (*.icc; *.icm)"));

    d->outputColorSpaceLabel    = new QLabel(i18n("Workspace:"), d->colormanSettings);
    d->outputColorSpaceComboBox = new RComboBox( d->colormanSettings );
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::RAWCOLOR,       i18n("Raw (linear)"));
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::SRGB,           i18n("sRGB"));
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::ADOBERGB,       i18n("Adobe RGB"));
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::WIDEGAMMUT,     i18n("Wide Gamut"));
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::PROPHOTO,       i18n("Pro-Photo"));
    d->outputColorSpaceComboBox->insertItem(RawDecodingSettings::CUSTOMOUTPUTCS, i18n("Custom"));
    d->outputColorSpaceComboBox->setDefaultIndex(RawDecodingSettings::SRGB);
    d->outputColorSpaceComboBox->setWhatsThis(i18n("<p><b>Workspace</b><p>"
                "Select here the output color space used to decode RAW data.<p>"
                "<b>Raw (linear)</b>: in this mode, no output color space is used "
                "during RAW decoding.<p>"
                "<b>sRGB</b>: this is a RGB color space, created "
                "cooperatively by Hewlett-Packard and Microsoft. It is the "
                "best choice for images destined for the Web and portrait photography.<p>"
                "<b>Adobe RGB</b>: this color space is an extended RGB color space, developed by "
                "Adobe. It is used for photography applications such as advertising "
                "and fine art.<p>"
                "<b>Wide Gamut</b>: this color space is an expanded version of the "
                "Adobe RGB color space.<p>"
                "<b>Pro-Photo</b>: this color space is an RGB color space, developed by "
                "Kodak, that offers an especially large gamut designed for use with "
                "photographic outputs in mind.<p>"
                "<b>Custom</b>: use a custom output color space profile."));

    d->outIccUrlEdit = new KUrlRequester(d->colormanSettings);
    d->outIccUrlEdit->setMode(KFile::LocalOnly | KFile::File | KFile::ExistingOnly);
    d->outIccUrlEdit->setFilter("*.icc *.icm|"+i18n("ICC Files (*.icc; *.icm)"));

    colormanLayout->addWidget(d->inputColorSpaceLabel,     0, 0, 1, 1);
    colormanLayout->addWidget(d->inputColorSpaceComboBox,  0, 1, 1, 2);
    colormanLayout->addWidget(d->inIccUrlEdit,             1, 0, 1, 3);
    colormanLayout->addWidget(d->outputColorSpaceLabel,    2, 0, 1, 1);
    colormanLayout->addWidget(d->outputColorSpaceComboBox, 2, 1, 1, 2);
    colormanLayout->addWidget(d->outIccUrlEdit,            3, 0, 1, 3);
    colormanLayout->setRowStretch(4, 10);
    colormanLayout->setSpacing(KDialog::spacingHint());
    colormanLayout->setMargin(KDialog::spacingHint());

    insertItem(COLORMANAGEMENT, d->colormanSettings, i18n("Color Management"));

    if (!outputColorSpaceOption)
    {
        removeItem(indexOf(d->colormanSettings));
        d->colormanSettings->hide();
        d->inputColorSpaceLabel->hide();
        d->inputColorSpaceComboBox->hide();
        d->inIccUrlEdit->hide();
        d->outputColorSpaceLabel->hide();
        d->outputColorSpaceComboBox->hide();
        d->outIccUrlEdit->hide();
    }

    // ---------------------------------------------------------------

    connect(d->unclipColorComboBox, SIGNAL(activated(int)),
            this, SLOT(slotUnclipColorActivated(int)));

    connect(d->whiteBalanceComboBox, SIGNAL(activated(int)),
            this, SLOT(slotWhiteBalanceToggled(int)));

    connect(d->enableNoiseReduction, SIGNAL(toggled(bool)),
            this, SLOT(slotNoiseReductionToggled(bool)));

    connect(d->enableCACorrection, SIGNAL(toggled(bool)),
            this, SLOT(slotCACorrectionToggled(bool)));

    connect(d->blackPointCheckBox, SIGNAL(toggled(bool)),
            d->blackPointSpinBox, SLOT(setEnabled(bool)));

    connect(d->whitePointCheckBox, SIGNAL(toggled(bool)),
            d->whitePointSpinBox, SLOT(setEnabled(bool)));

    connect(d->sixteenBitsImage, SIGNAL(toggled(bool)),
            this, SLOT(slotsixteenBitsImageToggled(bool)));

    connect(dcrawVersion, SIGNAL(leftClickedUrl(const QString&)),
            this, SLOT(processDcrawUrl(const QString&)));

    connect(d->inputColorSpaceComboBox, SIGNAL(activated(int)),
            this, SLOT(slotInputColorSpaceChanged(int)));

    connect(d->outputColorSpaceComboBox, SIGNAL(activated(int)),
            this, SLOT(slotOutputColorSpaceChanged(int)));

    // Wrapper to emit signal when something is changed in settings.

    connect(d->inIccUrlEdit, SIGNAL(urlSelected(const KUrl&)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->outIccUrlEdit, SIGNAL(urlSelected(const KUrl&)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->whiteBalanceComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->RAWQualityComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->unclipColorComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->inputColorSpaceComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->outputColorSpaceComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->blackPointCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->whitePointCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->sixteenBitsImage, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->fourColorCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->dontStretchPixelsCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->enableNoiseReduction, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->enableCACorrection, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->customWhiteBalanceSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->reconstructSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->blackPointSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->whitePointSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->NRThresholdSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->medianFilterPassesSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->customWhiteBalanceGreenSpinBox, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->caRedMultSpinBox, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->caBlueMultSpinBox, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->brightnessSpinBox, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalSettingsChanged()));
}

DcrawSettingsWidget::~DcrawSettingsWidget()
{
    delete d;
}

void DcrawSettingsWidget::updateMinimumWidth()
{
    int width = 0;
    for (int i = 0; i < count(); i++)
        if (widget(i)->width() > width)
            width = widget(i)->width();

    setMinimumWidth(width);
}

void DcrawSettingsWidget::processDcrawUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

KUrlRequester* DcrawSettingsWidget::inputProfileUrlEdit() const
{
    return d->inIccUrlEdit;
}

KUrlRequester* DcrawSettingsWidget::outputProfileUrlEdit() const
{
    return d->outIccUrlEdit;
}

void DcrawSettingsWidget::setDefaultSettings()
{
    setWhiteBalance((RawDecodingSettings::WhiteBalance)d->whiteBalanceComboBox->defaultIndex());
    setCustomWhiteBalance(d->customWhiteBalanceSpinBox->defaultValue());
    setCustomWhiteBalanceGreen(d->customWhiteBalanceGreenSpinBox->defaultValue());
    setFourColor(false);
    setUnclipColor(d->unclipColorComboBox->defaultIndex());
    setDontStretchPixels(false);
    setNoiseReduction(false);
    setUseCACorrection(false);
    setcaRedMultiplier(d->caRedMultSpinBox->defaultValue());
    setcaBlueMultiplier(d->caBlueMultSpinBox->defaultValue());
    setBrightness(d->brightnessSpinBox->defaultValue());
    setUseBlackPoint(false);
    setUseWhitePoint(false);
    setBlackPoint(d->blackPointSpinBox->defaultValue());
    setWhitePoint(d->whitePointSpinBox->defaultValue());
    setNRThreshold(d->NRThresholdSpinBox->defaultValue());
    setQuality((RawDecodingSettings::DecodingQuality)d->RAWQualityComboBox->defaultIndex());
    setInputColorSpace((RawDecodingSettings::InputColorSpace)d->inputColorSpaceComboBox->defaultIndex());
    setOutputColorSpace((RawDecodingSettings::OutputColorSpace)d->outputColorSpaceComboBox->defaultIndex());
    setMedianFilterPasses(d->medianFilterPassesSpinBox->defaultValue());
}

void DcrawSettingsWidget::slotsixteenBitsImageToggled(bool b)
{
    setEnabledBrightnessSettings(!b);
    emit signalSixteenBitsImageToggled(d->sixteenBitsImage->isChecked());
}

void DcrawSettingsWidget::slotWhiteBalanceToggled(int v)
{
    if (v == 3)
    {
        d->customWhiteBalanceSpinBox->setEnabled(true);
        d->customWhiteBalanceGreenSpinBox->setEnabled(true);
        d->customWhiteBalanceLabel->setEnabled(true);
        d->customWhiteBalanceGreenLabel->setEnabled(true);
    }
    else
    {
        d->customWhiteBalanceSpinBox->setEnabled(false);
        d->customWhiteBalanceGreenSpinBox->setEnabled(false);
        d->customWhiteBalanceLabel->setEnabled(false);
        d->customWhiteBalanceGreenLabel->setEnabled(false);
    }
}

void DcrawSettingsWidget::slotUnclipColorActivated(int v)
{
    if (v == 3)     // Reconstruct Highlight method
    {
        d->reconstructLabel->setEnabled(true);
        d->reconstructSpinBox->setEnabled(true);
    }
    else
    {
        d->reconstructLabel->setEnabled(false);
        d->reconstructSpinBox->setEnabled(false);
    }
}

void DcrawSettingsWidget::slotNoiseReductionToggled(bool b)
{
    d->NRThresholdSpinBox->setEnabled(b);
    d->NRThresholdLabel->setEnabled(b);
}

void DcrawSettingsWidget::slotCACorrectionToggled(bool b)
{
    d->caRedMultSpinBox->setEnabled(b);
    d->caBlueMultSpinBox->setEnabled(b);
    d->caRedMultLabel->setEnabled(b);
    d->caBlueMultLabel->setEnabled(b);
}

void DcrawSettingsWidget::slotInputColorSpaceChanged(int item)
{
    d->inIccUrlEdit->setEnabled(item == RawDecodingSettings::CUSTOMINPUTCS);
}

void DcrawSettingsWidget::slotOutputColorSpaceChanged(int item)
{
    d->outIccUrlEdit->setEnabled(item == RawDecodingSettings::CUSTOMOUTPUTCS);
}

// ---------------------------------------------------------------

void DcrawSettingsWidget::setEnabledBrightnessSettings(bool b)
{
    d->brightnessLabel->setEnabled(b);
    d->brightnessSpinBox->setEnabled(b);
}

bool DcrawSettingsWidget::brightnessSettingsIsEnabled()
{
    return d->brightnessSpinBox->isEnabled();
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::sixteenBits()
{
    return d->sixteenBitsImage->isChecked();
}

void DcrawSettingsWidget::setSixteenBits(bool b)
{
    d->sixteenBitsImage->setChecked(b);
}

// ---------------------------------------------------------------

RawDecodingSettings::WhiteBalance DcrawSettingsWidget::whiteBalance()
{
    switch(d->whiteBalanceComboBox->currentIndex())
    {
        case 1:
            return RawDecodingSettings::CAMERA;
            break;
        case 2:
            return RawDecodingSettings::AUTO;
            break;
        case 3:
            return RawDecodingSettings::CUSTOM;
            break;
        default:
            return RawDecodingSettings::NONE;
            break;
    }
}

void DcrawSettingsWidget::setWhiteBalance(RawDecodingSettings::WhiteBalance v)
{
    switch(v)
    {
        case RawDecodingSettings::CAMERA:
            d->whiteBalanceComboBox->setCurrentIndex(1);
            break;
        case RawDecodingSettings::AUTO:
            d->whiteBalanceComboBox->setCurrentIndex(2);
            break;
        case RawDecodingSettings::CUSTOM:
            d->whiteBalanceComboBox->setCurrentIndex(3);
            break;
        default:
            d->whiteBalanceComboBox->setCurrentIndex(0);
            break;
    }
    slotWhiteBalanceToggled(d->whiteBalanceComboBox->currentIndex());
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::medianFilterPasses()
{
    return d->medianFilterPassesSpinBox->value();
}

void DcrawSettingsWidget::setMedianFilterPasses(int p)
{
    d->medianFilterPassesSpinBox->setValue(p);
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::customWhiteBalance()
{
    return d->customWhiteBalanceSpinBox->value();
}

void DcrawSettingsWidget::setCustomWhiteBalance(int v)
{
    d->customWhiteBalanceSpinBox->setValue(v);
}

// ---------------------------------------------------------------

double DcrawSettingsWidget::customWhiteBalanceGreen()
{
    return d->customWhiteBalanceGreenSpinBox->value();
}

void DcrawSettingsWidget::setCustomWhiteBalanceGreen(double v)
{
    d->customWhiteBalanceGreenSpinBox->setValue(v);
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useFourColor()
{
    return d->fourColorCheckBox->isChecked();
}

void DcrawSettingsWidget::setFourColor(bool b)
{
    d->fourColorCheckBox->setChecked(b);
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::unclipColor()
{
    switch(d->unclipColorComboBox->currentIndex())
    {
        case 0:
            return 0;
            break;
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        default:         // Reconstruct Highlight method
            return d->reconstructSpinBox->value()+3;
            break;
    }
}

void DcrawSettingsWidget::setUnclipColor(int v)
{
    switch(v)
    {
        case 0:
            d->unclipColorComboBox->setCurrentIndex(0);
            break;
        case 1:
            d->unclipColorComboBox->setCurrentIndex(1);
            break;
        case 2:
            d->unclipColorComboBox->setCurrentIndex(2);
            break;
        default:         // Reconstruct Highlight method
            d->unclipColorComboBox->setCurrentIndex(3);
            d->reconstructSpinBox->setValue(v-3);
            break;
    }

    slotUnclipColorActivated(d->unclipColorComboBox->currentIndex());
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useDontStretchPixels()
{
    return d->dontStretchPixelsCheckBox->isChecked();
}

void DcrawSettingsWidget::setDontStretchPixels(bool b)
{
    d->dontStretchPixelsCheckBox->setChecked(b);
}

// ---------------------------------------------------------------

double DcrawSettingsWidget::brightness()
{
    return d->brightnessSpinBox->value();
}

void DcrawSettingsWidget::setBrightness(double b)
{
    d->brightnessSpinBox->setValue(b);
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useBlackPoint()
{
    return d->blackPointCheckBox->isChecked();
}

void DcrawSettingsWidget::setUseBlackPoint(bool b)
{
    d->blackPointCheckBox->setChecked(b);
    d->blackPointSpinBox->setEnabled(b);
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::blackPoint()
{
    return d->blackPointSpinBox->value();
}

void DcrawSettingsWidget::setBlackPoint(int b)
{
    d->blackPointSpinBox->setValue(b);
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useWhitePoint()
{
    return d->whitePointCheckBox->isChecked();
}

void DcrawSettingsWidget::setUseWhitePoint(bool b)
{
    d->whitePointCheckBox->setChecked(b);
    d->whitePointSpinBox->setEnabled(b);
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::whitePoint()
{
    return d->whitePointSpinBox->value();
}

void DcrawSettingsWidget::setWhitePoint(int b)
{
    d->whitePointSpinBox->setValue(b);
}

// ---------------------------------------------------------------

RawDecodingSettings::DecodingQuality DcrawSettingsWidget::quality()
{
    switch(d->RAWQualityComboBox->currentIndex())
    {
        case 1:
            return RawDecodingSettings::VNG;
            break;
        case 2:
            return RawDecodingSettings::PPG;
            break;
        case 3:
            return RawDecodingSettings::AHD;
            break;
        default:
            return RawDecodingSettings::BILINEAR;
            break;
    }
}

void DcrawSettingsWidget::setQuality(RawDecodingSettings::DecodingQuality q)
{
    switch(q)
    {
        case RawDecodingSettings::VNG:
            d->RAWQualityComboBox->setCurrentIndex(1);
            break;
        case RawDecodingSettings::PPG:
            d->RAWQualityComboBox->setCurrentIndex(2);
            break;
        case RawDecodingSettings::AHD:
            d->RAWQualityComboBox->setCurrentIndex(3);
            break;
        default:
            d->RAWQualityComboBox->setCurrentIndex(0);
            break;
    }
}

// ---------------------------------------------------------------

RawDecodingSettings::InputColorSpace DcrawSettingsWidget::inputColorSpace()
{
    return (RawDecodingSettings::InputColorSpace)(d->inputColorSpaceComboBox->currentIndex());
}

void DcrawSettingsWidget::setInputColorSpace(RawDecodingSettings::InputColorSpace c)
{
    d->inputColorSpaceComboBox->setCurrentIndex((int)c);
    slotInputColorSpaceChanged((int)c);
}

// ---------------------------------------------------------------

RawDecodingSettings::OutputColorSpace DcrawSettingsWidget::outputColorSpace()
{
    return (RawDecodingSettings::OutputColorSpace)(d->outputColorSpaceComboBox->currentIndex());
}

void DcrawSettingsWidget::setOutputColorSpace(RawDecodingSettings::OutputColorSpace c)
{
    d->outputColorSpaceComboBox->setCurrentIndex((int)c);
    slotOutputColorSpaceChanged((int)c);
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useNoiseReduction()
{
    return d->enableNoiseReduction->isChecked();
}

void DcrawSettingsWidget::setNoiseReduction(bool b)
{
    d->enableNoiseReduction->setChecked(b);
    slotNoiseReductionToggled(b);
}

// ---------------------------------------------------------------

int DcrawSettingsWidget::NRThreshold()
{
    return d->NRThresholdSpinBox->value();
}

void DcrawSettingsWidget::setNRThreshold(int b)
{
    d->NRThresholdSpinBox->setValue(b);
}

// ---------------------------------------------------------------

bool DcrawSettingsWidget::useCACorrection()
{
    return d->enableCACorrection->isChecked();
}

void DcrawSettingsWidget::setUseCACorrection(bool b)
{
    d->enableCACorrection->setChecked(b);
    slotCACorrectionToggled(b);
}

// ---------------------------------------------------------------

double DcrawSettingsWidget::caRedMultiplier()
{
    return d->caRedMultSpinBox->value();
}

void DcrawSettingsWidget::setcaRedMultiplier(double b)
{
    d->caRedMultSpinBox->setValue(b);
}

// ---------------------------------------------------------------

double DcrawSettingsWidget::caBlueMultiplier()
{
    return d->caBlueMultSpinBox->value();
}

void DcrawSettingsWidget::setcaBlueMultiplier(double b)
{
    d->caBlueMultSpinBox->setValue(b);
}

// ---------------------------------------------------------------

QString DcrawSettingsWidget::inputColorProfile()
{
    return d->inIccUrlEdit->url().path();
}

void DcrawSettingsWidget::setInputColorProfile(const QString& path)
{
    d->inIccUrlEdit->setUrl(KUrl(path));
}

// ---------------------------------------------------------------

QString DcrawSettingsWidget::outputColorProfile()
{
    return d->outIccUrlEdit->url().path();
}

void DcrawSettingsWidget::setOutputColorProfile(const QString& path)
{
    d->outIccUrlEdit->setUrl(KUrl(path));
}

} // NameSpace KDcrawIface
