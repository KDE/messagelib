/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QDialog>
class QWebEnginePage;
namespace WebEngineViewer
{
class DeveloperToolWidget;
class WEBENGINEVIEWER_EXPORT DeveloperToolDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeveloperToolDialog(QWidget *parent = nullptr);
    ~DeveloperToolDialog() override;
    [[nodiscard]] QWebEnginePage *enginePage() const;

private:
    WEBENGINEVIEWER_NO_EXPORT void readConfig();
    WEBENGINEVIEWER_NO_EXPORT void writeConfig();
    DeveloperToolWidget *const mDeveloperToolWidget;
};
}
