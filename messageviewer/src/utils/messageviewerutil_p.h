/*******************************************************************************
**
** Filename   : util
** Created on : 03 April, 2005
** SPDX-FileCopyrightText : 2005 Till Adam <adam@kde.org>
**
*******************************************************************************/

/*******************************************************************************
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   It is distributed in the hope that it will be useful, but
**   WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
**   In addition, as a special exception, the copyright holders give
**   permission to link the code of this program with any edition of
**   the Qt library by Trolltech AS, Norway (or with modified versions
**   of Qt that use the same license as Qt), and distribute linked
**   combinations including the two.  You must obey the GNU General
**   Public License in all respects for all of the code used other than
**   Qt.  If you modify this file, you may extend this exception to
**   your version of the file, but you are not obligated to do so.  If
**   you do not wish to do so, delete this exception statement from
**   your version.
**
*******************************************************************************/
#pragma once

#include <KMime/Content>
class QUrl;
class QWidget;
class QAction;

namespace MessageViewer
{
/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{
Q_REQUIRED_RESULT bool handleUrlWithQDesktopServices(const QUrl &url);

Q_REQUIRED_RESULT KMime::Content::List allContents(const KMime::Content *message);

Q_REQUIRED_RESULT bool saveContents(QWidget *parent, const KMime::Content::List &contents, QList<QUrl> &urlList);
Q_REQUIRED_RESULT bool saveContent(QWidget *parent, KMime::Content *content, const QUrl &url);
void addHelpTextAction(QAction *act, const QString &text);
void readGravatarConfig();
}
}
