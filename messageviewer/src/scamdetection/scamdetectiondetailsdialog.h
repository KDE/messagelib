/*
  SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef SCAMDETECTIONDETAILSDIALOG_H
#define SCAMDETECTIONDETAILSDIALOG_H

#include <QDialog>

namespace KPIMTextEdit {
class RichTextEditorWidget;
}

namespace MessageViewer {
class ScamDetectionDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScamDetectionDetailsDialog(QWidget *parent = nullptr);
    ~ScamDetectionDetailsDialog();

    void setDetails(const QString &details);

private:
    void slotSaveAs();
    void writeConfig();
    void readConfig();
    KPIMTextEdit::RichTextEditorWidget *mDetails = nullptr;
};
}

#endif // SCAMDETECTIONDETAILSDIALOG_H
