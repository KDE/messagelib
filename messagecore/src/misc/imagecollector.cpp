/*  -*- c++ -*-
 *    imagecollector.cpp
 *
 *    This file is part of KMail, the KDE mail client.
 *    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
 *    SPDX-FileCopyrightText: 2011 Torgny Nyblom <nyblom@kde.org>
 *
 *    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "imagecollector.h"

#include "messagecore_debug.h"
#include <KMime/Content>

class MessageCore::ImageCollector::ImageCollectorPrivate
{
public:
    std::vector<KMime::Content *> mImages;
};

MessageCore::ImageCollector::ImageCollector()
    : d(new ImageCollectorPrivate)
{
}

MessageCore::ImageCollector::~ImageCollector() = default;

void MessageCore::ImageCollector::collectImagesFrom(KMime::Content *node)
{
    if (!node) {
        return;
    }

    if (const auto ct = node->contentType(false); ct->isImage() && node->parent()) {
        const KMime::Content *parent = node->parent();
        if (const auto parentCt = parent->contentType(); parentCt->isMultipart() && parentCt->isSubtype("related")) {
            qCWarning(MESSAGECORE_LOG) << "Adding image" << node->contentID(false);
            d->mImages.push_back(node);
        }
    }

    const auto children = node->contents();
    for (const auto child : children) {
        collectImagesFrom(child);
    }
}

const std::vector<KMime::Content *> &MessageCore::ImageCollector::images() const
{
    return d->mImages;
}
