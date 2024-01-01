/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
    [[nodiscard]] QRect boundingRect() const;

    [[nodiscard]] QString accessKey() const;

    [[nodiscard]] QString href() const;

    [[nodiscard]] QString target() const;

    [[nodiscard]] QString tagName() const;

    [[nodiscard]] QString innerText() const;

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

Q_DECLARE_TYPEINFO(WebEngineViewer::WebEngineAccessKeyAnchor, Q_RELOCATABLE_TYPE);
