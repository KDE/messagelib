/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openwithurlinfo.h"

using namespace MessageViewer;
OpenWithUrlInfo::OpenWithUrlInfo() = default;

OpenWithUrlInfo::~OpenWithUrlInfo() = default;

QDebug operator<<(QDebug d, const MessageViewer::OpenWithUrlInfo &t)
{
    d.space() << "command " << t.command();
    d.space() << "command line " << t.commandLine();
    d.space() << "url " << t.url();
    d.space() << "isLocalOpenWith" << t.isLocalOpenWithInfo();
    d.space() << "enabled" << t.enabled();
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

const QString &OpenWithUrlInfo::commandLine() const
{
    return mCommandLine;
}

void OpenWithUrlInfo::setCommandLine(const QString &newCommandLine)
{
    mCommandLine = newCommandLine;
}

bool OpenWithUrlInfo::operator==(const OpenWithUrlInfo &other) const
{
    return mCommandLine == other.commandLine() && mCommand == other.command() && mUrl == other.url() && mIsLocalOpenWithInfo == other.isLocalOpenWithInfo()
        && mEnabled == other.enabled();
}

bool OpenWithUrlInfo::isLocalOpenWithInfo() const
{
    return mIsLocalOpenWithInfo;
}

void OpenWithUrlInfo::setIsLocalOpenWithInfo(bool newLocalOpenWith)
{
    mIsLocalOpenWithInfo = newLocalOpenWith;
}

bool OpenWithUrlInfo::enabled() const
{
    return mEnabled;
}

void OpenWithUrlInfo::setEnabled(bool newEnabled)
{
    mEnabled = newEnabled;
}

#include "moc_openwithurlinfo.cpp"
