/*
   Copyright (c) 2015-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "headerstyle.h"
#include "headerstyleinterface.h"
#include "headerstyleplugin.h"

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
    act->setStatusTip(text);
    act->setToolTip(text);
    if (act->whatsThis().isEmpty()) {
        act->setWhatsThis(text);
    }
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

void MessageViewer::HeaderStyleInterface::slotStyleChanged()
{
    Q_EMIT styleChanged(mHeaderStylePlugin);
}
