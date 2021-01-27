/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEVELOPERTOOLDIALOG_H
#define DEVELOPERTOOLDIALOG_H

#include "messageviewer_private_export.h"
#include <QDialog>
class QWebEnginePage;
namespace MessageViewer
{
class DeveloperToolWidget;
class MESSAGEVIEWER_TESTS_EXPORT DeveloperToolDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeveloperToolDialog(QWidget *parent = nullptr);
    ~DeveloperToolDialog() override;
    QWebEnginePage *enginePage() const;

private:
    void readConfig();
    void writeConfig();
    DeveloperToolWidget *mDeveloperToolWidget = nullptr;
};
}
#endif // DEVELOPERTOOLDIALOG_H
