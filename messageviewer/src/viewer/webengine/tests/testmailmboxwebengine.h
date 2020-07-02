/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTMAILMBOXWEBENGINE_H
#define TESTMAILMBOXWEBENGINE_H

#include <QWidget>
#include <KMime/Message>

class TestMailMBoxWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailMBoxWebEngine(QWidget *parent = nullptr);
    ~TestMailMBoxWebEngine();
private:
    KMime::Message::Ptr readAndParseMail(const QString &mailFile);
};

#endif // TESTMAILMBOXWEBENGINE_H
