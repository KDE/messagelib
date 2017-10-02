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
#ifndef MAILWEBENGINESCRIPT_H
#define MAILWEBENGINESCRIPT_H

#include "messageviewer_export.h"
#include <QString>

namespace MessageViewer {
namespace MailWebEngineScript {
MESSAGEVIEWER_EXPORT QString injectAttachments(const QString &delayedHtml, const QString &element);
MESSAGEVIEWER_EXPORT QString replaceInnerHtml(const QString &field, const QString &html);
MESSAGEVIEWER_EXPORT QString manageShowHideAttachments(bool hide);
MESSAGEVIEWER_EXPORT QString manageShowHideToAddress(bool hide);
MESSAGEVIEWER_EXPORT QString manageShowHideCcAddress(bool hide);
MESSAGEVIEWER_EXPORT QString manageShowHideEncryptionDetails(bool hide);
MESSAGEVIEWER_EXPORT QString createShowHideAddressScript(const QString &field, bool hide);
}
}
#endif // MAILWEBENGINESCRIPT_H
