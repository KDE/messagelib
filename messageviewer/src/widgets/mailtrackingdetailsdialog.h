/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRACKINGDETAILSDIALOG_H
#define MAILTRACKINGDETAILSDIALOG_H

#include <QDialog>
#include "messageviewer_private_export.h"

namespace KPIMTextEdit {
class RichTextEditorWidget;
}

namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT MailTrackingDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MailTrackingDetailsDialog(QWidget *parent = nullptr);
    ~MailTrackingDetailsDialog();

    void setDetails(const QString &details);
private:
    void writeConfig();
    void readConfig();

    KPIMTextEdit::RichTextEditorWidget *mDetails = nullptr;
};
}

#endif // MAILTRACKINGDETAILSDIALOG_H
