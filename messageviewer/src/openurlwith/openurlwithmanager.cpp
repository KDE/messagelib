/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanager.h"
#include "messageviewer_debug.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

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

OpenUrlWithManager::~OpenUrlWithManager() = default;

OpenUrlWithManager *OpenUrlWithManager::self()
{
    static OpenUrlWithManager s_self;
    return &s_self;
}

void OpenUrlWithManager::clear()
{
    mOpenWithUrlInfo.clear();
}

void OpenUrlWithManager::loadUserSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myOpenUrlWithGroupName));
    const QStringList openWithUrls = group.readEntry("Urls", QStringList());
    const QStringList commands = group.readEntry("Commands", QStringList());
    const QStringList commandLines = group.readEntry("CommandLines", QStringList());
    const QList<bool> enabledCommandLines = group.readEntry("EnabledCommandLines", QList<bool>());
    const auto nbElement{openWithUrls.count()};
    mOpenWithUrlInfo.reserve(nbElement);
    for (int i = 0; i < nbElement; ++i) {
        OpenWithUrlInfo info;
        info.setCommand(commands.at(i));
        info.setUrl(openWithUrls.at(i));
        if (i < commandLines.count()) {
            info.setCommandLine(commandLines.at(i));
        }
        if (i < enabledCommandLines.count()) {
            info.setEnabled(enabledCommandLines.at(i));
        }
        mOpenWithUrlInfo.append(std::move(info));
    }
}

QStringList OpenUrlWithManager::openUrlWithListPath() const
{
    return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("messageviewer/openurlwith/"), QStandardPaths::LocateDirectory);
}

void OpenUrlWithManager::loadGlobalSettings()
{
    for (const QString &path : openUrlWithListPath()) {
        QDir dir(path);
        const QStringList entries = dir.entryList({QStringLiteral("*.openurl")});
        for (const QString &entry : entries) {
            qCDebug(MESSAGEVIEWER_LOG) << "files: " << entry;
            QSettings settings(path + QLatin1Char('/') + entry, QSettings::IniFormat);
            OpenWithUrlInfo info;
            info.setIsLocalOpenWithInfo(false);
            info.setCommand(settings.value("Command").toString());
            info.setCommandLine(settings.value("CommandLine").toString());
            info.setUrl(settings.value("Url").toString());
            if (info.isValid()) {
                mOpenWithUrlInfo.append(std::move(info));
            }
        }
    }
}

void OpenUrlWithManager::loadSettings()
{
    mOpenWithUrlInfo.clear();
    loadUserSettings();
    loadGlobalSettings();
}

void OpenUrlWithManager::saveRules()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myOpenUrlWithGroupName));
    QStringList openWithUrls;
    QStringList commands;
    QStringList commandLines;
    QList<bool> enabledCommandLines;
    const auto nbElement{mOpenWithUrlInfo.count()};
    openWithUrls.reserve(nbElement);
    commands.reserve(nbElement);
    for (int i = 0; i < nbElement; ++i) {
        const auto &openWith(mOpenWithUrlInfo.at(i));
        if (openWith.isLocalOpenWithInfo()) {
            commands.append(openWith.command());
            openWithUrls.append(openWith.url());
            commandLines.append(openWith.commandLine());
            enabledCommandLines.append(openWith.enabled());
        }
    }
    group.writeEntry("Urls", openWithUrls);
    group.writeEntry("Commands", commands);
    group.writeEntry("CommandLines", commandLines);
    group.writeEntry("EnabledCommandLines", enabledCommandLines);
    group.sync();
}

bool OpenUrlWithManager::alwaysRuleForHost(const QUrl &url)
{
    for (const OpenWithUrlInfo &info : std::as_const(mOpenWithUrlInfo)) {
        // qDebug() << " info.url()" << info.url() << " url.host() " << url.host();
        if (info.enabled()) {
            if (QUrl(info.url()).host() == url.host()) {
                return true;
            }
        }
    }
    return false;
}

OpenWithUrlInfo OpenUrlWithManager::openWith(const QUrl &url)
{
    for (const OpenWithUrlInfo &info : std::as_const(mOpenWithUrlInfo)) {
        // qDebug() << " info.url()" << info.url() << " url.host() " << url.host();
        if (info.enabled()) {
            if (QUrl(info.url()).host() == url.host()) {
                return info;
            }
        }
    }
    return {};
}

QStringList OpenUrlWithManager::hosts() const
{
    QStringList lst;
    for (const OpenWithUrlInfo &info : std::as_const(mOpenWithUrlInfo)) {
        lst.append(info.url());
    }
    return lst;
}

const QList<OpenWithUrlInfo> &OpenUrlWithManager::openWithUrlInfo() const
{
    return mOpenWithUrlInfo;
}

void OpenUrlWithManager::setOpenWithUrlInfo(const QList<OpenWithUrlInfo> &newOpenWithUrlInfo)
{
    mOpenWithUrlInfo = newOpenWithUrlInfo;
}

#include "moc_openurlwithmanager.cpp"
