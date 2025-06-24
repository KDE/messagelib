/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "defaultgrantleeheaderstyleinterface.h"
using namespace Qt::Literals::StringLiterals;

#include "globalsettings_base.h"
#include "messageviewer/headerstyle.h"
#include "messageviewer/headerstyleplugin.h"
#include <GrantleeTheme/GrantleeThemeManager>
#include <KActionCollection>
#include <KToggleAction>
#include <QStandardPaths>

using namespace MessageViewer;
DefaultGrantleeHeaderStyleInterface::DefaultGrantleeHeaderStyleInterface(MessageViewer::HeaderStylePlugin *plugin, QObject *parent)
    : MessageViewer::HeaderStyleInterface(plugin, parent)
{
}

DefaultGrantleeHeaderStyleInterface::~DefaultGrantleeHeaderStyleInterface() = default;

void DefaultGrantleeHeaderStyleInterface::createAction(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac)
{
    const QStringList defaultThemePath =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, u"messageviewer/defaultthemes/"_s, QStandardPaths::LocateDirectory);
    if (!defaultThemePath.isEmpty()) {
        const QString themeName = DefaultGrantleeHeaderStylePluginSettings::self()->themeName();
        mDefaultTheme = GrantleeTheme::ThemeManager::loadTheme(defaultThemePath.at(0) + u'/' + themeName, themeName, u"kmail_default.desktop"_s);
    }
    mHeaderStylePlugin->headerStyle()->setTheme(mDefaultTheme);
    auto act = new KToggleAction(mDefaultTheme.name(), this);
    ac->addAction(u"default_grantlee_theme"_s, act);
    connect(act, &KToggleAction::triggered, this, &DefaultGrantleeHeaderStyleInterface::slotDefaultGrantleeHeaders);
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

#include "moc_defaultgrantleeheaderstyleinterface.cpp"
