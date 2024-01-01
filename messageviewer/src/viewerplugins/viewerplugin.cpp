/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "viewerplugin.h"

using namespace MessageViewer;

class MessageViewer::ViewerPluginPrivate
{
public:
    ViewerPluginPrivate() = default;

    bool mEnabled = false;
};

ViewerPlugin::ViewerPlugin(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ViewerPluginPrivate)
{
}

ViewerPlugin::~ViewerPlugin() = default;

void ViewerPlugin::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
}

bool ViewerPlugin::hasConfigureDialog() const
{
    return false;
}

void ViewerPlugin::setIsEnabled(bool enabled)
{
    d->mEnabled = enabled;
}

bool ViewerPlugin::isEnabled() const
{
    return d->mEnabled;
}

#include "moc_viewerplugin.cpp"
