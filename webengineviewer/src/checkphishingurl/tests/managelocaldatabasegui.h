/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
    ~ManageLocalDataBaseGui() override;
private Q_SLOTS:
    void slotDownloadFullDatabase();

private:
    QPlainTextEdit *mResult;
    WebEngineViewer::LocalDataBaseManager *mDbManager;
};
