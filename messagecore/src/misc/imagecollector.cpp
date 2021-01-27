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
#include <kmime/kmime_content.h>

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

class Q_DECL_HIDDEN MessageCore::ImageCollector::Private
{
public:
    std::vector<KMime::Content *> mImages;
};

MessageCore::ImageCollector::ImageCollector()
    : d(new Private)
{
}

MessageCore::ImageCollector::~ImageCollector()
{
    delete d;
}

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
