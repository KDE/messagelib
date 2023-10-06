/*  -*- c++ -*-
 *    imagecollector.h
 *
 *    This file is part of KMail, the KDE mail client.
 *    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
 *    SPDX-FileCopyrightText: 2011 Torgny Nyblom <nyblom@kde.org>
 *
 *    SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "messagecore_export.h"

#include <QObject>
#include <memory>
#include <vector>
namespace KMime
{
class Content;
}

namespace MessageCore
{
/**
 * @short A helper class to collect the embedded images of a email.
 *
 * @author Marc Mutz <mutz@kde.org>
 * @author Torgny Nyblom <nyblom@kde.org>
 * @todo: Make it a simple static method?!?
 */
class MESSAGECORE_EXPORT ImageCollector
{
public:
    /**
     * Creates a new image collector.
     */
    ImageCollector();

    /**
     * Destroys the image collector.
     */
    ~ImageCollector();

    /**
     * Starts collecting the images.
     *
     * @param content The email content that contains the images.
     */
    void collectImagesFrom(KMime::Content *content);

    /**
     * Returns the collected images.
     */
    [[nodiscard]] const std::vector<KMime::Content *> &images() const;

private:
    //@cond PRIVATE
    class ImageCollectorPrivate;
    std::unique_ptr<ImageCollectorPrivate> const d;

    Q_DISABLE_COPY(ImageCollector)
    //@endcond
};
}
