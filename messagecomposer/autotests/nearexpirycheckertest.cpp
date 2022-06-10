/*
  SPDX-FileCopyrightText: 2022 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include "../src/composer/nearexpirychecker_p.h"
#include <MessageComposer/NearExpiryChecker>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>
#include <gpgme++/keylistresult.h>

#include <QDebug>
#include <QProcess>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace MessageComposer;

void copyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
        return;

    for (QString d : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        QVERIFY(dir.mkpath(dst_path));
        copyPath(src + QDir::separator() + d, dst_path);
    }

    for (QString f : dir.entryList(QDir::Files)) {
        QFile ff(dst + QDir::separator() + f);
        if (ff.exists()) {
            ff.remove();
        }
        QVERIFY(QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f));
    }
}

class NearExpiryCheckerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        Test::setupFullEnv();
        qRegisterMetaType<GpgME::Key>();
        qRegisterMetaType<NearExpiryChecker::ExpiryInformation>();

        gnupgDir = std::unique_ptr<QTemporaryDir>(new QTemporaryDir());
        copyPath(QStringLiteral(GNUPGHOME), gnupgDir->path());
        qputenv("GNUPGHOME", gnupgDir->path().toUtf8());
    }

    void cleanupTestCase()
    {
        QProcess p;
        p.start(QStringLiteral("gpg-connect-agent"), QStringList() << QStringLiteral("KILLAGENT") << QStringLiteral("/bye"));
        QVERIFY(p.waitForFinished());
        if (p.exitCode()) {
            qDebug() << p.readAllStandardError() << p.readAllStandardOutput();
            QVERIFY(p.exitCode());
        }
    }

    void valid_data()
    {
        QTest::addColumn<GpgME::Key>("key");
        QTest::addColumn<int>("difftime");
        QTest::newRow("neverExpire") << Test::getKeys()[0] << -1;

        const auto backend = QGpgME::openpgp();
        Q_ASSERT(backend);
        const auto job = backend->keyListJob(false);
        Q_ASSERT(job);

        std::vector<GpgME::Key> keys;
        job->exec(QStringList() << QStringLiteral("EB85BB5FA33A75E15E944E63F231550C4F47E38E"), false, keys);
        QTest::newRow("openpgp") << keys[0] << 2 * 24 * 60 * 60;
        QTest::newRow("smime") << Test::getKeys(true)[0] << 2 * 24 * 60 * 60;
    }

    void valid()
    {
        QFETCH(GpgME::Key, key);
        QFETCH(int, difftime);

        NearExpiryChecker checker(1, 1, 1, 1, 1, 1);
        QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
        checker.d->testMode = true;
        checker.d->difftime = difftime;

        checker.checkKey(key);
        QCOMPARE(spy.count(), 0);
    }

    void expired_data()
    {
        QTest::addColumn<GpgME::Key>("key");
        QTest::addColumn<QString>("msg");
        QTest::addColumn<QString>("msgOwnKey");
        QTest::addColumn<QString>("msgOwnSigningKey");

        const auto backend = QGpgME::openpgp();
        Q_ASSERT(backend);
        const auto job = backend->keyListJob(false);
        Q_ASSERT(job);

        std::vector<GpgME::Key> keys;
        job->exec(QStringList() << QStringLiteral("EB85BB5FA33A75E15E944E63F231550C4F47E38E"), false, keys);
        QTest::newRow("openpgp")
            << keys[0]
            << QStringLiteral(
                   "<p>The OpenPGP key for</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expired less than a day ago.</p>")
            << QStringLiteral(
                   "<p>Your OpenPGP encryption key</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expired less than a day "
                   "ago.</p>")
            << QStringLiteral(
                   "<p>Your OpenPGP signing key</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expired less than a day "
                   "ago.</p>");
        QTest::newRow("smime") << Test::getKeys(true)[0]
                               << QStringLiteral(
                                      "<p>The S/MIME certificate for</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> (serial "
                                      "number 00D345203A186385C9)</p><p>expired less than a day ago.</p>")
                               << QStringLiteral(
                                      "<p>Your S/MIME encryption certificate</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> "
                                      "(serial number 00D345203A186385C9)</p><p>expired less than a day ago.</p>")
                               << QStringLiteral(
                                      "<p>Your S/MIME signing certificate</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> "
                                      "(serial number 00D345203A186385C9)</p><p>expired less than a day ago.</p>");
    }

    void expired()
    {
        QFETCH(GpgME::Key, key);
        QFETCH(QString, msg);
        QFETCH(QString, msgOwnKey);
        QFETCH(QString, msgOwnSigningKey);

        NearExpiryChecker checker(1, 1, 1, 1, 1, 1);
        checker.d->testMode = true;
        checker.d->difftime = -1;
        {
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msg);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OtherKeyExpired);
        }
        checker.d->alreadyWarnedFingerprints.clear();
        {
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkOwnKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msgOwnKey);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OwnKeyExpired);
        }
        checker.d->alreadyWarnedFingerprints.clear();
        {
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkOwnSigningKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msgOwnSigningKey);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OwnKeyExpired);
        }
    }

    void nearexpiry_data()
    {
        QTest::addColumn<GpgME::Key>("key");
        QTest::addColumn<QString>("msg");
        QTest::addColumn<QString>("msgOwnKey");
        QTest::addColumn<QString>("msgOwnSigningKey");

        const auto backend = QGpgME::openpgp();
        Q_ASSERT(backend);
        const auto job = backend->keyListJob(false);
        Q_ASSERT(job);

        std::vector<GpgME::Key> keys;
        job->exec(QStringList() << QStringLiteral("EB85BB5FA33A75E15E944E63F231550C4F47E38E"), false, keys);
        QTest::newRow("openpgp")
            << keys[0]
            << QStringLiteral(
                   "<p>The OpenPGP key for</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expires in less than 6 days.</p>")
            << QStringLiteral(
                   "<p>Your OpenPGP encryption key</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expires in less than 6 "
                   "days.</p>")
            << QStringLiteral(
                   "<p>Your OpenPGP signing key</p><p align=center><b>alice@autocrypt.example</b> (KeyID 0xF231550C4F47E38E)</p><p>expires in less than 6 "
                   "days.</p>");
        QTest::newRow("smime") << Test::getKeys(true)[0]
                               << QStringLiteral(
                                      "<p>The S/MIME certificate for</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> (serial "
                                      "number 00D345203A186385C9);</p><p>expires in less than 6 days.</p>")
                               << QStringLiteral(
                                      "<p>Your S/MIME encryption certificate</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> "
                                      "(serial number 00D345203A186385C9);</p><p>expires in less than 6 days.</p>")
                               << QStringLiteral(
                                      "<p>Your S/MIME signing certificate</p><p align=center><b>CN=unittest cert,EMAIL=test@example.com,O=KDAB,C=US</b> "
                                      "(serial number 00D345203A186385C9);</p><p>expires in less than 6 days.</p>");
    }

    void nearexpiry()
    {
        QFETCH(GpgME::Key, key);
        QFETCH(QString, msg);
        QFETCH(QString, msgOwnKey);
        QFETCH(QString, msgOwnSigningKey);

        {
            NearExpiryChecker checker(10, 1, 1, 1, 1, 1);
            checker.d->testMode = true;
            checker.d->difftime = 5 * 24 * 3600; // 5 days
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msg);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OtherKeyNearExpiry);
        }
        {
            NearExpiryChecker checker(10, 1, 1, 1, 1, 1);
            checker.d->testMode = true;
            checker.d->difftime = 5 * 24 * 3600; // 5 days
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkOwnKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msgOwnKey);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OwnKeyNearExpiry);
        }
        {
            NearExpiryChecker checker(1, 10, 1, 1, 1, 1);
            checker.d->testMode = true;
            checker.d->difftime = 5 * 24 * 3600; // 5 days
            QSignalSpy spy(&checker, &NearExpiryChecker::expiryMessage);
            checker.checkOwnSigningKey(key);
            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments.at(0).value<GpgME::Key>().keyID(), key.keyID());
            QCOMPARE(arguments.at(1).toString(), msgOwnSigningKey);
            QCOMPARE(arguments.at(2).value<NearExpiryChecker::ExpiryInformation>(), NearExpiryChecker::OwnKeyNearExpiry);
        }
    }

private:
    std::unique_ptr<QTemporaryDir> gnupgDir;
};
QTEST_MAIN(NearExpiryCheckerTest)
#include "nearexpirycheckertest.moc"
