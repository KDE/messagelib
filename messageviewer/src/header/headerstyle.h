/*  -*- c++ -*-
    headerstyle.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <Akonadi/MessageStatus>
#include <KMime/Message>
#include <memory.h>

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
    [[nodiscard]] const HeaderStrategy *headerStrategy() const;

    void setVCardName(const QString &vCardName);
    [[nodiscard]] QString vCardName() const;

    void setPrinting(bool printing);
    [[nodiscard]] bool isPrinting() const;

    void setTopLevel(bool topLevel);
    [[nodiscard]] bool isTopLevel() const;

    void setNodeHelper(MimeTreeParser::NodeHelper *nodeHelper);
    [[nodiscard]] MimeTreeParser::NodeHelper *nodeHelper() const;

    void setAllowAsync(bool allowAsync);
    [[nodiscard]] bool allowAsync() const;

    void setSourceObject(QObject *sourceObject);
    [[nodiscard]] QObject *sourceObject() const;

    void setMessageStatus(Akonadi::MessageStatus status);
    [[nodiscard]] Akonadi::MessageStatus messageStatus() const;

    void setTheme(const GrantleeTheme::Theme &theme);
    [[nodiscard]] GrantleeTheme::Theme theme() const;

    [[nodiscard]] bool readOnlyMessage() const;
    void setReadOnlyMessage(bool readOnlyMessage);

    [[nodiscard]] bool showEmoticons() const;
    void setShowEmoticons(bool b);

    void setAttachmentHtml(const QString &html);
    [[nodiscard]] QString attachmentHtml() const;

private:
    std::unique_ptr<HeaderStylePrivate> const d;
};
}
