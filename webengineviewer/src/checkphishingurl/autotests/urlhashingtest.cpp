/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "urlhashingtest.h"
#include "../urlhashing.h"
#include <QUrl>
#include <QTest>
#include <QList>

UrlHashingTest::UrlHashingTest(QObject *parent)
    : QObject(parent)
{

}

UrlHashingTest::~UrlHashingTest()
{

}

void UrlHashingTest::shouldCanonicalizeUrl_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
#if 0
    Canonicalize("http://host/%25%32%35") = "http://host/%25";
    Canonicalize("http://host/%25%32%35%25%32%35") = "http://host/%25%25";
    Canonicalize("http://host/%2525252525252525") = "http://host/%25";
    Canonicalize("http://host/asdf%25%32%35asd") = "http://host/asdf%25asd";
    Canonicalize("http://host/%%%25%32%35asd%%") = "http://host/%25%25%25asd%25%25";
    Canonicalize("http://www.google.com/") = "http://www.google.com/";
    Canonicalize("http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/") = "http://168.188.99.26/.secure/www.ebay.com/";
    Canonicalize("http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/") = "http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/";
    Canonicalize("http://host%23.com/%257Ea%2521b%2540c%2523d%2524e%25f%255E00%252611%252A22%252833%252944_55%252B") = "http://host%23.com/~a!b@c%23d$e%25f^00&11*22(33)44_55+";
    Canonicalize("http://3279880203/blah") = "http://195.127.0.11/blah";
    Canonicalize("http://www.google.com/blah/..") = "http://www.google.com/";
    Canonicalize("www.google.com/") = "http://www.google.com/";
    Canonicalize("www.google.com") = "http://www.google.com/";
    Canonicalize("http://www.evil.com/blah#frag") = "http://www.evil.com/blah";
    Canonicalize("http://www.GOOgle.com/") = "http://www.google.com/";
    Canonicalize("http://www.google.com.../") = "http://www.google.com/";
    Canonicalize("http://www.google.com/foo\tbar\rbaz\n2") = "http://www.google.com/foobarbaz2";
    Canonicalize("http://www.google.com/q?") = "http://www.google.com/q?";
    Canonicalize("http://www.google.com/q?r?") = "http://www.google.com/q?r?";
    Canonicalize("http://www.google.com/q?r?s") = "http://www.google.com/q?r?s";
    Canonicalize("http://evil.com/foo#bar#baz") = "http://evil.com/foo";
    Canonicalize("http://evil.com/foo;") = "http://evil.com/foo;";
    Canonicalize("http://evil.com/foo?bar;") = "http://evil.com/foo?bar;";
    Canonicalize("http://\x01\x80.com/") = "http://%01%80.com/";
    Canonicalize("http://notrailingslash.com") = "http://notrailingslash.com/";
    Canonicalize("http://www.gotaport.com:1234/") = "http://www.gotaport.com/";
    Canonicalize("  http://www.google.com/  ") = "http://www.google.com/";
    Canonicalize("http:// leadingspace.com/") = "http://%20leadingspace.com/";
    Canonicalize("http://%20leadingspace.com/") = "http://%20leadingspace.com/";
    Canonicalize("%20leadingspace.com/") = "http://%20leadingspace.com/";
    Canonicalize("https://www.securesite.com/") = "https://www.securesite.com/";
    Canonicalize("http://host.com/ab%23cd") = "http://host.com/ab%23cd";
    Canonicalize("http://host.com//twoslashes?more//slashes") = "http://host.com/twoslashes?more//slashes";
