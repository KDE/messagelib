/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

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
    /*! Constructs a DeveloperToolWidget with the given parent. */
    explicit DeveloperToolWidget(QWidget *parent = nullptr);
    /*! Destroys the DeveloperToolWidget. */
    ~DeveloperToolWidget() override;
    /*! Returns the underlying QWebEnginePage. */
    [[nodiscard]] QWebEnginePage *enginePage() const;

Q_SIGNALS:
    /*! Emitted when the tool is requested to close. */
    void closeRequested();

private:
    QWebEngineView *const mWebEngineView;
    QWebEnginePage *const mEnginePage;
};
}
