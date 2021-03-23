/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "../searchfullhashjob.h"
#include <QWidget>
class QLineEdit;
class QPlainTextEdit;
class SearchFullHashGui : public QWidget
{
    Q_OBJECT
public:
    explicit SearchFullHashGui(QWidget *parent = nullptr);
    ~SearchFullHashGui();
private Q_SLOTS:
    void slotCheckUrl();
    void slotGetResult(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus result);
    void slotJSonDebug(const QByteArray &debug);

private:
    QLineEdit *mCheckHashLineEdit;
    QLineEdit *mDataBaseHashLineEdit;
    QPlainTextEdit *mJson;
    QPlainTextEdit *mResult;
};

