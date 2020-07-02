/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SEARCHFULLHASHGUI_H
#define SEARCHFULLHASHGUI_H

#include <QWidget>
#include "../searchfullhashjob.h"
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

#endif // SEARCHFULLHASHGUI_H
