/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKSIGNATUREJOBTEST_H
#define DKIMCHECKSIGNATUREJOBTEST_H

#include <QObject>
#include <QtCrypto>
class DKIMCheckSignatureJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckSignatureJobTest(QObject *parent = nullptr);
    ~DKIMCheckSignatureJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestMail();
    void shouldTestMail_data();
    void initTestCase();
    void cleanupTestCase();

private:
    QCA::Initializer *mQcaInitializer = nullptr;
};

#endif // DKIMCHECKSIGNATUREJOBTEST_H
