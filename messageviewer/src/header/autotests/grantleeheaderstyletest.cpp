/*
   SPDX-FileCopyrightText: 2018 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "grantleeheaderstyletest.h"
using namespace Qt::Literals::StringLiterals;

#include <MessageViewer/GrantleeHeaderStyle>
#include <MimeTreeParser/NodeHelper>

#include <GrantleeTheme/GrantleeThemeManager>

#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>
#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(GrantleeHeaderStyleTest)

static void testHeaderFile(const HeaderStyle &style, KMime::Message *msg, const QString &name)
{
    QString header = QStringLiteral(
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<body>\n");
    header += style.format(msg) + u"</div>"_s;
    header += u"\n</body>\n</html>\n"_s;

    header.replace(u"file://"_s + style.theme().absolutePath(), u"file://PATHTOSTYLE"_s);
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

void GrantleeHeaderStyleTest::initTestCase()
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
    expectedDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    expectedDataLocation += u"/messageviewer/defaultthemes"_s;
    QDir targetDir(expectedDataLocation);
    QDir sourceDir(QStringLiteral(GRANTLEETHEME_DATA_DIR));
    const auto themeDirs = sourceDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (targetDir.exists()) {
        QVERIFY(targetDir.removeRecursively()); // Start with a fresh copy
    }

    for (const auto &themeDir : themeDirs) {
        const QString &dirName = targetDir.filePath(themeDir.fileName());
        QVERIFY(targetDir.mkpath(themeDir.fileName()));
        const auto files = QDir(themeDir.absoluteFilePath()).entryInfoList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
        for (const auto &file : files) {
            const QString &newPath = dirName + u'/' + file.fileName();
            QVERIFY(QFile(file.absoluteFilePath()).copy(newPath));
        }
    }
    const auto configDir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
    if (!configDir.exists()) {
        QVERIFY(configDir.mkpath(u"."_s));
    }
    QFile antispamFile(QStringLiteral(HEADER_DATA_DIR "/kmail.antispamrc"));
    const QString &newPath = configDir.filePath(u"kmail.antispamrc"_s);
    antispamFile.copy(newPath);
}

void GrantleeHeaderStyleTest::cleanupTestCase()
{
    QDir targetDir(expectedDataLocation);

    if (targetDir.exists()) {
        QVERIFY(targetDir.removeRecursively()); // Start with a fresh copy
    }
}

static const GrantleeTheme::Theme defaultTheme(const QString &name = u"5.2"_s)
{
    const QStringList defaultThemePath =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, u"messageviewer/defaultthemes/"_s, QStandardPaths::LocateDirectory);
    return GrantleeTheme::ThemeManager::loadTheme(defaultThemePath.at(0) + u"/"_s + name, name, u"kmail_default.desktop"_s);
}

void GrantleeHeaderStyleTest::testName()
{
    auto style = GrantleeHeaderStyle();
    QCOMPARE(style.name(), "grantlee");
}

void GrantleeHeaderStyleTest::testRenderHeaderNoMessage()
{
    auto style = GrantleeHeaderStyle();
    QCOMPARE(style.format(nullptr), QString());
}

void GrantleeHeaderStyleTest::testRenderHeaderInvalidTheme()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    QCOMPARE(style.theme().isValid(), false);
    QCOMPARE(style.format(aMsg), u"Grantlee theme \"\" is not valid."_s);
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeaderEmpty()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setTheme(defaultTheme());
    style.setNodeHelper(&nodeHelper);

    testHeaderFile(style, aMsg, u"empty"_s);
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeaderVCard()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setTheme(defaultTheme());
    style.setNodeHelper(&nodeHelper);
    style.setVCardName(u"nofile.vcd"_s);

    testHeaderFile(style, aMsg, u"vcard"_s);
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeader_data()
{
    QTest::addColumn<QString>("mailFileName");

    QDir dir(QStringLiteral(HEADER_DATA_DIR));
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".html"_s)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file;
    }
}

void GrantleeHeaderStyleTest::testRenderHeader()
{
    QFETCH(QString, mailFileName);

    auto style = GrantleeHeaderStyle();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setNodeHelper(&nodeHelper);
    auto aMsg = readAndParseMail(mailFileName);
    style.setTheme(defaultTheme());

    testHeaderFile(style, aMsg.data(), mailFileName);
    aMsg.clear();
}

#include "moc_grantleeheaderstyletest.cpp"
