/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_private_export.h"
#include <QWidget>
class QWebEnginePage;
class QWebEngineView;
namespace WebEngineViewer
{
class WEBENGINEVIEWER_TESTS_EXPORT DeveloperToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeveloperToolWidget(QWidget *parent = nullptr);
    ~DeveloperToolWidget() override;
    [[nodiscard]] QWebEnginePage *enginePage() const;

Q_SIGNALS:
    void closeRequested();

private:
    QWebEngineView *const mWebEngineView;
    QWebEnginePage *const mEnginePage;
};
}
