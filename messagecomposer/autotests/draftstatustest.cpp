/*
   SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <MessageComposer/DraftStatus>

#include <QObject>
#include <QTest>

using namespace MessageComposer;

class DraftStatusTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void noState()
    {
        KMime::Message::Ptr msg(new KMime::Message);
        {
            DraftEncryptionState encryptState(msg);
            QCOMPARE(encryptState.isDefined(), false);
            QCOMPARE(encryptState.encryptionState(), false);
        }
        {
            DraftSignatureState signState(msg);
            QCOMPARE(signState.isDefined(), false);
            QCOMPARE(signState.signState(), false);
        }
        {
            DraftCryptoMessageFormatState formatState(msg);
            QCOMPARE(formatState.isDefined(), false);
            QCOMPARE(formatState.cryptoMessageFormatState(), Kleo::AutoFormat);
        }
    }

    void setRemoveState()
    {
        KMime::Message::Ptr msg(new KMime::Message);
        {
            DraftEncryptionState encryptState(msg);
            encryptState.setState(true);
            QCOMPARE(encryptState.isDefined(), true);
            QCOMPARE(encryptState.encryptionState(), true);
            encryptState.removeState();
            QCOMPARE(encryptState.isDefined(), false);
            QCOMPARE(encryptState.encryptionState(), false);
        }
        {
            DraftSignatureState signState(msg);
            signState.setState(true);
            QCOMPARE(signState.isDefined(), true);
            QCOMPARE(signState.signState(), true);
            signState.removeState();
            QCOMPARE(signState.isDefined(), false);
            QCOMPARE(signState.signState(), false);
        }
        {
            DraftCryptoMessageFormatState formatState(msg);
            formatState.setState(Kleo::AnyOpenPGP);
            QCOMPARE(formatState.isDefined(), true);
            QCOMPARE(formatState.cryptoMessageFormatState(), Kleo::AnyOpenPGP);
            formatState.removeState();
            QCOMPARE(formatState.isDefined(), false);
            QCOMPARE(formatState.cryptoMessageFormatState(), Kleo::AutoFormat);
        }
    }

    void testRemoveDraftCryptoHeaders()
    {
        KMime::Message::Ptr msg(new KMime::Message);
        DraftEncryptionState encryptState(msg);
        DraftSignatureState signState(msg);
        DraftCryptoMessageFormatState formatState(msg);
        encryptState.setState(true);
        signState.setState(true);
        formatState.setState(Kleo::AnyOpenPGP);

        removeDraftCryptoHeaders(msg);

        QCOMPARE(encryptState.isDefined(), false);
        QCOMPARE(signState.isDefined(), false);
        QCOMPARE(formatState.isDefined(), false);
    }
};
QTEST_MAIN(DraftStatusTest)
#include "draftstatustest.moc"
