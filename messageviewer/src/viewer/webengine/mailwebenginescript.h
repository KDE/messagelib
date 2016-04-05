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

#ifndef MAILWEBENGINESCRIPT_H
#define MAILWEBENGINESCRIPT_H

#include "messageviewer_export.h"
#include <QString>
#include <QPoint>

namespace MessageViewer
{
namespace MailWebEngineScript
{
MESSAGEVIEWER_EXPORT QString injectAttachments(const QString &delayedHtml, const QString &element);
MESSAGEVIEWER_EXPORT QString toggleFullAddressList(const QString &field, const QString &html, bool doShow);
MESSAGEVIEWER_EXPORT QString replaceInnerHtml(const QString &field, const QString &html, bool doShow);
MESSAGEVIEWER_EXPORT QString updateToggleFullAddressList(const QString &field, bool doShow);
}
}
#endif // MAILWEBENGINESCRIPT_H
