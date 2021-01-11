/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTDIALOG_H
#define REMOTECONTENTDIALOG_H

#include <QDialog>

#include "messageviewer_private_export.h"
#include "remote-content/remotecontentinfo.h"
namespace MessageViewer {
class RemoteContentWidget;
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoteContentDialog(QWidget *parent = nullptr);
    ~RemoteContentDialog() override;

    Q_REQUIRED_RESULT RemoteContentInfo info() const;

private:
    RemoteContentWidget *const mRemoveContentWidget;
};
}
#endif // REMOTECONTENTDIALOG_H
