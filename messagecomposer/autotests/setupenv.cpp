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

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTest>

using namespace MessageComposer;

void Test::setupEnv()
{
    qputenv("LC_ALL", "C");
    qputenv("KDEHOME", QFile::encodeName(QDir::homePath() + QLatin1String("/.qttest")).constData());
    QStandardPaths::setTestModeEnabled(true);
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

KMime::Message::Ptr Test::loadMessageFromFile(const QString &filename)
{
    QFile file(QLatin1String(QByteArray(MAIL_DATA_DIR "/" + filename.toLatin1())));
    const bool opened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(opened);
    Q_UNUSED(opened)
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
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
