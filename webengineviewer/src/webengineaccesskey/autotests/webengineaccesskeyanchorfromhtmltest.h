/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>
#include <QWidget>

#include "../webengineaccesskeyanchor.h"
class QWebEngineView;
class TestWebEngineAccessKey : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineAccessKey(QWidget *parent = nullptr);
    ~TestWebEngineAccessKey() override;

    void setHtml(const QString &html);
private Q_SLOTS:
    void loadFinished(bool b);

    void handleSearchAccessKey(const QVariant &var);
Q_SIGNALS:
    void accessKeySearchFinished(const QList<WebEngineViewer::WebEngineAccessKeyAnchor> &var);

private:
    QWebEngineView *mEngineView = nullptr;
};

class WebEngineAccessKeyAnchorFromHtmlTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineAccessKeyAnchorFromHtmlTest(QObject *parent = nullptr);
private Q_SLOTS:
    void shouldNotShowAccessKeyWhenHtmlAsNotAnchor();
    void shouldReturnOneAnchor();
    void shouldReturnTwoAnchor();
};
