/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mailwebengineaccesskeyanchor.h"
#include <QDebug>

using namespace MessageViewer;

MailWebEngineAccessKeyAnchor::MailWebEngineAccessKeyAnchor(const QVariant &result)
{
    initialize(result);
}

MailWebEngineAccessKeyAnchor::MailWebEngineAccessKeyAnchor()
{

}

void MailWebEngineAccessKeyAnchor::initialize(const QVariant &result)
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
    }
}

QString MailWebEngineAccessKeyAnchor::innerText() const
{
    return mInnerText;
}

QString MailWebEngineAccessKeyAnchor::tagName() const
{
    return mTagName;
}

QString MailWebEngineAccessKeyAnchor::target() const
{
    return mTarget;
}

QString MailWebEngineAccessKeyAnchor::href() const
{
    return mHref;
}

QString MailWebEngineAccessKeyAnchor::accessKey() const
{
    return mAccessKey;
}

QRect MailWebEngineAccessKeyAnchor::boundingRect() const
{
    return mBoundingRect;
}
