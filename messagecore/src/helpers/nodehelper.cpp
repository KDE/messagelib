/*
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nodehelper.h"

#include <kmime/kmime_content.h>
#include <kmime/kmime_message.h>

KMime::Content *MessageCore::NodeHelper::nextSibling(const KMime::Content *node)
{
    if (!node) {
        return nullptr;
    }

    KMime::Content *next = nullptr;
    KMime::Content *parent = node->parent();
    if (parent) {
        const auto contents = parent->contents();
        const int index = contents.indexOf(const_cast<KMime::Content *>(node)) + 1;
        if (index < contents.size()) { // next on the same level
            next = contents.at(index);
        }
    }

    return next;
}

KMime::Content *MessageCore::NodeHelper::next(KMime::Content *node, bool allowChildren)
{
    if (allowChildren) {
        if (KMime::Content *child = firstChild(node)) {
            return child;
        }
    }

    if (KMime::Content *sibling = nextSibling(node)) {
        return sibling;
    }

    for (KMime::Content *parent = node->parent(); parent; parent = parent->parent()) {
        if (KMime::Content *sibling = nextSibling(parent)) {
            return sibling;
        }
    }

    return nullptr;
}

KMime::Content *MessageCore::NodeHelper::firstChild(const KMime::Content *node)
{
    if (!node) {
        return nullptr;
    }

    KMime::Content *child = nullptr;
    if (!node->contents().isEmpty()) {
        child = node->contents().at(0);
    }

    return child;
}
