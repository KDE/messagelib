/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTMAILDNDATTACHMENT_H
#define TESTMAILDNDATTACHMENT_H

#include <QWidget>
#include <KMime/Message>

class TestMailDndAttachment : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailDndAttachment(QWidget *parent = nullptr);
    ~TestMailDndAttachment();
private:
    KMime::Message::Ptr readAndParseMail(const QString &mailFile);
};

#endif // TESTMAILDNDATTACHMENT_H
