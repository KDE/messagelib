/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineaccesskeyanchor.h"

using namespace WebEngineViewer;

WebEngineAccessKeyAnchor::WebEngineAccessKeyAnchor(const QVariant &result)
{
    initialize(result);
}

WebEngineAccessKeyAnchor::WebEngineAccessKeyAnchor() = default;

void WebEngineAccessKeyAnchor::initialize(const QVariant &result)
{
    if (result.isValid()) {
        const QVariantMap map = result.toMap();
        const QVariantList &rect = map.value(QStringLiteral("boundingRect")).toList();
        if (rect.size() == 4) {
            mBoundingRect = QRect(rect.at(0).toInt(), rect.at(1).toInt(), rect.at(2).toInt(), rect.at(3).toInt());
        }
        mHref = map.value(QStringLiteral("src")).toString();
        mAccessKey = map.value(QStringLiteral("accessKey")).toString();
        mTarget = map.value(QStringLiteral("target")).toString();
        mTagName = map.value(QStringLiteral("tagName")).toString();
        mInnerText = map.value(QStringLiteral("text")).toString();
    }
}

QString WebEngineAccessKeyAnchor::innerText() const
{
    return mInnerText;
}

QString WebEngineAccessKeyAnchor::tagName() const
{
    return mTagName;
}

QString WebEngineAccessKeyAnchor::target() const
{
    return mTarget;
}

QString WebEngineAccessKeyAnchor::href() const
{
    return mHref;
}

QString WebEngineAccessKeyAnchor::accessKey() const
{
    return mAccessKey;
}

QRect WebEngineAccessKeyAnchor::boundingRect() const
{
    return mBoundingRect;
}
