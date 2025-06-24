/*
   SPDX-FileCopyrightText: 2018 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "grantleeheaderformattertest.h"
using namespace Qt::Literals::StringLiterals;

#include "../grantleeheaderformatter.h"
#include <MessageViewer/GrantleeHeaderStyle>
#include <MimeTreeParser/NodeHelper>

#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>
#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(GrantleeHeaderFormatterTest)

GrantleeHeaderFormatterTest::GrantleeHeaderFormatterTest(QObject *parent)
    : QObject(parent)
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
}

static void testHeaderFile(const QString &data, const QString &absolutePath, const QString &name)
{
    QString header = QStringLiteral(
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<body>\n");
    header += data + u"</div>\n</div>"_s;
    header += u"\n</body>\n</html>\n"_s;

    header.replace(u"file://"_s + absolutePath, u"file://PATHTOSTYLE"_s);
    header.replace(QRegularExpression(u"[\t ]+"_s), u" "_s);
    header.replace(QRegularExpression(u"[\t ]*\n+[\t ]*"_s), u"\n"_s);
    header.replace(QRegularExpression(u"([\n\t ])\\1+"_s), u"\\1"_s);
    header.replace(QRegularExpression(u">\n+[\t ]*"_s), u">"_s);
    header.replace(QRegularExpression(u"[\t ]*\n+[\t ]*<"_s), u"<"_s);
    header.replace(QLatin1StringView("&nbsp;"), QLatin1StringView("NBSP_ENTITY_PLACEHOLDER")); // xmlling chokes on &nbsp;

    QString outName = name + u".out.html"_s;
    QString fName = name + u".html"_s;

    QVERIFY(QFile(QStringLiteral(HEADER_DATA_DIR "/") + fName).exists());

    {
        QFile f(outName);
        f.open(QIODevice::WriteOnly);
        f.write(header.toUtf8());
        f.close();
    }
    // TODO add proper cmake check for xmllint and diff
    {
        const QStringList args = QStringList() << u"--format"_s << u"--encode"_s << QStringLiteral("UTF8") << u"--output"_s << fName << outName;
        QCOMPARE(QProcess::execute(u"xmllint"_s, args), 0);
    }

    {
        // compare to reference file
        const QStringList args = QStringList() << u"-u"_s << fName << QStringLiteral(HEADER_DATA_DIR "/") + fName;
        QProcess proc;
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
        proc.start(u"diff"_s, args);
        QVERIFY(proc.waitForFinished());

        QCOMPARE(proc.exitCode(), 0);
    }
}

static KMime::Message::Ptr readAndParseMail(const QString &mailFile)
{
    QFile file(QStringLiteral(HEADER_DATA_DIR) + u'/' + mailFile);
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
    MimeTreeParser::NodeHelper nodeHelper;
    style.setNodeHelper(&nodeHelper);
    auto aMsg = readAndParseMail(u"allheaders.mbox"_s);

    QString filename = u"invalid"_s;
    QString absolutePath = QStringLiteral(HEADER_DATA_DIR) + u'/' + filename;
    QString data = formatter.toHtml(QStringList(), absolutePath, filename, &style, aMsg.data(), false);

    QCOMPARE(data, u"Template not found, invalid"_s);
}

void GrantleeHeaderFormatterTest::testPrint()
{
    QString tmplName = u"printtest.tmpl"_s;

    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setNodeHelper(&nodeHelper);
    KMime::Message::Ptr aMsg(new KMime::Message);

    const QString &absolutePath = QStringLiteral(HEADER_DATA_DIR) + u'/' + tmplName;

    {
        const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), false);
        testHeaderFile(u"<div><div>"_s + data, absolutePath, u"printtest.off"_s);
    }

    {
        const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), true);
        testHeaderFile(u"<div><div>"_s + data, absolutePath, u"printtest.on"_s);
    }
}

void GrantleeHeaderFormatterTest::testFancyDate()
{
    QString tmplName = u"fancydate.tmpl"_s;

    auto style = GrantleeHeaderStyle();
    auto formatter = GrantleeHeaderFormatter();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setNodeHelper(&nodeHelper);
    KMime::Message::Ptr msg(new KMime::Message);

    {
        auto datetime(QDateTime::currentDateTime());
        datetime.setTime(QTime(12, 34, 56));
        datetime = datetime.addDays(-1);

        const QByteArray data = "From: from@example.com\nDate: " + datetime.toString(Qt::RFC2822Date).toLocal8Bit() + "\nTo: to@example.com\n\ncontent";
        msg->setContent(KMime::CRLFtoLF(data));
        msg->parse();
    }

    const QString &absolutePath = QStringLiteral(HEADER_DATA_DIR) + u'/' + tmplName;

    const QString &data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, msg.data(), false);
    testHeaderFile(u"<div><div>"_s + data, absolutePath, u"fancydate"_s);
}

void GrantleeHeaderFormatterTest::testBlock_data()
{
    QTest::addColumn<QString>("tmplName");

    QDir dir(QStringLiteral(HEADER_DATA_DIR));
    const auto l = dir.entryList(QStringList(u"*.tmpl"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".html"_s)) {
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
    MimeTreeParser::NodeHelper nodeHelper;
    style.setNodeHelper(&nodeHelper);
    auto aMsg = readAndParseMail(u"headertest.mbox"_s);

    QString absolutePath = QStringLiteral(HEADER_DATA_DIR) + u'/' + tmplName;
    QString data = formatter.toHtml(QStringList(), QStringLiteral(HEADER_DATA_DIR), tmplName, &style, aMsg.data(), false);

    testHeaderFile(u"<div><div>"_s + data, absolutePath, tmplName);
}

#include "moc_grantleeheaderformattertest.cpp"
