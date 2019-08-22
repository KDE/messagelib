/*
   Copyright 2018 Sandro Knauß <sknauss@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "grantleeheaderformattertest.h"

#include <MessageViewer/GrantleeHeaderStyle>
#include "../grantleeheaderformatter.h"

#include <QFile>
#include <QProcess>
#include <QRegExp>
#include <QStringList>
#include <QStandardPaths>
#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(GrantleeHeaderFormatterTest)

void testHeaderFile(const QString &data, const QString &absolutePath, const QString &name)
{
    QString header = QStringLiteral("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
                                    "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                                    "<body>\n");
    header += data + QStringLiteral("</div>\n</div>");
    header += QStringLiteral("\n</body>\n</html>\n");

    header.replace(QStringLiteral("file://") + absolutePath, QStringLiteral("file://PATHTOSTYLE"));
    header.replace(QRegExp(QStringLiteral("[\t ]+")), QStringLiteral(" "));
    header.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*")), QStringLiteral("\n"));
    header.replace(QRegExp(QStringLiteral("([\n\t ])\\1+")), QStringLiteral("\\1"));
    header.replace(QRegExp(QStringLiteral(">\n+[\t ]*")), QStringLiteral(">"));
    header.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*<")), QStringLiteral("<"));
    header.replace(QLatin1String("&nbsp;"), QLatin1String("NBSP_ENTITY_PLACEHOLDER")); // xmlling chokes on &nbsp;

    QString outName = name + QStringLiteral(".out.html");
    QString fName = name + QStringLiteral(".html");

    QVERIFY(QFile(QStringLiteral(HEADER_DATA_DIR "/") + fName).exists());

    {
        QFile f(outName);
        f.open(QIODevice::WriteOnly);
        f.write(header.toUtf8());
        f.close();
    }
    // TODO add proper cmake check for xmllint and diff
    {
        const QStringList args = QStringList()
                                 << QStringLiteral("--format")
                                 << QStringLiteral("--encode") << QStringLiteral("UTF8")
                                 << QStringLiteral("--output") << fName
                                 << outName;
        QCOMPARE(QProcess::execute(QStringLiteral("xmllint"), args), 0);
    }

    {
        // compare to reference file
        const QStringList args = QStringList()
                                 << QStringLiteral("-u")
                                 << fName
                                 << QStringLiteral(HEADER_DATA_DIR "/") + fName;
        QProcess proc;
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
        proc.start(QStringLiteral("diff"), args);
        QVERIFY(proc.waitForFinished());

        QCOMPARE(proc.exitCode(), 0);
    }
}

KMime::Message::Ptr readAndParseMail(const QString &mailFile)
{
    QFile file(QStringLiteral(HEADER_DATA_DIR) + QLatin1Char('/') + mailFile);
    bool openFile = file.open(QIODevice::ReadOnly);
    Q_ASSERT(openFile);
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}

void GrantleeHeaderFormatterTest::testInvalid()
{
    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    auto aMsg = readAndParseMail(QStringLiteral("allheaders.mbox"));

    QString filename = QStringLiteral("invalid");
    QString absolutePath = QStringLiteral(HEADER_DATA_DIR) + QLatin1Char('/') + filename;
    QString data = formatter.toHtml(QStringList(), absolutePath, filename, &style, aMsg.data(), false);

    QCOMPARE(data, QStringLiteral("Template not found, invalid"));
}

void GrantleeHeaderFormatterTest::testPrint()
{
    QString tmplName = QStringLiteral("printtest.tmpl");

    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    KMime::Message::Ptr aMsg(new KMime::Message);

    const QString &absolutePath = QStringLiteral(HEADER_DATA_DIR) + QLatin1Char('/') + tmplName;

    {
        const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), false);
        testHeaderFile(QStringLiteral("<div><div>")+data, absolutePath, QStringLiteral("printtest.off"));
    }

    {
        const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), true);
        testHeaderFile(QStringLiteral("<div><div>")+data, absolutePath, QStringLiteral("printtest.on"));
    }
}

void GrantleeHeaderFormatterTest::testFancyDate()
{
    QString tmplName = QStringLiteral("fancydate.tmpl");

    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    KMime::Message::Ptr msg(new KMime::Message);

    {
        auto datetime(QDateTime::currentDateTime());
        datetime.setTime(QTime(12,34,56));
        datetime = datetime.addDays(-1);

        const QByteArray data = "From: from@example.com\nDate: " + datetime.toString(Qt::RFC2822Date).toLocal8Bit() + "\nTo: to@example.com\n\ncontent";
        msg->setContent(KMime::CRLFtoLF(data));
        msg->parse();
    }

    const QString &absolutePath = QStringLiteral(HEADER_DATA_DIR) + QLatin1Char('/') + tmplName;

    const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, msg.data(), false);
    testHeaderFile(QStringLiteral("<div><div>")+data, absolutePath, QStringLiteral("fancydate"));

}

void GrantleeHeaderFormatterTest::testBlock_data()
{
    QTest::addColumn<QString>("tmplName");

    QDir dir(QStringLiteral(HEADER_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.tmpl")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file;
    }
}

void GrantleeHeaderFormatterTest::testBlock()
{
    QFETCH(QString, tmplName);

    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    auto aMsg = readAndParseMail(QStringLiteral("headertest.mbox"));

    QString absolutePath = QStringLiteral(HEADER_DATA_DIR) + QLatin1Char('/') + tmplName;
    QString data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), false);

    testHeaderFile(QStringLiteral("<div><div>")+data, absolutePath, tmplName);
}
