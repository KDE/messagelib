/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "grantleeheaderteststyle.h"

#include "header/grantleeheaderformatter.h"

#include <KMime/Message>

using namespace MessageViewer;

class MessageViewer::GrantleeHeaderTestStylePrivate
{
public:
    GrantleeHeaderTestStylePrivate()
        : mGrantleeFormatter(new GrantleeHeaderFormatter)
    {
    }

    ~GrantleeHeaderTestStylePrivate()
    {
        delete mGrantleeFormatter;
    }

    QStringList mExtraDisplay;
    QString mAbsolutePath;
    QString mMainFilename;
    GrantleeHeaderFormatter *const mGrantleeFormatter;
};

GrantleeHeaderTestStyle::GrantleeHeaderTestStyle()
    : HeaderStyle()
    , d(new MessageViewer::GrantleeHeaderTestStylePrivate)
{
}

GrantleeHeaderTestStyle::~GrantleeHeaderTestStyle() = default;

const char *GrantleeHeaderTestStyle::name() const
{
    return "grantleetest";
}

QString GrantleeHeaderTestStyle::format(KMime::Message *message) const
{
    if (!message) {
        return {};
    }
    return d->mGrantleeFormatter->toHtml(d->mExtraDisplay, d->mAbsolutePath, d->mMainFilename, this, message, isPrinting());
}

void GrantleeHeaderTestStyle::setAbsolutePath(const QString &path)
{
    d->mAbsolutePath = path;
}

void GrantleeHeaderTestStyle::setMainFilename(const QString &filename)
{
    d->mMainFilename = filename;
}

void GrantleeHeaderTestStyle::setExtraDisplayHeaders(const QStringList &extraDisplay)
{
    d->mExtraDisplay = extraDisplay;
}
