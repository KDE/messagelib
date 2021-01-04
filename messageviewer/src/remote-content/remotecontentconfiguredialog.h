/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREDIALOG_H
#define REMOTECONTENTCONFIGUREDIALOG_H

#include <QDialog>
#include "messageviewer_private_export.h"
namespace MessageViewer {
class RemoteContentConfigureWidget;
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentConfigureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureDialog(QWidget *parent = nullptr);
    ~RemoteContentConfigureDialog() override;

private:
    void readConfig();
    void writeConfig();
    void slotAccept();
    RemoteContentConfigureWidget *const mRemoteContentConfigureWidget;
};
}

#endif // REMOTECONTENTCONFIGUREDIALOG_H
