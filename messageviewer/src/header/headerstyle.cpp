/*  -*- c++ -*-
    headerstyle.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    Copyright (C) 2013-2017 Laurent Montel <montel@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

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

#include "headerstyle.h"
#include "messageviewer_debug.h"
#include "grantleetheme/grantleetheme.h"

using namespace MessageViewer;
//
// HeaderStyle abstract base:
//
class MessageViewer::HeaderStylePrivate
{
public:
    HeaderStylePrivate()
        : mStrategy(nullptr)
        , mNodeHelper(nullptr)
        , mSourceObject(nullptr)
        , mPrinting(false)
        , mTopLevel(true)
        , mAllowAsync(false)
        , mReadOnlyMessage(false)
        , mShowEmoticons(true)
    {
    }

    GrantleeTheme::Theme mTheme;
    QString mMessagePath;
    const HeaderStrategy *mStrategy;
    QString mVCardName;
    QString mCollectionName;
    MimeTreeParser::NodeHelper *mNodeHelper;
    QObject *mSourceObject;
    Akonadi::MessageStatus mMessageStatus;
    bool mPrinting;
    bool mTopLevel;
    bool mAllowAsync;
    bool mReadOnlyMessage;
    bool mShowEmoticons;
};

HeaderStyle::HeaderStyle()
    : d(new MessageViewer::HeaderStylePrivate)
{
}

HeaderStyle::~HeaderStyle()
{
    delete d;
}

bool HeaderStyle::hasAttachmentQuickList() const
{
    return false;
}

void HeaderStyle::setMessagePath(const QString &path)
{
    d->mMessagePath = path;
}

QString HeaderStyle::messagePath() const
{
    return d->mMessagePath;
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

void HeaderStyle::setCollectionName(const QString &name)
{
    d->mCollectionName = name;
}

QString HeaderStyle::collectionName() const
{
    return d->mCollectionName;
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
