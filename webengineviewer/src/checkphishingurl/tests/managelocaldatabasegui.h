/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MANAGELOCALDATABASEGUI_H
#define MANAGELOCALDATABASEGUI_H

#include <QWidget>
class QPlainTextEdit;
namespace WebEngineViewer {
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

#endif // MANAGELOCALDATABASEGUI_H
