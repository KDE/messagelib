/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "defaultgrantleeheaderstyleplugin.h"
#include "defaultgrantleeheaderstrategy.h"
#include "defaultgrantleeheaderstyleinterface.h"
#include "globalsettings_base.h"
#include "messageviewer/grantleeheaderstyle.h"
#include <KPluginFactory>
using namespace MessageViewer;

K_PLUGIN_CLASS_WITH_JSON(DefaultGrantleeHeaderStylePlugin, "messageviewer_defaultgrantleeheaderstyleplugin.json")

DefaultGrantleeHeaderStylePlugin::DefaultGrantleeHeaderStylePlugin(QObject *parent, const QList<QVariant> &)
    : MessageViewer::HeaderStylePlugin(parent)
    , mHeaderStyle(new GrantleeHeaderStyle)
    , mHeaderStrategy(new DefaultGrantleeHeaderStrategy)
{
    initializePlugin();
}

DefaultGrantleeHeaderStylePlugin::~DefaultGrantleeHeaderStylePlugin()
{
    delete mHeaderStyle;
    delete mHeaderStrategy;
}

void DefaultGrantleeHeaderStylePlugin::initializePlugin()
{
}

HeaderStyle *DefaultGrantleeHeaderStylePlugin::headerStyle() const
{
    return mHeaderStyle;
}

HeaderStrategy *DefaultGrantleeHeaderStylePlugin::headerStrategy() const
{
    return mHeaderStrategy;
}

HeaderStyleInterface *DefaultGrantleeHeaderStylePlugin::createView(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac, QObject *parent)
{
    MessageViewer::HeaderStyleInterface *view = new MessageViewer::DefaultGrantleeHeaderStyleInterface(this, parent);
    if (ac) {
        view->createAction(menu, actionGroup, ac);
    }
    return view;
}

QString DefaultGrantleeHeaderStylePlugin::name() const
{
    return QStringLiteral("defaultgrantlee");
}

#include "defaultgrantleeheaderstyleplugin.moc"

#include "moc_defaultgrantleeheaderstyleplugin.cpp"
