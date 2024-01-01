/*  -*- c++ -*-
    headerstyle.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "headerstyle.h"
#include <GrantleeTheme/GrantleeTheme>

using namespace MessageViewer;
//
// HeaderStyle abstract base:
//
class MessageViewer::HeaderStylePrivate
{
public:
    HeaderStylePrivate() = default;

    GrantleeTheme::Theme mTheme;
    const HeaderStrategy *mStrategy = nullptr;
    QString mVCardName;
    QString mAttachmentHtml;
    MimeTreeParser::NodeHelper *mNodeHelper = nullptr;
    QObject *mSourceObject = nullptr;
    Akonadi::MessageStatus mMessageStatus;
    bool mPrinting = false;
    bool mTopLevel = true;
    bool mAllowAsync = false;
    bool mReadOnlyMessage = false;
    bool mShowEmoticons = true;
};

HeaderStyle::HeaderStyle()
    : d(new MessageViewer::HeaderStylePrivate)
{
}

HeaderStyle::~HeaderStyle() = default;

void HeaderStyle::setAttachmentHtml(const QString &html)
{
    d->mAttachmentHtml = html;
}

QString HeaderStyle::attachmentHtml() const
{
    return d->mAttachmentHtml;
}

void HeaderStyle::setHeaderStrategy(const HeaderStrategy *strategy)
{
    d->mStrategy = strategy;
}

const HeaderStrategy *HeaderStyle::headerStrategy() const
{
    return d->mStrategy;
}

void HeaderStyle::setVCardName(const QString &vCardName)
{
    d->mVCardName = vCardName;
}

QString HeaderStyle::vCardName() const
{
    return d->mVCardName;
}

void HeaderStyle::setPrinting(bool printing)
{
    d->mPrinting = printing;
}

bool HeaderStyle::isPrinting() const
{
    return d->mPrinting;
}

void HeaderStyle::setTopLevel(bool topLevel)
{
    d->mTopLevel = topLevel;
}

bool HeaderStyle::isTopLevel() const
{
    return d->mTopLevel;
}

void HeaderStyle::setNodeHelper(MimeTreeParser::NodeHelper *nodeHelper)
{
    d->mNodeHelper = nodeHelper;
}

MimeTreeParser::NodeHelper *HeaderStyle::nodeHelper() const
{
    return d->mNodeHelper;
}

void HeaderStyle::setAllowAsync(bool allowAsync)
{
    d->mAllowAsync = allowAsync;
}

bool HeaderStyle::allowAsync() const
{
    return d->mAllowAsync;
}

void HeaderStyle::setSourceObject(QObject *sourceObject)
{
    d->mSourceObject = sourceObject;
}

QObject *HeaderStyle::sourceObject() const
{
    return d->mSourceObject;
}

void HeaderStyle::setMessageStatus(Akonadi::MessageStatus status)
{
    d->mMessageStatus = status;
}

Akonadi::MessageStatus HeaderStyle::messageStatus() const
{
    return d->mMessageStatus;
}

void HeaderStyle::setTheme(const GrantleeTheme::Theme &theme)
{
    d->mTheme = theme;
}

GrantleeTheme::Theme HeaderStyle::theme() const
{
    return d->mTheme;
}

bool HeaderStyle::readOnlyMessage() const
{
    return d->mReadOnlyMessage;
}

void HeaderStyle::setReadOnlyMessage(bool readOnlyMessage)
{
    d->mReadOnlyMessage = readOnlyMessage;
}

bool HeaderStyle::showEmoticons() const
{
    return d->mShowEmoticons;
}

void HeaderStyle::setShowEmoticons(bool b)
{
    d->mShowEmoticons = b;
}
