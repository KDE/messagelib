/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "managelocaldatabasegui.h"
#include "../localdatabasemanager.h"
#include <QStandardPaths>

#include <QApplication>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

ManageLocalDataBaseGui::ManageLocalDataBaseGui(QWidget *parent)
    : QWidget(parent)
    , mDbManager(new WebEngineViewer::LocalDataBaseManager(this))
{
    auto layout = new QVBoxLayout(this);

    mResult = new QPlainTextEdit(this);
    mResult->setReadOnly(true);
    layout->addWidget(mResult);

    auto button = new QPushButton(QStringLiteral("Create Database"), this);
    connect(button, &QPushButton::clicked, this, &ManageLocalDataBaseGui::slotDownloadFullDatabase);
    layout->addWidget(button);
}

ManageLocalDataBaseGui::~ManageLocalDataBaseGui() = default;

void ManageLocalDataBaseGui::slotDownloadFullDatabase()
{
    mDbManager->initialize();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    auto w = new ManageLocalDataBaseGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_managelocaldatabasegui.cpp"
