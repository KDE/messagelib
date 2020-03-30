/*******************************************************************************
**
** Filename   : util
** Created on : 03 April, 2005
** Copyright  : (c) 2005 Till Adam
** Email      : <adam@kde.org>
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
#ifndef MESSAGEVIEWERUTIL_H
#define MESSAGEVIEWERUTIL_H

#include "messageviewer_export.h"
#include <KMime/Content>
#include <KService>
#include <AkonadiCore/Item>

class QUrl;
class QWidget;
class QActionGroup;
class QAction;

namespace KMime {
class Message;
}

namespace MessageViewer {
/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util {
// return true if we should proceed, false if we should abort
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool containsExternalReferences(const QString &str, const QString &extraHead);
Q_REQUIRED_RESULT bool MESSAGEVIEWER_EXPORT checkOverwrite(const QUrl &url, QWidget *w);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool saveMessageInMboxAndGetUrl(QUrl &url, const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool saveMessageInMbox(const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QString generateMboxFileName(const Akonadi::Item &msgBase);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool saveAttachments(const KMime::Content::List &contents, QWidget *parent, QList<QUrl> &saveUrl);

Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QAction *createAppAction(const KService::Ptr &service, bool singleOffer, QActionGroup *actionGroup, QObject *parent);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool excludeExtraHeader(const QString &s);
/** Return a QTextCodec for the specified charset.
* This function is a bit more tolerant, than QTextCodec::codecForName */
MESSAGEVIEWER_EXPORT const QTextCodec *codecForName(const QByteArray &_str);

Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QString processHtml(const QString &htmlSource, QString &extraHead);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QByteArray htmlCodec(const QByteArray &data, const QByteArray &currentCodec);
}
}
Q_DECLARE_METATYPE(KService::Ptr)
#endif