#endif

    QTest::newRow("empty") << QString() << QString();

    QTest::newRow("http://host/%25%32%35") << QStringLiteral("http://host/%25%32%35") << QStringLiteral("http://host/%25");
    QTest::newRow("http://host/%25%32%35%25%32%35") << QStringLiteral("http://host/%25%32%35%25%32%35") << QStringLiteral("http://host/%25%25");
    QTest::newRow("ttp://host/%2525252525252525") << QStringLiteral("http://host/%2525252525252525") << QStringLiteral("http://host/%25");
    QTest::newRow("http://host/asdf%25%32%35asd") << QStringLiteral("http://host/asdf%25%32%35asd") << QStringLiteral("http://host/asdf%25asd");
    QTest::newRow("http://host/%%%25%32%35asd%%") << QStringLiteral("http://host/%%%25%32%35asd%%") << QStringLiteral("http://host/%25%25%25asd%25%25");
    QTest::newRow("http://www.google.com/") << QStringLiteral("http://www.google.com/") << QStringLiteral("http://www.google.com/");
    QTest::newRow("http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/") << QStringLiteral("http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/") << QStringLiteral("http://168.188.99.26/.secure/www.ebay.com/");
    QTest::newRow("test8") << QStringLiteral("http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/") << QStringLiteral("http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/");
    QTest::newRow("test9") << QStringLiteral("http://host%23.com/%257Ea%2521b%2540c%2523d%2524e%25f%255E00%252611%252A22%252833%252944_55%252B") << QStringLiteral("http://host%23.com/~a!b@c%23d$e%25f^00&11*22(33)44_55+");
    QTest::newRow("http://3279880203/blah") << QStringLiteral("http://3279880203/blah") << QStringLiteral("http://195.127.0.11/blah");
    QTest::newRow("http://www.google.com/blah/..") << QStringLiteral("http://www.google.com/blah/..") << QStringLiteral("http://www.google.com/");
    QTest::newRow("www.google.com/") << QStringLiteral("www.google.com/") << QStringLiteral("http://www.google.com/");
    QTest::newRow("www.google.com") << QStringLiteral("www.google.com") << QStringLiteral("http://www.google.com/");
    QTest::newRow("http://www.evil.com/blah#frag") << QStringLiteral("http://www.evil.com/blah#frag") << QStringLiteral("http://www.evil.com/blah");
    QTest::newRow("http://www.GOOgle.com/") << QStringLiteral("http://www.GOOgle.com/") << QStringLiteral("http://www.google.com/");
    QTest::newRow("http://www.google.com.../") << QStringLiteral("http://www.google.com.../") << QStringLiteral("http://www.google.com/");
    QTest::newRow("http://www.google.com/foo\tbar\rbaz\n2") << QStringLiteral("http://www.google.com/foo\tbar\rbaz\n2") << QStringLiteral("http://www.google.com/foobarbaz2");
    QTest::newRow("http://www.google.com/q?") << QStringLiteral("http://www.google.com/q?") << QStringLiteral("http://www.google.com/q?");
    QTest::newRow("http://www.google.com/q?r?") << QStringLiteral("http://www.google.com/q?r?") << QStringLiteral("http://www.google.com/q?r?");
    QTest::newRow("http://www.google.com/q?r?s") << QStringLiteral("http://www.google.com/q?r?s") << QStringLiteral("http://www.google.com/q?r?s");
    QTest::newRow("http://evil.com/foo#bar#baz") << QStringLiteral("http://evil.com/foo#bar#baz") << QStringLiteral("http://evil.com/foo");
    QTest::newRow("http://evil.com/foo;") << QStringLiteral("http://evil.com/foo;") << QStringLiteral("http://evil.com/foo;");
    QTest::newRow("http://evil.com/foo?bar;") << QStringLiteral("http://evil.com/foo?bar;") << QStringLiteral("http://evil.com/foo?bar;");
    QTest::newRow("http://\x01\x80.com/") << QStringLiteral("http://\x01\x80.com/") << QStringLiteral("http://%01%80.com/");
    QTest::newRow("http://notrailingslash.com") << QStringLiteral("http://notrailingslash.com") << QStringLiteral("http://notrailingslash.com/");
    QTest::newRow("http://www.gotaport.com:1234/") << QStringLiteral("http://www.gotaport.com:1234/") << QStringLiteral("http://www.gotaport.com/");
    QTest::newRow("  http://www.google.com/  ") << QStringLiteral("  http://www.google.com/  ") << QStringLiteral("http://www.google.com/");
    QTest::newRow("http:// leadingspace.com/") << QStringLiteral("http:// leadingspace.com/") << QStringLiteral("http://%20leadingspace.com/");
    QTest::newRow("http://%20leadingspace.com/") << QStringLiteral("http://%20leadingspace.com/") << QStringLiteral("http://%20leadingspace.com/");
    QTest::newRow("%20leadingspace.com/") << QStringLiteral("%20leadingspace.com/") << QStringLiteral("http://%20leadingspace.com/");
    QTest::newRow("https://www.securesite.com/") << QStringLiteral("https://www.securesite.com/") << QStringLiteral("https://www.securesite.com/");
    QTest::newRow("http://host.com/ab%23cd") << QStringLiteral("http://host.com/ab%23cd") << QStringLiteral("http://host.com/ab%23cd");
    QTest::newRow("http://host.com//twoslashes?more//slashes") << QStringLiteral("http://host.com//twoslashes?more//slashes") << QStringLiteral("http://host.com/twoslashes?more//slashes");
}

