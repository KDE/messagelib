/*
   SPDX-FileCopyrightText: 2018 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "grantleeheaderstyletest.h"

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

void testHeaderFile(const HeaderStyle &style, KMime::Message *msg, const QString &name)
{
    QString header = QStringLiteral(
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<body>\n");
    header += style.format(msg) + QStringLiteral("</div>");
    header += QStringLiteral("\n</body>\n</html>\n");

    header.replace(QStringLiteral("file://") + style.theme().absolutePath(), QStringLiteral("file://PATHTOSTYLE"));
    header.replace(QRegularExpression(QStringLiteral("[\t ]+")), QStringLiteral(" "));
    header.replace(QRegularExpression(QStringLiteral("[\t ]*\n+[\t ]*")), QStringLiteral("\n"));
    header.replace(QRegularExpression(QStringLiteral("([\n\t ])\\1+")), QStringLiteral("\\1"));
    header.replace(QRegularExpression(QStringLiteral(">\n+[\t ]*")), QStringLiteral(">"));
    header.replace(QRegularExpression(QStringLiteral("[\t ]*\n+[\t ]*<")), QStringLiteral("<"));
    header.replace(QLatin1StringView("&nbsp;"), QLatin1StringView("NBSP_ENTITY_PLACEHOLDER")); // xmlling chokes on &nbsp;

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
        const QStringList args = QStringList() << QStringLiteral("--format") << QStringLiteral("--encode") << QStringLiteral("UTF8")
                                               << QStringLiteral("--output") << fName << outName;
        QCOMPARE(QProcess::execute(QStringLiteral("xmllint"), args), 0);
    }

    {
        // compare to reference file
        const QStringList args = QStringList() << QStringLiteral("-u") << fName << QStringLiteral(HEADER_DATA_DIR "/") + fName;
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

void GrantleeHeaderStyleTest::initTestCase()
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
    expectedDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    expectedDataLocation += QStringLiteral("/messageviewer/defaultthemes");
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
            const QString &newPath = dirName + QLatin1Char('/') + file.fileName();
            QVERIFY(QFile(file.absoluteFilePath()).copy(newPath));
        }
    }
    const auto configDir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
    if (!configDir.exists()) {
        QVERIFY(configDir.mkpath(QStringLiteral(".")));
    }
    QFile antispamFile(QStringLiteral(HEADER_DATA_DIR "/kmail.antispamrc"));
    const QString &newPath = configDir.filePath(QStringLiteral("kmail.antispamrc"));
    antispamFile.copy(newPath);
}

void GrantleeHeaderStyleTest::cleanupTestCase()
{
    QDir targetDir(expectedDataLocation);

    if (targetDir.exists()) {
        QVERIFY(targetDir.removeRecursively()); // Start with a fresh copy
    }
}

const GrantleeTheme::Theme defaultTheme(const QString &name = QStringLiteral("5.2"))
{
    const QStringList defaultThemePath =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("messageviewer/defaultthemes/"), QStandardPaths::LocateDirectory);
    return GrantleeTheme::ThemeManager::loadTheme(defaultThemePath.at(0) + QStringLiteral("/") + name, name, QStringLiteral("kmail_default.desktop"));
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
    QCOMPARE(style.format(aMsg), QStringLiteral("Grantlee theme \"\" is not valid."));
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeaderEmpty()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setTheme(defaultTheme());
    style.setNodeHelper(&nodeHelper);

    testHeaderFile(style, aMsg, QStringLiteral("empty"));
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeaderVCard()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    MimeTreeParser::NodeHelper nodeHelper;
    style.setTheme(defaultTheme());
    style.setNodeHelper(&nodeHelper);
    style.setVCardName(QStringLiteral("nofile.vcd"));

    testHeaderFile(style, aMsg, QStringLiteral("vcard"));
    delete aMsg;
}

void GrantleeHeaderStyleTest::testRenderHeader_data()
{
    QTest::addColumn<QString>("mailFileName");

    QDir dir(QStringLiteral(HEADER_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))) {
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
