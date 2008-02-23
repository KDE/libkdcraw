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
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QWhatsThis>
#include <QToolTip>
#include <QTabBar>

// KDE includes.

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <knuminput.h>
#include <kurllabel.h>
#include <ktoolinvocation.h>

// Local includes.

#include "dcrawbinary.h"
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
        stdSettings                    = 0;
        advSettings                    = 0;
    }

    QWidget         *stdSettings;
    QWidget         *advSettings;

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
    QLabel          *outputColorSpaceLabel;

    QComboBox       *whiteBalanceComboBox;
    QComboBox       *RAWQualityComboBox;
    QComboBox       *unclipColorComboBox;
    QComboBox       *outputColorSpaceComboBox;

    QCheckBox       *blackPointCheckBox;
    QCheckBox       *sixteenBitsImage;
    QCheckBox       *fourColorCheckBox;
    QCheckBox       *customWhiteBalanceCheckBox;
    QCheckBox       *dontStretchPixelsCheckBox;
    QCheckBox       *enableNoiseReduction;
    QCheckBox       *enableCACorrection;

    KIntNumInput    *customWhiteBalanceSpinBox;
    KIntNumInput    *reconstructSpinBox;
    KIntNumInput    *blackPointSpinBox;
    KIntNumInput    *NRThresholdSpinBox;

    KDoubleNumInput *customWhiteBalanceGreenSpinBox;
    KDoubleNumInput *caRedMultSpinBox;
    KDoubleNumInput *caBlueMultSpinBox;
    KDoubleNumInput *brightnessSpinBox;
};

