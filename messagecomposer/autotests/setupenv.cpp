/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupenv.h"

#include <QGpgME/ExportJob>
#include <QGpgME/ImportJob>
#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>
#include <gpgme++/context.h>
#include <gpgme++/importresult.h>
#include <gpgme++/keylistresult.h>

#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTest>

using namespace MessageComposer;

void Test::setupEnv()
{
    QStandardPaths::setTestModeEnabled(true);
}

void Test::setupFullEnv()
{
    setupEnv();
    qputenv("LC_ALL", "en_US.UTF-8");
    qputenv("TZ", "UTC");
}

std::vector<GpgME::Key, std::allocator<GpgME::Key>> Test::getKeys(bool smime)
{
    QGpgME::KeyListJob *job = nullptr;

    if (smime) {
        const QGpgME::Protocol *const backend = QGpgME::smime();
        Q_ASSERT(backend);
        job = backend->keyListJob(false);
    } else {
        const QGpgME::Protocol *const backend = QGpgME::openpgp();
        Q_ASSERT(backend);
        job = backend->keyListJob(false);
    }
    Q_ASSERT(job);

    std::vector<GpgME::Key> keys;
    GpgME::KeyListResult res = job->exec(QStringList(), true, keys);

    if (!smime) {
        Q_ASSERT(keys.size() == 3);
    }

    Q_ASSERT(!res.error());

    /*
    qDebug() << "got private keys:" << keys.size();

    for (std::vector< GpgME::Key >::iterator i = keys.begin(); i != keys.end(); ++i) {
        qDebug() << "key isnull:" << i->isNull() << "isexpired:" << i->isExpired();
        qDebug() << "key numuserIds:" << i->numUserIDs();
        for (uint k = 0; k < i->numUserIDs(); ++k) {
            qDebug() << "userIDs:" << i->userID(k).email();
        }
    }
    */

    return keys;
}

KMime::Message::Ptr Test::loadMessage(const QString &filename)
{
    QFile mailFile(filename);
    Q_ASSERT(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    Q_ASSERT(!mailData.isEmpty());

    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();
    return origMsg;
}

KMime::Message::Ptr Test::loadMessageFromDataDir(const QString &filename)
{
    return loadMessage(QLatin1StringView(QByteArray(MAIL_DATA_DIR "/" + filename.toLatin1())));
}

void Test::compareFile(KMime::Content *content, const QString &referenceFile)
{
    QFileInfo fi(referenceFile);
    const QString actualFile = fi.fileName();
    QFile f(actualFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    const QByteArray encodedContent(content->encodedContent());
    f.write(encodedContent);
    if (!encodedContent.endsWith('\n')) {
        f.write("\n");
    }
    f.close();
    compareFile(actualFile, referenceFile);
}

void Test::compareFile(const QString &outFile, const QString &referenceFile)
{
    QVERIFY(QFile::exists(outFile));

    // compare to reference file
    const auto args = QStringList() << QStringLiteral("-u") << referenceFile << outFile;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}

void Test::populateKeyring(const QString &gnupgHome, const GpgME::Key &key)
{
    QEventLoop loop;
    const QGpgME::Protocol *proto(QGpgME::openpgp());
    auto exportJob = proto->publicKeyExportJob(false);
    QObject::connect(
        exportJob,
        &QGpgME::ExportJob::result,
        exportJob,
        [&gnupgHome, &loop](const GpgME::Error &result, const QByteArray &keyData, const QString &auditLogAsHtml, const GpgME::Error &auditLogError) {
            Q_UNUSED(auditLogAsHtml);
            Q_UNUSED(auditLogError);
            loop.quit();
            QVERIFY(!result);
            populateKeyring(gnupgHome, keyData);
        });
    QStringList patterns = {QString::fromUtf8(key.primaryFingerprint())};
    exportJob->start(patterns);
    loop.exec();
}

void Test::populateKeyring(const QString &gnupgHome, const QByteArray &keydata)
{
    const QGpgME::Protocol *proto(QGpgME::openpgp());
    auto importJob = proto->importJob();
    QGpgME::Job::context(importJob)->setEngineHomeDirectory(gnupgHome.toUtf8().constData());
    const auto result = importJob->exec(keydata);
    importJob->deleteLater();
}
