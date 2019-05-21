/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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
