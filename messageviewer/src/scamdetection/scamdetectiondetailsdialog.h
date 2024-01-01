/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QDialog>

namespace TextCustomEditor
{
class RichTextEditorWidget;
}

namespace MessageViewer
{
class ScamDetectionDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScamDetectionDetailsDialog(QWidget *parent = nullptr);
    ~ScamDetectionDetailsDialog() override;

    void setDetails(const QString &details);

private:
    void slotSaveAs();
    void writeConfig();
    void readConfig();
    TextCustomEditor::RichTextEditorWidget *const mDetails;
};
}
