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

#ifndef MAILWEBENGINEACCESSKEYANCHOR_H
#define MAILWEBENGINEACCESSKEYANCHOR_H

#include <QVariant>
#include <QRect>

namespace MessageViewer
{
class MailWebEngineAccessKeyAnchor
{
public:
    MailWebEngineAccessKeyAnchor(const QVariant &result);
    MailWebEngineAccessKeyAnchor();
    QRect boundingRect() const;

    QString accessKey() const;

    QString href() const;

    QString target() const;

    QString tagName() const;

    QString innerText() const;

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
#endif // MAILWEBENGINEACCESSKEYANCHOR_H
