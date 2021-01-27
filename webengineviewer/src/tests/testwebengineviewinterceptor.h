/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTWEBENGINEVIEWINTERCEPTOR_H
#define TESTWEBENGINEVIEWINTERCEPTOR_H

#include <QWidget>
class QWebEngineView;
class TestWebEngineViewInterceptor : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineViewInterceptor(QWidget *parent = nullptr);

private:
    QWebEngineView *mWebEngineView = nullptr;
};

#endif // TESTWEBENGINEVIEWINTERCEPTOR_H