void UrlHashingTest::shouldCanonicalizeUrl()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    input = input.trimmed();
    QCOMPARE(WebEngineViewer::UrlHashing::canonicalizeUrl(QUrl::fromUserInput(input)), output);
}


void UrlHashingTest::shouldGenerateHostPath_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("hosts");
    QTest::addColumn<QStringList>("paths");
    QTest::newRow("empty") << QString() << QStringList() << QStringList();
    QStringList hosts;
    QStringList paths;
    hosts << QStringLiteral("b.c") << QStringLiteral("a.b.c");
    paths << QStringLiteral("/") << QStringLiteral("/1/") << QStringLiteral("/1/2.html") << QStringLiteral("/1/2.html?param=1");
    QTest::newRow("http://a.b.c/1/2.html?param=1") << QStringLiteral("http://a.b.c/1/2.html?param=1") << hosts << paths;
    hosts.clear();
    paths.clear();
    hosts << QStringLiteral("f.g") << QStringLiteral("e.f.g") << QStringLiteral("d.e.f.g") << QStringLiteral("c.d.e.f.g") << QStringLiteral("a.b.c.d.e.f.g");
    paths << QStringLiteral("/") << QStringLiteral("/1.html");
    QTest::newRow("http://a.b.c.d.e.f.g/1.html") << QStringLiteral("http://a.b.c.d.e.f.g/1.html") << hosts << paths;

    hosts.clear();
    paths.clear();
    hosts << QStringLiteral("a.b");
    paths << QStringLiteral("/") << QStringLiteral("/saw-cgi/") << QStringLiteral("/saw-cgi/eBayISAPI.dll/");
    QTest::newRow("http://a.b/saw-cgi/eBayISAPI.dll/") << QStringLiteral("http://a.b/saw-cgi/eBayISAPI.dll/") << hosts << paths;
}

void UrlHashingTest::shouldGenerateHostPath()
{
    QFETCH(QString, input);
    QFETCH(QStringList, hosts);
    QFETCH(QStringList, paths);
    QString result = WebEngineViewer::UrlHashing::canonicalizeUrl(QUrl::fromUserInput(input));
    QUrl url(result);

    QCOMPARE(WebEngineViewer::UrlHashing::generateHostsToCheck(url.host()), hosts);
    QCOMPARE(WebEngineViewer::UrlHashing::generatePathsToCheck(url.path(), url.query()), paths);
}

void UrlHashingTest::shouldGenerateHashList_data()
{
    QTest::addColumn<QUrl>("input");
    QTest::addColumn<int>("numberItems");
    QTest::newRow("http://a.b/saw-cgi/eBayISAPI.dll/") << QUrl(QStringLiteral("http://a.b/saw-cgi/eBayISAPI.dll/")) << 3;
}

void UrlHashingTest::shouldGenerateHashList()
{
    QFETCH(QUrl, input);
    QFETCH(int, numberItems);
    WebEngineViewer::UrlHashing hashing(input);
    QCOMPARE(hashing.hashList().count(), numberItems);
}


QTEST_MAIN(UrlHashingTest)
