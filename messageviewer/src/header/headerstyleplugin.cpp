/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstyleplugin.h"

using namespace MessageViewer;
class MessageViewer::HeaderStylePluginPrivate
{
public:
    HeaderStylePluginPrivate() = default;

    bool mIsEnabled = false;
    bool mShowEmoticons = true;
};

HeaderStylePlugin::HeaderStylePlugin(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::HeaderStylePluginPrivate)
{
}

HeaderStylePlugin::~HeaderStylePlugin() = default;

bool HeaderStylePlugin::hasMargin() const
{
    return true;
}

QString HeaderStylePlugin::alignment() const
{
    return QStringLiteral("left");
}

int HeaderStylePlugin::elidedTextSize() const
{
    return -1;
}

void HeaderStylePlugin::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool HeaderStylePlugin::isEnabled() const
{
    return d->mIsEnabled;
}

QString HeaderStylePlugin::attachmentHtml() const
{
    return {};
}

bool HeaderStylePlugin::hasConfigureDialog() const
{
    return false;
}

void HeaderStylePlugin::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
    // Reimplement
}

QString HeaderStylePlugin::extraScreenCss(const QString &headerFont) const
{
    Q_UNUSED(headerFont)
    return {};
}

QString HeaderStylePlugin::extraPrintCss(const QString &headerFont) const
{
    Q_UNUSED(headerFont)
    return {};
}

QString HeaderStylePlugin::extraCommonCss(const QString &headerFont) const
{
    Q_UNUSED(headerFont)
    return {};
}

#include "moc_headerstyleplugin.cpp"
