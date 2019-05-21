/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#ifndef MAILWEBENGINEACCESSKEYANCHOR_H
#define MAILWEBENGINEACCESSKEYANCHOR_H

#include <QVariant>
#include <QRect>

namespace WebEngineViewer {
class WebEngineAccessKeyAnchor
{
public:
    WebEngineAccessKeyAnchor(const QVariant &result);
    WebEngineAccessKeyAnchor();
    Q_REQUIRED_RESULT QRect boundingRect() const;

    Q_REQUIRED_RESULT QString accessKey() const;

    Q_REQUIRED_RESULT QString href() const;

    Q_REQUIRED_RESULT QString target() const;

    Q_REQUIRED_RESULT QString tagName() const;

    Q_REQUIRED_RESULT QString innerText() const;

private:
    void initialize(const QVariant &result);
    QRect mBoundingRect;
    QString mAccessKey;
    QString mHref;
    QString mTarget;
    QString mTagName;
    QString mInnerText;
};
}

Q_DECLARE_TYPEINFO(WebEngineViewer::WebEngineAccessKeyAnchor, Q_MOVABLE_TYPE);
#endif // MAILWEBENGINEACCESSKEYANCHOR_H
