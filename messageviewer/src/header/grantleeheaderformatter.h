/*
   Copyright (C) 2013-2018 Laurent Montel <montel@kde.org>

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

#ifndef GRANTLEEHEADERFORMATTER_H
#define GRANTLEEHEADERFORMATTER_H

#include <QString>
#include <grantlee/templateloader.h>
#include <GrantleeTheme/GrantleeTheme>
#include "messageviewer_export.h"
namespace KMime {
class Message;
}

namespace MessageViewer {
class HeaderStyle;
class MESSAGEVIEWER_EXPORT GrantleeHeaderFormatter
{
public:
    struct GrantleeHeaderFormatterSettings {
        GrantleeHeaderFormatterSettings()
        {
        }

        GrantleeTheme::Theme theme;
        bool isPrinting = false;
        mutable const MessageViewer::HeaderStyle *style = nullptr;
        KMime::Message *message = nullptr;
        bool showEmoticons = true;
    };

    explicit GrantleeHeaderFormatter();
    ~GrantleeHeaderFormatter();

    Q_REQUIRED_RESULT QString toHtml(const GrantleeHeaderFormatterSettings &settings) const;

    Q_REQUIRED_RESULT QString toHtml(const QStringList &displayExtraHeaders, const QString &absolutPath, const QString &filename, const MessageViewer::HeaderStyle *style, KMime::Message *message,
                                     bool isPrinting) const;
private:
    QString format(const QString &absolutePath, const Grantlee::Template &headerTemplate, const QStringList &displayExtraHeaders, bool isPrinting, const MessageViewer::HeaderStyle *style,
                   KMime::Message *message, bool showEmoticons = true) const;
    class Private;
    Private *const d;
};
}

#endif // GRANTLEEHEADERFORMATTER_H
