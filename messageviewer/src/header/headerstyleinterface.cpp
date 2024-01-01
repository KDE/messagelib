/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstyleinterface.h"
#include "headerstyleplugin.h"
#include "utils/messageviewerutil_p.h"
#include <KActionMenu>
#include <KToggleAction>
#include <QAction>
#include <QActionGroup>
using namespace MessageViewer;

HeaderStyleInterface::HeaderStyleInterface(MessageViewer::HeaderStylePlugin *headerStylePlugin, QObject *parent)
    : QObject(parent)
    , mHeaderStylePlugin(headerStylePlugin)
{
}

HeaderStyleInterface::~HeaderStyleInterface() = default;

QList<KToggleAction *> HeaderStyleInterface::action() const
{
    return mAction;
}

void HeaderStyleInterface::addHelpTextAction(QAction *act, const QString &text)
{
    MessageViewer::Util::addHelpTextAction(act, text);
}

void HeaderStyleInterface::addActionToMenu(KActionMenu *menu, QActionGroup *actionGroup)
{
    for (KToggleAction *taction : std::as_const(mAction)) {
        menu->addAction(taction);
        actionGroup->addAction(taction);
    }
}

HeaderStylePlugin *HeaderStyleInterface::headerStylePlugin() const
{
    return mHeaderStylePlugin;
}

void HeaderStyleInterface::slotStyleChanged()
{
    Q_EMIT styleChanged(mHeaderStylePlugin);
}

#include "moc_headerstyleinterface.cpp"
