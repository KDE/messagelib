/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanager.h"

#include <KConfigGroup>
#include <KSharedConfig>
using namespace MessageViewer;
namespace
{
static const char myOpenUrlWithGroupName[] = "OpenUrlWith";
}
OpenUrlWithManager::OpenUrlWithManager(QObject *parent)
    : QObject{parent}
{
    loadSettings();
}

OpenUrlWithManager::~OpenUrlWithManager()
{
    writeSettings();
}

OpenUrlWithManager *OpenUrlWithManager::self()
{
    static OpenUrlWithManager s_self;
    return &s_self;
}

void OpenUrlWithManager::clear()
{
    mOpenWithUrlInfo.clear();
}

void OpenUrlWithManager::loadSettings()
{
    mOpenWithUrlInfo.clear();
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, myOpenUrlWithGroupName);
    const QStringList openWithUrls = group.readEntry("Urls", QStringList());
    const QStringList commands = group.readEntry("Commands", QStringList());
    mOpenWithUrlInfo.reserve(commands.count());
    for (int i = 0; i < openWithUrls.count(); ++i) {
        OpenWithUrlInfo info;
        info.setCommand(commands.at(i));
        info.setUrl(openWithUrls.at(i));
        mOpenWithUrlInfo.append(info);
    }
}

void OpenUrlWithManager::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, myOpenUrlWithGroupName);
    QStringList openWithUrls;
    QStringList commands;
    const int nbElement{mOpenWithUrlInfo.count()};
    openWithUrls.reserve(nbElement);
    commands.reserve(nbElement);
    for (int i = 0; i < nbElement; ++i) {
        commands.append(mOpenWithUrlInfo.at(i).command());
        openWithUrls.append(mOpenWithUrlInfo.at(i).url());
    }
    group.writeEntry("Urls", openWithUrls);
    group.writeEntry("Commands", commands);
    group.sync();
}

const QVector<OpenWithUrlInfo> &OpenUrlWithManager::openWithUrlInfo() const
{
    return mOpenWithUrlInfo;
}

void OpenUrlWithManager::setOpenWithUrlInfo(const QVector<OpenWithUrlInfo> &newOpenWithUrlInfo)
{
    mOpenWithUrlInfo = newOpenWithUrlInfo;
}
