/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "webengineaccesskeyanchor.h"

using namespace WebEngineViewer;

WebEngineAccessKeyAnchor::WebEngineAccessKeyAnchor(const QVariant &result)
{
    initialize(result);
}

WebEngineAccessKeyAnchor::WebEngineAccessKeyAnchor()
{
}

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
