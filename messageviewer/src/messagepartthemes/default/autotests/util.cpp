/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "util.h"
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QTest>
using namespace MessageViewer;
KMime::Message::Ptr Test::readAndParseMail(const QString &mailFile)
{
    QFile file(QStringLiteral(MAIL_DATA_DIR) + QLatin1Char('/') + mailFile);
    const bool openFile = file.open(QIODevice::ReadOnly);
    Q_ASSERT(openFile);
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}

void Test::compareFile(const QString &outFile, const QString &referenceFile)
{
    QVERIFY(QFile::exists(outFile));

    const QString htmlFile = outFile + QStringLiteral(".html");
    // remove tailing newlines and spaces and make htmlmore uniform (breaks pre tags)
    {
        QFile f(outFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegularExpression(QStringLiteral("[\t ]+")), QStringLiteral(" "));
        content.replace(QRegularExpression(QStringLiteral("[\t ]*\n+[\t ]*")), QStringLiteral("\n"));
        content.replace(QRegularExpression(QStringLiteral("([\n\t ])\\1+")), QStringLiteral("\\1"));
        content.replace(QRegularExpression(QStringLiteral(">\n+[\t ]*")), QStringLiteral(">"));
        content.replace(QRegularExpression(QStringLiteral("[\t ]*\n+[\t ]*<")), QStringLiteral("<"));
        content.replace(QLatin1StringView("&nbsp;"), QLatin1StringView("NBSP_ENTITY_PLACEHOLDER")); // xmlling chokes on &nbsp;
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    // validate xml and pretty-print for comparison
    // TODO add proper cmake check for xmllint and diff
    QStringList args = QStringList() << QStringLiteral("--format") << QStringLiteral("--encode") << QStringLiteral("UTF8") << QStringLiteral("--output")
                                     << htmlFile << outFile;
    QCOMPARE(QProcess::execute(QStringLiteral("xmllint"), args), 0);

    // get rid of system dependent or random paths
    {
        QFile f(htmlFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegularExpression(QStringLiteral("\"file:[^\"]*[/(?:%2F)]([^\"/(?:%2F)]*)\"")), QStringLiteral("\"file:\\1\""));
        content.replace(QRegularExpression(QStringLiteral("\"qrc:[^\"]*[/(?:%2F)]([^\"/(?:%2F)]*)\"")), QStringLiteral("\"file:\\1\""));
        content.replace(QRegularExpression(QStringLiteral("(file:///tmp/messageviewer)(_[^\"]+)(\\.index\\.[^\"]*)")), QStringLiteral("\\1\\3"));
        content.replace(QLatin1StringView("NBSP_ENTITY_PLACEHOLDER"), QLatin1StringView("&nbsp;")); // undo above transformation for xmllint
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    QProcess proc;
#ifdef _WIN32
    args = QStringList() << QStringLiteral("Compare-Object") << QString(QStringLiteral("(Get-Content %1)")).arg(referenceFile)
                         << QString(QStringLiteral("(Get-Content %1)")).arg(htmlFile);

    proc.start(QStringLiteral("powershell"), args);
    QVERIFY(proc.waitForFinished());

    auto pStdOut = proc.readAllStandardOutput();
    if (pStdOut.size()) {
        qDebug() << "Files are different, diff output message:\n" << pStdOut.toStdString().c_str();
    }

    QCOMPARE(pStdOut.size(), 0);
#else
    // compare to reference file
    args = QStringList() << QStringLiteral("-u") << referenceFile << htmlFile;

    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());
    QCOMPARE(proc.exitCode(), 0);
#endif
}
