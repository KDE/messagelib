/*  -*- c++ -*-
    headerstyle.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

//#include <GrantleeTheme/GrantleeTheme>

#include <Akonadi/KMime/MessageStatus>
#include <KMime/Message>

class QString;

namespace MimeTreeParser
{
class NodeHelper;
}

namespace GrantleeTheme
{
class Theme;
}

namespace MessageViewer
{
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

