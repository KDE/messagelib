/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "util.h"
using namespace Qt::Literals::StringLiterals;

#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QTest>

using namespace MessageViewer;
KMime::Message::Ptr Test::readAndParseMail(const QString &mailFile)
{
    QFile file(QStringLiteral(MAIL_DATA_DIR) + u'/' + mailFile);
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

    const QString htmlFile = outFile + u".html"_s;
    // remove tailing newlines and spaces and make htmlmore uniform (breaks pre tags)
    {
        QFile f(outFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegularExpression(u"[\t ]+"_s), u" "_s);
        content.replace(QRegularExpression(u"[\t ]*\n+[\t ]*"_s), u"\n"_s);
        content.replace(QRegularExpression(u"([\n\t ])\\1+"_s), u"\\1"_s);
        content.replace(QRegularExpression(u">\n+[\t ]*"_s), u">"_s);
        content.replace(QRegularExpression(u"[\t ]*\n+[\t ]*<"_s), u"<"_s);
        content.replace(QLatin1StringView("&nbsp;"), QLatin1StringView("NBSP_ENTITY_PLACEHOLDER")); // xmlling chokes on &nbsp;
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    // validate xml and pretty-print for comparison
    // TODO add proper cmake check for xmllint and diff
    QStringList args = QStringList() << u"--format"_s << u"--encode"_s << QStringLiteral("UTF8") << QStringLiteral("--output") << htmlFile << outFile;
    QCOMPARE(QProcess::execute(u"xmllint"_s, args), 0);

    // get rid of system dependent or random paths
    {
        QFile f(htmlFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegularExpression(u"\"file:[^\"]*[/(?:%2F)]([^\"/(?:%2F)]*)\""_s), u"\"file:\\1\""_s);
        content.replace(QRegularExpression(u"src=\"/[^\"]*/([^\"/(?:%2F)]*)\""_s), u"src=\"file:\\1\""_s);
        content.replace(QRegularExpression(u"\"qrc:[^\"]*[/(?:%2F)]([^\"/(?:%2F)]*)\""_s), u"\"file:\\1\""_s);
        content.replace(QRegularExpression(u"(file:///tmp/messageviewer)(_[^\"]+)(\\.index\\.[^\"]*)"_s), u"\\1\\3"_s);
        content.replace(QLatin1StringView("NBSP_ENTITY_PLACEHOLDER"), QLatin1StringView("&nbsp;")); // undo above transformation for xmllint
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    QProcess proc;
#ifdef _WIN32
    args = QStringList() << u"Compare-Object"_s << QString(u"(Get-Content %1)"_s).arg(referenceFile) << QString(u"(Get-Content %1)"_s).arg(htmlFile);

    proc.start(u"powershell"_s, args);
    QVERIFY(proc.waitForFinished());

    auto pStdOut = proc.readAllStandardOutput();
    if (pStdOut.size()) {
        qDebug() << "Files are different, diff output message:\n" << pStdOut.toStdString().c_str();
    }

    QCOMPARE(pStdOut.size(), 0);
#else
    // Uncommment to update test data
    // proc.start(u"cp"_s, {htmlFile, referenceFile});
    // QVERIFY(proc.waitForFinished());

    // compare to reference file
    args = QStringList() << u"-u"_s << referenceFile << htmlFile;

    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(u"diff"_s, args);
    QVERIFY(proc.waitForFinished());
    QCOMPARE(proc.exitCode(), 0);
#endif
}
