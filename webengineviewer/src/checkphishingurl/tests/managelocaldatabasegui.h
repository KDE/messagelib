/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
class QPlainTextEdit;
namespace WebEngineViewer
{
class LocalDataBaseManager;
}

class ManageLocalDataBaseGui : public QWidget
{
    Q_OBJECT
public:
    explicit ManageLocalDataBaseGui(QWidget *parent = nullptr);
    ~ManageLocalDataBaseGui();
private Q_SLOTS:
    void slotDownloadFullDatabase();

private:
    QPlainTextEdit *mResult;
    WebEngineViewer::LocalDataBaseManager *mDbManager;
};

