/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEVELOPERTOOLWIDGET_H
#define DEVELOPERTOOLWIDGET_H

#include <QWidget>
#include "messageviewer_private_export.h"
class QWebEnginePage;
class QWebEngineView;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DeveloperToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeveloperToolWidget(QWidget *parent = nullptr);
    ~DeveloperToolWidget() override;
    QWebEnginePage *enginePage() const;

private:
    QWebEngineView *mWebEngineView = nullptr;
    QWebEnginePage *mEnginePage = nullptr;
};
}

#endif // DEVELOPERTOOLWIDGET_H
