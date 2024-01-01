/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QtCrypto>
class DKIMCheckSignatureJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckSignatureJobTest(QObject *parent = nullptr);
    ~DKIMCheckSignatureJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestMail();
    void shouldTestMail_data();
    void initTestCase();
    void cleanupTestCase();

private:
    QCA::Initializer *mQcaInitializer = nullptr;
};
