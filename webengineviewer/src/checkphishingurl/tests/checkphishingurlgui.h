/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "../checkphishingurljob.h"
#include <QWidget>
class QLineEdit;
class QPlainTextEdit;
class CheckPhishingUrlGui : public QWidget
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlGui(QWidget *parent = nullptr);
    ~CheckPhishingUrlGui() override;
private Q_SLOTS:
    void slotCheckUrl();
    void slotGetResult(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus result, const QUrl &url, uint verifyCacheAfterThisTime);
    void slotJSonDebug(const QByteArray &debug);

private:
    QLineEdit *mCheckUrlLineEdit;
    QPlainTextEdit *mJson;
    QPlainTextEdit *mResult;
};