DcrawSettingsWidget::DcrawSettingsWidget(QWidget *parent, bool sixteenBitsOption, 
                                         bool outputColorSpaceOption, bool showAdvancedOptions)
                   : KTabWidget(parent)
{
    d = new DcrawSettingsWidgetPriv;

    d->stdSettings                 = new QWidget();
    QGridLayout* settingsBoxLayout = new QGridLayout(d->stdSettings);
    settingsBoxLayout->setMargin(KDialog::spacingHint());
    settingsBoxLayout->setSpacing(KDialog::spacingHint());

    // ---------------------------------------------------------------

    int line = 0;

    d->sixteenBitsImage = new QCheckBox(i18n("16 bits color depth"), d->stdSettings);
    d->sixteenBitsImage->setWhatsThis(i18n("<p>If enabled, all RAW files will be decoded in 16-bit "
                                           "color depth using a linear gamma curve. To prevent dark "
                                           "picture rendering in the editor, it is recommended to use "
                                           "Color Management in this mode.<p>"
                                           "If disabled, all RAW files will be decoded in 8-bit "
                                           "color depth with a BT.709 gamma curve and a 99th-percentile "
                                           "white point. This mode is faster than 16-bit decoding."));
    settingsBoxLayout->addWidget(d->sixteenBitsImage, 0, 0, 1, 1);

    if (sixteenBitsOption)
    {
        d->sixteenBitsImage->show();
        line = 1;
    }
    else
    {
        d->sixteenBitsImage->hide();
    }

    // ---------------------------------------------------------------

    d->fourColorCheckBox = new QCheckBox(i18n("Interpolate RGB as four colors"), d->stdSettings);
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
    settingsBoxLayout->addWidget(d->fourColorCheckBox, line, 0, 1, 1);
    line++;

    // ---------------------------------------------------------------

    KUrlLabel *dcrawVersion = new KUrlLabel("http://cybercom.net/~dcoffin/dcraw", QString("dcraw %1")
                                  .arg(DcrawBinary::internalVersion()), d->stdSettings);
    dcrawVersion->setAlignment(Qt::AlignRight);
    dcrawVersion->setToolTip(i18n("Visit dcraw project website"));
    settingsBoxLayout->addWidget(dcrawVersion, 0, 2, 1, 1);

    // ---------------------------------------------------------------

    d->whiteBalanceLabel    = new QLabel(i18n("White Balance:"), d->stdSettings);
    d->whiteBalanceComboBox = new QComboBox(d->stdSettings);
    d->whiteBalanceComboBox->insertItem(0, i18n("Default D65 White Balance"));
    d->whiteBalanceComboBox->insertItem(1, i18n("Camera White Walance"));
    d->whiteBalanceComboBox->insertItem(2, i18n("Automatic White Balance"));
    d->whiteBalanceComboBox->insertItem(3, i18n("Manual White balance"));
    d->whiteBalanceComboBox->setWhatsThis( i18n("<p><b>White Balance</b><p>"
                                             "Configure the raw white balance :<p>"
                                             "<b>Default D65 White Balance</b>: Use a standard daylight D65 white balance (dcraw defaults)<p>"
                                             "<b>Camera White Walance</b>: Use the white balance specified by the camera"
                                             "If not available, reverts to default neutral white balance<p>"
                                             "<b>Automatic White Balance</b>: Calculates an automatic white balance"
                                             "averaging the entire image<p>"
                                             "<b>Manual White balance</b>: Set a custom temperature and green level values"));
    settingsBoxLayout->addWidget(d->whiteBalanceLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->whiteBalanceComboBox, line, 1, 1, 2);
    line++;

    d->customWhiteBalanceSpinBox = new KIntNumInput(d->stdSettings);
    d->customWhiteBalanceSpinBox->setRange(2000, 12000, 10);
    d->customWhiteBalanceSpinBox->setSliderEnabled(true);
    d->customWhiteBalanceLabel   = new QLabel(i18n("Temperature (K):"), d->stdSettings);
    d->customWhiteBalanceSpinBox->setWhatsThis( i18n("<p><b>Temperature</b><p>"
                                                     "Set here the color temperature."));
    settingsBoxLayout->addWidget(d->customWhiteBalanceLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->customWhiteBalanceSpinBox, line, 1, 1, 2);
    line++;

    d->customWhiteBalanceGreenSpinBox = new KDoubleNumInput(d->stdSettings);
    d->customWhiteBalanceGreenSpinBox->setDecimals(2);
    d->customWhiteBalanceGreenSpinBox->setRange(0.2, 2.5, 0.01, true);
    d->customWhiteBalanceGreenLabel   = new QLabel(i18n("Green:"), d->stdSettings);
    d->customWhiteBalanceGreenSpinBox->setWhatsThis( i18n("<p>Set here the green component to set magenta color "
                                                          "cast removal level."));
    settingsBoxLayout->addWidget(d->customWhiteBalanceGreenLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->customWhiteBalanceGreenSpinBox, line, 1, 1, 2);
    line++;
 
    // ---------------------------------------------------------------

    d->unclipColorLabel    = new QLabel(i18n("Highlights:"), d->stdSettings);
    d->unclipColorComboBox = new QComboBox(d->stdSettings);
    d->unclipColorComboBox->insertItem(0, i18n("Solid white"));
    d->unclipColorComboBox->insertItem(1, i18n("Unclip"));
    d->unclipColorComboBox->insertItem(2, i18n("Blend")); 
    d->unclipColorComboBox->insertItem(3, i18n("Rebuild"));
    d->unclipColorComboBox->setWhatsThis(i18n("<p><b>Highlights</b><p>"
                                              "Select here the highlight clipping method:<p>"
                                              "<b>Solid white</b>: clip all highlights to solid white<p>"
                                              "<b>Unclip</b>: leave highlights unclipped in various "
                                              "shades of pink<p>"
                                              "<b>Blend</b>:Blend clipped and unclipped values together "
                                              "for a gradual fade to white<p>" 
                                              "<b>Rebuild</b>: reconstruct highlights using a " 
                                              "level value")); 
    settingsBoxLayout->addWidget(d->unclipColorLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->unclipColorComboBox, line, 1, 1, 2);
    line++;

    d->reconstructLabel   = new QLabel(i18n("Level:"), d->stdSettings);
    d->reconstructSpinBox = new KIntNumInput(d->stdSettings);
    d->reconstructSpinBox->setRange(0, 6, 1);
    d->reconstructSpinBox->setSliderEnabled(true);
    d->reconstructSpinBox->setWhatsThis(i18n("<p><b>Level</b><p>"
                                             "Specify the reconstruct highlight level. "
                                             "Low values favor whites and high values favor colors."));
    settingsBoxLayout->addWidget(d->reconstructLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->reconstructSpinBox, line, 1, 1, 2);
    line++;

    // ---------------------------------------------------------------

    d->brightnessLabel   = new QLabel(i18n("Brightness:"), d->stdSettings);
    d->brightnessSpinBox = new KDoubleNumInput(d->stdSettings);
    d->brightnessSpinBox->setDecimals(2);
    d->brightnessSpinBox->setRange(0.0, 10.0, 0.01, true);
    d->brightnessSpinBox->setWhatsThis(i18n("<p><b>Brighness</b><p>"
                                            "Specify the brightness level of output image."
                                            "The default value is 1.0 (works in 8-bit mode only).<p>"));
    settingsBoxLayout->addWidget(d->brightnessLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->brightnessSpinBox, line, 1, 1, 2);
    line++;

    // ---------------------------------------------------------------

    d->RAWQualityLabel    = new QLabel(i18n("Quality (interpolation):"), d->stdSettings);
    d->RAWQualityComboBox = new QComboBox(d->stdSettings);
    d->RAWQualityComboBox->insertItem(0, i18n("Bilinear"));
    d->RAWQualityComboBox->insertItem(1, i18n("VNG"));
    d->RAWQualityComboBox->insertItem(2, i18n("PPG"));
    d->RAWQualityComboBox->insertItem(3, i18n("AHD"));
    d->RAWQualityComboBox->setWhatsThis(i18n("<p><b>Quality</b><p>"
                "Select here the demosaicing RAW images decoding "
                "interpolation method. A demosaicing algorithm is a digital image process used to "
                "interpolate a complete image from the partial raw data received from the color-filtered "
                "image sensor internal to many digital cameras in form of a matrix of colored pixels. "
                "Also known as CFA interpolation or color reconstruction, another common spelling "
                "is demosaicing. There are 3 methods to demosaicing RAW images:<p>"
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
    settingsBoxLayout->addWidget(d->RAWQualityLabel, line, 0, 1, 1); 
    settingsBoxLayout->addWidget(d->RAWQualityComboBox, line, 1, 1, 2);
    line++;

    // ---------------------------------------------------------------

    d->enableNoiseReduction = new QCheckBox(i18n("Enable noise reduction"), d->stdSettings);
    d->enableNoiseReduction->setWhatsThis(i18n("<p><b>Enable Noise Reduction</b><p>"
                     "Use wavelets to erase noise while preserving real detail.<p>"));
    settingsBoxLayout->addWidget(d->enableNoiseReduction, line, 0, 1, 3 );
    line++;

    d->NRThresholdSpinBox = new KIntNumInput(d->stdSettings);
    d->NRThresholdSpinBox->setRange(10, 1000, 1);
    d->NRThresholdSpinBox->setSliderEnabled(true);
    d->NRThresholdLabel   = new QLabel(i18n("Threshold:"), d->stdSettings);
    d->NRThresholdSpinBox->setWhatsThis(i18n("<p><b>Threshold</b><p>"
                     "Set here the noise reduction threshold value to use."));
    settingsBoxLayout->addWidget(d->NRThresholdLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->NRThresholdSpinBox, line, 1, 1, 2);
    line++;

    // ---------------------------------------------------------------

    d->enableCACorrection = new QCheckBox(i18n("Enable Chromatic Aberration correction"), d->stdSettings);
    d->enableCACorrection->setWhatsThis(i18n("<p><b>Enable Chromatic Aberration correction</b><p>"
                     "Enlarge the raw red and blue layers by the given factors, "
                     "typically 0.999 to 1.001, to correct chromatic aberration.<p>"));
    settingsBoxLayout->addWidget(d->enableCACorrection, line, 0, 1, 3);
    line++;

    d->caRedMultLabel   = new QLabel(i18n("Red multiplier:"), d->stdSettings);
    d->caRedMultSpinBox = new KDoubleNumInput(d->stdSettings);
    d->caRedMultSpinBox->setDecimals(5);
    d->caRedMultSpinBox->setRange(0.00001, 2.0, 0.001, true);
    d->caRedMultSpinBox->setWhatsThis(i18n("<p><b>Red multiplier</b><p>"
                     "Set here the magnification factor of the red layer"));

    d->caBlueMultLabel   = new QLabel(i18n("Blue multiplier:"), d->stdSettings);
    d->caBlueMultSpinBox = new KDoubleNumInput(d->stdSettings);
    d->caBlueMultSpinBox->setDecimals(5);
    d->caBlueMultSpinBox->setRange(0.00001, 2.0, 0.001, true);
    d->caBlueMultSpinBox->setWhatsThis(i18n("<p><b>Blue multiplier</b><p>"
                     "Set here the magnification factor of the blue layer"));
    
    settingsBoxLayout->addWidget(d->caRedMultLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->caRedMultSpinBox, line, 1, 1, 2);
    line++;
    settingsBoxLayout->addWidget(d->caBlueMultLabel, line, 0, 1, 1);
    settingsBoxLayout->addWidget(d->caBlueMultSpinBox, line, 1, 1, 2);
    line++;

    // ---------------------------------------------------------------

    d->outputColorSpaceLabel    = new QLabel(i18n("Color space:"), d->stdSettings);
    d->outputColorSpaceComboBox = new QComboBox( d->stdSettings );
    d->outputColorSpaceComboBox->insertItem(0, i18n("Raw (linear)"));
    d->outputColorSpaceComboBox->insertItem(1, i18n("sRGB"));
    d->outputColorSpaceComboBox->insertItem(2, i18n("Adobe RGB"));
    d->outputColorSpaceComboBox->insertItem(3, i18n("Wide Gamut"));
    d->outputColorSpaceComboBox->insertItem(4, i18n("Pro-Photo"));
    d->outputColorSpaceComboBox->setWhatsThis(i18n("<p><b>Color space</b><p>"
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
                "photographic outputs in mind."));

    settingsBoxLayout->addWidget(d->outputColorSpaceLabel, line, 0, 1, 1); 
    settingsBoxLayout->addWidget(d->outputColorSpaceComboBox, line, 1, 1, 2);

    if (outputColorSpaceOption)
    {
        d->outputColorSpaceLabel->show();
        d->outputColorSpaceComboBox->show(); 
    }
    else
    {
        d->outputColorSpaceLabel->hide();
        d->outputColorSpaceComboBox->hide(); 
    }

    insertTab(0, d->stdSettings, i18n("Standard"));

    // ---------------------------------------------------------------

    d->advSettings                  = new QWidget();
    QGridLayout* settingsBoxLayout2 = new QGridLayout(d->advSettings);

    d->dontStretchPixelsCheckBox = new QCheckBox(i18n("Do not stretch or rotate pixels"), d->advSettings);
    d->dontStretchPixelsCheckBox->setWhatsThis(i18n("<p><b>Do not stretch or rotate pixels</b><p>"
                                                    "For Fuji Super CCD cameras, show the image tilted 45 "
                                                    "degrees. For cameras with non-square pixels, do not "
                                                    "stretch the image to its correct aspect ratio. In any "
                                                    "case, this option guarantees that each output pixel "
                                                    "corresponds to one RAW pixel.<p>"));


    // ---------------------------------------------------------------

    d->blackPointCheckBox = new QCheckBox(i18n("Black point"), d->advSettings);
    d->blackPointCheckBox->setWhatsThis(i18n("<p><b>Black point</b><p>"
                                             "Use a specific black point value to decode RAW pictures. "
                                             "If you set this option to off, the Black Point value will be "
                                             "automatically computed.<p>"));
    d->blackPointSpinBox = new KIntNumInput(d->advSettings);
    d->blackPointSpinBox->setRange(0, 1000, 1);
    d->blackPointSpinBox->setSliderEnabled(true);
    d->blackPointSpinBox->setWhatsThis(i18n("<p><b>Black point value</b><p>"
                                            "Specify specific black point value of the output image.<p>"));

    // ---------------------------------------------------------------

    settingsBoxLayout2->setMargin(KDialog::spacingHint());
    settingsBoxLayout2->setSpacing(KDialog::spacingHint());
    settingsBoxLayout2->addWidget(d->dontStretchPixelsCheckBox, 0, 0, 1, 3 );   
    settingsBoxLayout2->addWidget(d->blackPointCheckBox, 1, 0, 1, 1);
    settingsBoxLayout2->addWidget(d->blackPointSpinBox, 1, 1, 1, 2);
    settingsBoxLayout2->setRowStretch(7, 10);

    insertTab(1, d->advSettings, i18n("Advanced"));

    if (!showAdvancedOptions)
    {
        removeTab(1);
        setTabBarHidden(true);
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

    connect(d->sixteenBitsImage, SIGNAL(toggled(bool)),
            this, SLOT(slotsixteenBitsImageToggled(bool)));

    connect(dcrawVersion, SIGNAL(leftClickedUrl(const QString&)),
            this, SLOT(processDcrawUrl(const QString&)));
}

DcrawSettingsWidget::~DcrawSettingsWidget()
{
    delete d;
}

void DcrawSettingsWidget::processDcrawUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void DcrawSettingsWidget::setDefaultSettings()
{
    setWhiteBalance(RawDecodingSettings::CAMERA);
    setCustomWhiteBalance(6500);
    setCustomWhiteBalanceGreen(1.0);
    setFourColor(false);
    setUnclipColor(0);
    setDontStretchPixels(false);
    setNoiseReduction(false);
    setUseCACorrection(false);
    setcaRedMultiplier(1.0);
    setcaBlueMultiplier(1.0);
    setBrightness(1.0);
    setUseBlackPoint(false);
    setBlackPoint(0);
    setNRThreshold(100);
    setQuality(RawDecodingSettings::BILINEAR); 
    setOutputColorSpace(RawDecodingSettings::SRGB); 
}

void DcrawSettingsWidget::slotsixteenBitsImageToggled(bool b)
{
    d->brightnessLabel->setDisabled(b);
    d->brightnessSpinBox->setDisabled(b); 
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

RawDecodingSettings::OutputColorSpace DcrawSettingsWidget::outputColorSpace()
{
    return (RawDecodingSettings::OutputColorSpace)(d->outputColorSpaceComboBox->currentIndex());
}

void DcrawSettingsWidget::setOutputColorSpace(RawDecodingSettings::OutputColorSpace c)
{
    d->outputColorSpaceComboBox->setCurrentIndex((int)c);
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

} // NameSpace KDcrawIface
