/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "defaultgrantleeheaderstyleinterface.h"
#include "messageviewer/headerstyleplugin.h"
#include "messageviewer/headerstyle.h"
#include <GrantleeTheme/GrantleeThemeManager>
#include "globalsettings_base.h"
#include <KToggleAction>
#include <KActionCollection>
#include <QStandardPaths>

using namespace MessageViewer;
DefaultGrantleeHeaderStyleInterface::DefaultGrantleeHeaderStyleInterface(
    MessageViewer::HeaderStylePlugin *plugin, QObject *parent)
    : MessageViewer::HeaderStyleInterface(plugin, parent)
{
}

DefaultGrantleeHeaderStyleInterface::~DefaultGrantleeHeaderStyleInterface()
{
}

void DefaultGrantleeHeaderStyleInterface::createAction(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac)
{
    const QStringList defaultThemePath = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation, QStringLiteral(
            "messageviewer/defaultthemes/"), QStandardPaths::LocateDirectory);
    if (!defaultThemePath.isEmpty()) {
        const QString themeName = DefaultGrantleeHeaderStylePluginSettings::self()->themeName();
        mDefaultTheme
            = GrantleeTheme::ThemeManager::loadTheme(defaultThemePath.at(0) + QLatin1Char(
                                                         '/') + themeName,
                                                     themeName,
                                                     QStringLiteral(
                                                         "kmail_default.desktop"));
    }
    mHeaderStylePlugin->headerStyle()->setTheme(mDefaultTheme);
    KToggleAction *act = new KToggleAction(mDefaultTheme.name(), this);
    ac->addAction(QStringLiteral("default_grantlee_theme"), act);
    connect(act, &KToggleAction::triggered, this,
            &DefaultGrantleeHeaderStyleInterface::slotDefaultGrantleeHeaders);
    mAction.append(act);
    addActionToMenu(menu, actionGroup);
}

void DefaultGrantleeHeaderStyleInterface::activateAction()
{
    mAction.at(0)->setChecked(true);
}

void DefaultGrantleeHeaderStyleInterface::slotDefaultGrantleeHeaders()
{
    mHeaderStylePlugin->headerStyle()->setTheme(mDefaultTheme);
    slotStyleChanged();
}
