/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "managelocaldatabasegui.h"
#include <QStandardPaths>
#include "../localdatabasemanager.h"

#include <QApplication>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QVBoxLayout>

ManageLocalDataBaseGui::ManageLocalDataBaseGui(QWidget *parent)
    : QWidget(parent)
    , mDbManager(new WebEngineViewer::LocalDataBaseManager(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    mResult = new QPlainTextEdit(this);
    mResult->setReadOnly(true);
    layout->addWidget(mResult);

    QPushButton *button = new QPushButton(QStringLiteral("Create Database"), this);
    connect(button, &QPushButton::clicked, this, &ManageLocalDataBaseGui::slotDownloadFullDatabase);
    layout->addWidget(button);
}

ManageLocalDataBaseGui::~ManageLocalDataBaseGui()
{
}

void ManageLocalDataBaseGui::slotDownloadFullDatabase()
{
    mDbManager->initialize();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    ManageLocalDataBaseGui *w = new ManageLocalDataBaseGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}
