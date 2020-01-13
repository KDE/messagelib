/*  -*- c++ -*-
    headerstyle.h

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#ifndef MESSAGEVIEWER_HEADERSTYLE_H
#define MESSAGEVIEWER_HEADERSTYLE_H

#include "messageviewer_export.h"

//#include "grantleetheme/grantleetheme.h"

#include <Akonadi/KMime/MessageStatus>
#include <KMime/Message>

class QString;

namespace MimeTreeParser {
class NodeHelper;
}

namespace GrantleeTheme {
class Theme;
}

namespace MessageViewer {
class HeaderStrategy;
class HeaderStylePrivate;

/** This class encapsulates the visual appearance of message
    headers. Together with HeaderStrategy, which determines
    which of the headers present in the message be shown, it is
    responsible for the formatting of message headers.

    @short Encapsulates visual appearance of message headers.
    @author Marc Mutz <mutz@kde.org>
    @see HeaderStrategy
**/
class MESSAGEVIEWER_EXPORT HeaderStyle
{
protected:
    HeaderStyle();

public:
    virtual ~HeaderStyle();
    //
    // Methods for handling the styles:
    //
    virtual const char *name() const = 0;

    //
    // HeaderStyle interface:
    //
    virtual QString format(KMime::Message *message) const = 0;

    void setHeaderStrategy(const HeaderStrategy *strategy);
    Q_REQUIRED_RESULT const HeaderStrategy *headerStrategy() const;

    void setVCardName(const QString &vCardName);
    Q_REQUIRED_RESULT QString vCardName() const;

    void setPrinting(bool printing);
    Q_REQUIRED_RESULT bool isPrinting() const;

    void setTopLevel(bool topLevel);
    Q_REQUIRED_RESULT bool isTopLevel() const;

    void setNodeHelper(MimeTreeParser::NodeHelper *nodeHelper);
    Q_REQUIRED_RESULT MimeTreeParser::NodeHelper *nodeHelper() const;

    void setAllowAsync(bool allowAsync);
    Q_REQUIRED_RESULT bool allowAsync() const;

    void setSourceObject(QObject *sourceObject);
    QObject *sourceObject() const;

    void setMessageStatus(Akonadi::MessageStatus status);
    Q_REQUIRED_RESULT Akonadi::MessageStatus messageStatus() const;

    void setTheme(const GrantleeTheme::Theme &theme);
    Q_REQUIRED_RESULT GrantleeTheme::Theme theme() const;
    void setCollectionName(const QString &name);
    Q_REQUIRED_RESULT QString collectionName() const;

    Q_REQUIRED_RESULT bool readOnlyMessage() const;
    void setReadOnlyMessage(bool readOnlyMessage);

    Q_REQUIRED_RESULT bool showEmoticons() const;
    void setShowEmoticons(bool b);

    void setAttachmentHtml(const QString &html);
    Q_REQUIRED_RESULT QString attachmentHtml() const;
private:
    HeaderStylePrivate *const d;
};
}

#endif // MESSAGEVIEWER_HEADERSTYLE_H
