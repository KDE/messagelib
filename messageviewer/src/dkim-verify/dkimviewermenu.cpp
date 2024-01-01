/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimviewermenu.h"
#include <KLocalizedString>
#include <QMenu>

using namespace MessageViewer;
DKIMViewerMenu::DKIMViewerMenu(QObject *parent)
    : QObject(parent)
    , mMenu(new QMenu(i18n("DKIM")))
{
    initialize();
}

DKIMViewerMenu::~DKIMViewerMenu()
{
    delete mMenu;
}

QMenu *DKIMViewerMenu::menu() const
{
    return mMenu;
}

void DKIMViewerMenu::setEnableUpdateDkimKeyMenu(bool enabled)
{
    mUpdateDKIMKey->setEnabled(enabled);
}

void DKIMViewerMenu::initialize()
{
    QAction *checkSignature = mMenu->addAction(i18n("Recheck DKIM Signature"));
    connect(checkSignature, &QAction::triggered, this, &DKIMViewerMenu::recheckSignature);

    mUpdateDKIMKey = mMenu->addAction(i18n("Update DKIM key"));
    connect(mUpdateDKIMKey, &QAction::triggered, this, &DKIMViewerMenu::updateDkimKey);

    mMenu->addSeparator();
    QAction *manageDKIMRuleDKIMKey = mMenu->addAction(i18n("Rules..."));
    connect(manageDKIMRuleDKIMKey, &QAction::triggered, this, &DKIMViewerMenu::showDkimRules);
}

#include "moc_dkimviewermenu.cpp"
