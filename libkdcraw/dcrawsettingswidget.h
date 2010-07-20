/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-13
 * @brief  LibRaw settings widgets
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

#ifndef DCRAW_SETTINGS_WIDGET_H
#define DCRAW_SETTINGS_WIDGET_H

// Qt includes.

#include <QtCore/QString>

// KDE includes.

#include <kurlrequester.h>
#include <kconfig.h>

// Local includes.

#include "libkdcraw_export.h"
#include "rawdecodingsettings.h"
#include "rexpanderbox.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT DcrawSettingsWidget : public RExpanderBox
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
    DcrawSettingsWidget(QWidget* parent, int advSettings = COLORSPACE);
    virtual ~DcrawSettingsWidget();

    KUrlRequester* inputProfileUrlEdit() const;
    KUrlRequester* outputProfileUrlEdit() const;

    void setup(int advSettings);

    void setEnabledBrightnessSettings(bool b);
    bool brightnessSettingsIsEnabled();

    void updateMinimumWidth();

    /** deprecated. use resetToDefault() instead.
     */
    KDE_DEPRECATED void setDefaultSettings();
    void resetToDefault();

    void setSettings(const RawDecodingSettings& settings);
    RawDecodingSettings settings() const;

    /** deprecated. use KConfigGroup based methods instead.
     */
    KDE_DEPRECATED void readSettings() { RExpanderBox::readSettings();  };
    KDE_DEPRECATED void writeSettings(){ RExpanderBox::writeSettings(); };
    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    /** All methods below are deprecated and will be remove for 1.1.0.
        Use settings() and setSettings() instead.
     */

    KDE_DEPRECATED bool   sixteenBits();
    KDE_DEPRECATED int    customWhiteBalance();
    KDE_DEPRECATED double customWhiteBalanceGreen();
    KDE_DEPRECATED bool   useFourColor();
    KDE_DEPRECATED bool   useDontStretchPixels();
    KDE_DEPRECATED bool   useNoiseReduction();
    KDE_DEPRECATED bool   useCACorrection();
    KDE_DEPRECATED bool   useBlackPoint();
    KDE_DEPRECATED bool   useWhitePoint();
    KDE_DEPRECATED int    unclipColor();
    KDE_DEPRECATED bool   useAutoBrightness();
    KDE_DEPRECATED double brightness();
    KDE_DEPRECATED int    blackPoint();
    KDE_DEPRECATED int    whitePoint();
    KDE_DEPRECATED int    NRThreshold();
    KDE_DEPRECATED double caRedMultiplier();
    KDE_DEPRECATED double caBlueMultiplier();
    KDE_DEPRECATED int    medianFilterPasses();

    KDE_DEPRECATED RawDecodingSettings::WhiteBalance     whiteBalance();
    KDE_DEPRECATED RawDecodingSettings::DecodingQuality  quality();
    KDE_DEPRECATED RawDecodingSettings::InputColorSpace  inputColorSpace();
    KDE_DEPRECATED RawDecodingSettings::OutputColorSpace outputColorSpace();

    KDE_DEPRECATED QString inputColorProfile();
    KDE_DEPRECATED QString outputColorProfile();

    KDE_DEPRECATED void setSixteenBits(bool b);
    KDE_DEPRECATED void setCameraWB(bool b);
    KDE_DEPRECATED void setWhiteBalance(RawDecodingSettings::WhiteBalance v);
    KDE_DEPRECATED void setCustomWhiteBalance(int v);
    KDE_DEPRECATED void setCustomWhiteBalanceGreen(double v);
    KDE_DEPRECATED void setFourColor(bool b);
    KDE_DEPRECATED void setDontStretchPixels(bool b);
    KDE_DEPRECATED void setNoiseReduction(bool b);
    KDE_DEPRECATED void setUseCACorrection(bool b);
    KDE_DEPRECATED void setUseBlackPoint(bool b);
    KDE_DEPRECATED void setUseWhitePoint(bool b);
    KDE_DEPRECATED void setUnclipColor(int v);
    KDE_DEPRECATED void setAutoBrightness(bool b);
    KDE_DEPRECATED void setBrightness(double b);
    KDE_DEPRECATED void setBlackPoint(int b);
    KDE_DEPRECATED void setWhitePoint(int b);
    KDE_DEPRECATED void setNRThreshold(int b);
    KDE_DEPRECATED void setcaRedMultiplier(double b);
    KDE_DEPRECATED void setcaBlueMultiplier(double b);
    KDE_DEPRECATED void setMedianFilterPasses(int p);
    KDE_DEPRECATED void setQuality(RawDecodingSettings::DecodingQuality q);
    KDE_DEPRECATED void setInputColorSpace(RawDecodingSettings::InputColorSpace c);
    KDE_DEPRECATED void setOutputColorSpace(RawDecodingSettings::OutputColorSpace c);
    KDE_DEPRECATED void setInputColorProfile(const QString& path);
    KDE_DEPRECATED void setOutputColorProfile(const QString& path);

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

    class DcrawSettingsWidgetPriv;
    DcrawSettingsWidgetPriv* const d;
};

} // NameSpace KDcrawIface

#endif /* DCRAW_SETTINGS_WIDGET_H */
