/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstyle.h"
#include "headerstyleinterface.h"
#include "headerstyleplugin.h"
#include "utils/messageviewerutil_p.h"
#include <KActionMenu>
#include <QAction>
#include <KToggleAction>
using namespace MessageViewer;

HeaderStyleInterface::HeaderStyleInterface(MessageViewer::HeaderStylePlugin *headerStylePlugin, QObject *parent)
    : QObject(parent)
    , mHeaderStylePlugin(headerStylePlugin)
{
}

HeaderStyleInterface::~HeaderStyleInterface()
{
}

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
    for (KToggleAction *taction : qAsConst(mAction)) {
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
