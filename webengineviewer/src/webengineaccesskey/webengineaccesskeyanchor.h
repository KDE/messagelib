/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QRect>
#include <QVariant>

namespace WebEngineViewer
{
class WebEngineAccessKeyAnchor
{
public:
    explicit WebEngineAccessKeyAnchor(const QVariant &result);
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
