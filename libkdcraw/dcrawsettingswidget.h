/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-13
 * Description : LibRaw settings widgets
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DCRAW_SETTINGS_WIDGET_H
#define DCRAW_SETTINGS_WIDGET_H

// Qt includes.

#include <QtCore/QString>
#include <QtGui/QToolBox>

// KDE includes.

#include <kurlrequester.h>

// Local includes.

#include "libkdcraw_export.h"
#include "rawdecodingsettings.h"

namespace KDcrawIface
{

class DcrawSettingsWidgetPriv;

class LIBKDCRAW_EXPORT DcrawSettingsWidget : public QToolBox
{
    Q_OBJECT

public: 

    enum AdvancedSettingsOptions
    {
        SIXTEENBITS      = 0x00000001,
        COLORSPACE       = 0x00000002,
        POSTPROCESSING   = 0x00000004,
        BLACKWHITEPOINTS = 0x00000008
    };

    enum SettingsTabs
    {
        DEMOSAICING = 0,
        WHITEBALANCE,
        CORRECTIONS,
        COLORMANAGEMENT
    };

public:

    /**
     * @param advSettings the default value is COLORSPACE
     */
    DcrawSettingsWidget(QWidget *parent, int advSettings);
    KDE_DEPRECATED DcrawSettingsWidget(QWidget *parent,
                        bool sixteenBitsOption=false,
                        bool outputColorSpaceOption=true,
                        bool postProcessingOptions=false);
    virtual ~DcrawSettingsWidget();

    void setup(int advSettings);

    bool   sixteenBits();
    int    customWhiteBalance();
    double customWhiteBalanceGreen();
    bool   useFourColor();
    bool   useDontStretchPixels();
    bool   useNoiseReduction();
    bool   useCACorrection();
    bool   useBlackPoint();
    bool   useWhitePoint();
    int    unclipColor();
    bool   useAutoBrightness();
    double brightness();
    int    blackPoint();
    int    whitePoint();
    int    NRThreshold();
    double caRedMultiplier();
    double caBlueMultiplier();
    int    medianFilterPasses();

    RawDecodingSettings::WhiteBalance     whiteBalance();
    RawDecodingSettings::DecodingQuality  quality();
    RawDecodingSettings::InputColorSpace  inputColorSpace();
    RawDecodingSettings::OutputColorSpace outputColorSpace();

    QString inputColorProfile();
    QString outputColorProfile();

    void   setSixteenBits(bool b);
    void   setCameraWB(bool b);
    void   setWhiteBalance(RawDecodingSettings::WhiteBalance v);
    void   setCustomWhiteBalance(int v);
    void   setCustomWhiteBalanceGreen(double v);
    void   setFourColor(bool b);
    void   setDontStretchPixels(bool b);
    void   setNoiseReduction(bool b);
    void   setUseCACorrection(bool b);
    void   setUseBlackPoint(bool b);
    void   setUseWhitePoint(bool b);
    void   setUnclipColor(int v);
    void   setAutoBrightness(bool b);
    void   setBrightness(double b);
    void   setBlackPoint(int b);
    void   setWhitePoint(int b);
    void   setNRThreshold(int b);
    void   setcaRedMultiplier(double b);
    void   setcaBlueMultiplier(double b);
    void   setMedianFilterPasses(int p);
    void   setQuality(RawDecodingSettings::DecodingQuality q);
    void   setInputColorSpace(RawDecodingSettings::InputColorSpace c);
    void   setOutputColorSpace(RawDecodingSettings::OutputColorSpace c);
    void   setInputColorProfile(const QString& path);
    void   setOutputColorProfile(const QString& path);

    void   setDefaultSettings();

    void   setEnabledBrightnessSettings(bool b);
    bool   brightnessSettingsIsEnabled();

    void   updateMinimumWidth();

    KUrlRequester* inputProfileUrlEdit() const;
    KUrlRequester* outputProfileUrlEdit() const;

Q_SIGNALS:

    void signalSixteenBitsImageToggled(bool);
    void signalSettingsChanged();

private Q_SLOTS:

    void slotWhiteBalanceToggled(int);
    void slotsixteenBitsImageToggled(bool);
    void slotUnclipColorActivated(int);
    void slotNoiseReductionToggled(bool);
    void slotCACorrectionToggled(bool);
    void processDcrawUrl(const QString&);
    void slotInputColorSpaceChanged(int);
    void slotOutputColorSpaceChanged(int);

private:

    DcrawSettingsWidgetPriv* const d;
};

} // NameSpace KDcrawIface

#endif /* DCRAW_SETTINGS_WIDGET_H */
