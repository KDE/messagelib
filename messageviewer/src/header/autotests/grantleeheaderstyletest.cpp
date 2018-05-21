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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "grantleeheaderstyletest.h"

#include <MessageViewer/GrantleeHeaderStyle>

#include <GrantleeTheme/GrantleeThemeManager>

#include <QFile>
#include <QProcess>
#include <QRegExp>
#include <QStringList>
#include <QStandardPaths>
#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(GrantleeHeaderStyleTest)

void testHeaderFile(const HeaderStyle &style, KMime::Message *msg, const QString &name)
{
    QString header = QStringLiteral("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
                       "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                       "<body>\n");
    header += style.format(msg) + QStringLiteral("</div>\n</div>");
    header += QStringLiteral("\n</body>\n</html>\n");

    header.replace(QStringLiteral("file://") + style.theme().absolutePath(), QStringLiteral("file://PATHTOSTYLE"));
    header.replace(QRegExp(QStringLiteral("[\t ]+")), QStringLiteral(" "));
    header.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*")), QStringLiteral("\n"));
    header.replace(QRegExp(QStringLiteral("([\n\t ])\\1+")), QStringLiteral("\\1"));
    header.replace(QRegExp(QStringLiteral(">\n+[\t ]*")), QStringLiteral(">"));
    header.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*<")), QStringLiteral("<"));

    QString outName = name + QStringLiteral(".out.html");
    QString fName = name + QStringLiteral(".html");

    QVERIFY(QFile(QStringLiteral(HEADER_DATA_DIR"/") + fName).exists());

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
                                << QStringLiteral(HEADER_DATA_DIR"/") + fName;
        QProcess proc;
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
        proc.start(QStringLiteral("diff"), args);
        QVERIFY(proc.waitForFinished());

        QCOMPARE(proc.exitCode(), 0);
    }
}

const GrantleeTheme::Theme defaultTheme(const QString &name=QStringLiteral("5.2"))
{
    const QStringList defaultThemePath = QStandardPaths::locateAll(
                QStandardPaths::GenericDataLocation,
                QStringLiteral("messageviewer/defaultthemes/"),
                QStandardPaths::LocateDirectory);
    return GrantleeTheme::ThemeManager::loadTheme(defaultThemePath.at(0) + QStringLiteral("/")+name,
                name,
                QStringLiteral("kmail_default.desktop"));
}

void GrantleeHeaderStyleTest::testName()
{
    auto style = GrantleeHeaderStyle();
    QCOMPARE(style.name(), "grantlee");
}

void GrantleeHeaderStyleTest::testRenderHeaderEmpty()
{
    auto style = GrantleeHeaderStyle();
    auto aMsg = new KMime::Message();
    QCOMPARE(style.format(nullptr), QString());

    style.setTheme(defaultTheme());

    testHeaderFile(style, aMsg, QStringLiteral("empty"));
}
