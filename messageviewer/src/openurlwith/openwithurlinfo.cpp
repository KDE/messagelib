/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openwithurlinfo.h"

using namespace MessageViewer;
OpenWithUrlInfo::OpenWithUrlInfo() = default;

OpenWithUrlInfo::~OpenWithUrlInfo() = default;

QDebug operator<<(QDebug d, const MessageViewer::OpenWithUrlInfo &t)
{
    d << "command " << t.command();
    d << "url " << t.url();
    return d;
}

const QString &OpenWithUrlInfo::command() const
{
    return mCommand;
}

void OpenWithUrlInfo::setCommand(const QString &newCommand)
{
    mCommand = newCommand;
}

const QString &OpenWithUrlInfo::url() const
{
    return mUrl;
}

void OpenWithUrlInfo::setUrl(const QString &newUrl)
{
    mUrl = newUrl;
}

bool OpenWithUrlInfo::isValid() const
{
    return !mUrl.isEmpty() && !mCommand.isEmpty();
}
