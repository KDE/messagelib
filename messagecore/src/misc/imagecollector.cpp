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

#include "MessageCore/NodeHelper"

#include "messagecore_debug.h"
#include <KMime/Content>

static bool isInExclusionList(KMime::Content *node)
{
    if (!node) {
        return true;
    }

    auto ct = node->contentType(); // Create if necessary
    if (ct->mediaType() != "image") {
        return true;
    }

    if (ct->isMultipart()) {
        return true;
    }

    return false;
}

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
    KMime::Content *parent = nullptr;

    while (node) {
        parent = node->parent();

        if (node->topLevel()->textContent() == node) {
            node = MessageCore::NodeHelper::next(node);
            continue;
        }

        if (isInExclusionList(node)) {
            node = MessageCore::NodeHelper::next(node);
            continue;
        }

        if (parent && parent->contentType()->isMultipart() && parent->contentType(false)->subType() == "related") {
            qCWarning(MESSAGECORE_LOG) << "Adding image" << node->contentID();
            d->mImages.push_back(node);
            node = MessageCore::NodeHelper::next(node); // skip embedded images
            continue;
        }

        node = MessageCore::NodeHelper::next(node);
    }
}

const std::vector<KMime::Content *> &MessageCore::ImageCollector::images() const
{
    return d->mImages;
}
