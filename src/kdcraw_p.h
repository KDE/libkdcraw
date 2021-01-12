/*
    SPDX-FileCopyrightText: 2008-2015 Gilles Caulier <caulier dot gilles at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDCRAWPRIVATE_H
#define KDCRAWPRIVATE_H

// Qt includes

#include <QByteArray>

// Pragma directives to reduce warnings from LibRaw header files.
#if !defined(__APPLE__) && defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

// LibRaw includes

#include <libraw.h>

// Restore warnings
#if !defined(__APPLE__) && defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic pop
#endif

// Local includes

#include "dcrawinfocontainer.h"
#include "kdcraw.h"

namespace KDcrawIface
{

extern "C"
{
    int callbackForLibRaw(void* data, enum LibRaw_progress p, int iteration, int expected);
}

class KDcrawPrivate
{

public:

    explicit KDcrawPrivate(KDcraw* const p);
    ~KDcrawPrivate();

public:

    int    progressCallback(enum LibRaw_progress p, int iteration, int expected);

    void   setProgress(double value);
    double progressValue() const;

    bool   loadFromLibraw(const QString& filePath, QByteArray& imageData,
                          int& width, int& height, int& rgbmax);

public:

    static void createPPMHeader(QByteArray& imgData, libraw_processed_image_t* const img);

    static void fillIndentifyInfo(LibRaw* const raw, DcrawInfoContainer& identify);

    static bool loadEmbeddedPreview(QByteArray&, LibRaw&);

    static bool loadHalfPreview(QImage&, LibRaw&);

private:

    double  m_progress;

    KDcraw* m_parent;

    friend class KDcraw;
};

}  // namespace KDcrawIface

#endif /* KDCRAWPRIVATE_H */
