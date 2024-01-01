/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "contactpreferencetest.h"
#include "contactpreference/contactpreference.h"
#include <QTest>
QTEST_GUILESS_MAIN(ContactPreferenceTest)
ContactPreferenceTest::ContactPreferenceTest(QObject *parent)
    : QObject{parent}
{
}

void ContactPreferenceTest::shouldHaveDefaultValues()
{
    MessageComposer::ContactPreference w;
    QCOMPARE(w.cryptoMessageFormat, Kleo::AutoFormat);
    QCOMPARE(w.signingPreference, Kleo::UnknownSigningPreference);
    QCOMPARE(w.encryptionPreference, Kleo::UnknownPreference);
    QVERIFY(w.pgpKeyFingerprints.isEmpty());
    QVERIFY(w.smimeCertFingerprints.isEmpty());
}

#include "moc_contactpreferencetest.cpp"
