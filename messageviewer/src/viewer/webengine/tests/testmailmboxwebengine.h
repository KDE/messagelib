/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
#include <QWidget>

class TestMailMBoxWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailMBoxWebEngine(QWidget *parent = nullptr);
    ~TestMailMBoxWebEngine();

private:
    KMime::Message::Ptr readAndParseMail(const QString &mailFile);
};

