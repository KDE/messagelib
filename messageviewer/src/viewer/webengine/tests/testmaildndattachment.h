/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
#include <QWidget>

class TestMailDndAttachment : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailDndAttachment(QWidget *parent = nullptr);
    ~TestMailDndAttachment() override;

private:
    std::shared_ptr<KMime::Message> readAndParseMail(const QString &mailFile);
};
