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
#ifndef MAILWEBENGINEPAGE_H
#define MAILWEBENGINEPAGE_H

#include "messageviewer_export.h"
#include <WebEngineViewer/WebEnginePage>

namespace MessageViewer {
class MESSAGEVIEWER_EXPORT MailWebEnginePage : public WebEngineViewer::WebEnginePage
{
    Q_OBJECT
public:
    explicit MailWebEnginePage(QObject *parent = nullptr);
    explicit MailWebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);
    ~MailWebEnginePage();

    void setPrintElementBackground(bool printElementBackground);

private:
    void slotFeaturePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature);
    void initialize();
};
}
#endif // MAILWEBENGINEPAGE_H
