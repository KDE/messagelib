/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "urlhashingtest.h"
using namespace Qt::Literals::StringLiterals;

#include "../urlhashing.h"
#include <QTest>
#include <QUrl>

UrlHashingTest::UrlHashingTest(QObject *parent)
    : QObject(parent)
{
}

UrlHashingTest::~UrlHashingTest() = default;

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
    Canonicalize("http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/")
        = "http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/";
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

    QTest::newRow("http://host/%25%32%35") << u"http://host/%25%32%35"_s << u"http://host/%25"_s;
    QTest::newRow("http://host/%25%32%35%25%32%35") << u"http://host/%25%32%35%25%32%35"_s << u"http://host/%25%25"_s;
    QTest::newRow("http://host/%2525252525252525") << u"http://host/%2525252525252525"_s << u"http://host/%25"_s;
    QTest::newRow("http://host/asdf%25%32%35asd") << u"http://host/asdf%25%32%35asd"_s << u"http://host/asdf%25asd"_s;
    QTest::newRow("http://host/%%%25%32%35asd%%") << u"http://host/%%%25%32%35asd%%"_s << u"http://host/%25%25%25asd%25%25"_s;
    QTest::newRow("http://www.google.com/") << u"http://www.google.com/"_s << u"http://www.google.com/"_s;
    QTest::newRow("http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/")
        << u"http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/"_s
        << u"http://168.188.99.26/.secure/www.ebay.com/"_s;
    QTest::newRow("test8") << u"http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/"_s
                           << u"http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure=updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx=hgplmcx/"_s;
    QTest::newRow("test9") << u"http://host%23.com/%257Ea%2521b%2540c%2523d%2524e%25f%255E00%252611%252A22%252833%252944_55%252B"_s
                           << u"http://host%23.com/~a!b@c%23d$e%25f^00&11*22(33)44_55+"_s;
    QTest::newRow("http://3279880203/blah") << u"http://3279880203/blah"_s << u"http://195.127.0.11/blah"_s;
    QTest::newRow("http://www.google.com/blah/..") << u"http://www.google.com/blah/.."_s << u"http://www.google.com/"_s;
    QTest::newRow("www.google.com/") << u"www.google.com/"_s << u"http://www.google.com/"_s;
    QTest::newRow("www.google.com") << u"www.google.com"_s << u"http://www.google.com/"_s;
    QTest::newRow("http://www.evil.com/blah#frag") << u"http://www.evil.com/blah#frag"_s << u"http://www.evil.com/blah"_s;
    QTest::newRow("http://www.GOOgle.com/") << u"http://www.GOOgle.com/"_s << u"http://www.google.com/"_s;
    QTest::newRow("http://www.google.com.../") << u"http://www.google.com.../"_s << u"http://www.google.com/"_s;
    QTest::newRow("http://www.google.com/foo\tbar\rbaz\n2") << u"http://www.google.com/foo\tbar\rbaz\n2"_s << u"http://www.google.com/foobarbaz2"_s;
    QTest::newRow("http://www.google.com/q?") << u"http://www.google.com/q?"_s << u"http://www.google.com/q?"_s;
    QTest::newRow("http://www.google.com/q?r?") << u"http://www.google.com/q?r?"_s << u"http://www.google.com/q?r?"_s;
    QTest::newRow("http://www.google.com/q?r?s") << u"http://www.google.com/q?r?s"_s << u"http://www.google.com/q?r?s"_s;
    QTest::newRow("http://evil.com/foo#bar#baz") << u"http://evil.com/foo#bar#baz"_s << u"http://evil.com/foo"_s;
    QTest::newRow("http://evil.com/foo;") << u"http://evil.com/foo;"_s << u"http://evil.com/foo;"_s;
    QTest::newRow("http://evil.com/foo?bar;") << u"http://evil.com/foo?bar;"_s << u"http://evil.com/foo?bar;"_s;
    QTest::newRow("http://\x01\x80.com/") << u"http://\x01\x80.com/"_s << u"http://%01%80.com/"_s;
    QTest::newRow("http://notrailingslash.com") << u"http://notrailingslash.com"_s << u"http://notrailingslash.com/"_s;
    QTest::newRow("http://www.gotaport.com:1234/") << u"http://www.gotaport.com:1234/"_s << u"http://www.gotaport.com/"_s;
    QTest::newRow("  http://www.google.com/  ") << u"  http://www.google.com/  "_s << u"http://www.google.com/"_s;
    QTest::newRow("http:// leadingspace.com/") << u"http:// leadingspace.com/"_s << u"http://%20leadingspace.com/"_s;
    QTest::newRow("http://%20leadingspace.com/") << u"http://%20leadingspace.com/"_s << u"http://%20leadingspace.com/"_s;
    QTest::newRow("%20leadingspace.com/") << u"%20leadingspace.com/"_s << u"http://%20leadingspace.com/"_s;
    QTest::newRow("https://www.securesite.com/") << u"https://www.securesite.com/"_s << u"https://www.securesite.com/"_s;
    QTest::newRow("http://host.com/ab%23cd") << u"http://host.com/ab%23cd"_s << u"http://host.com/ab%23cd"_s;
    QTest::newRow("http://host.com//twoslashes?more//slashes")
        << u"http://host.com//twoslashes?more//slashes"_s << u"http://host.com/twoslashes?more//slashes"_s;
}

void UrlHashingTest::shouldCanonicalizeUrl()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    input = input.trimmed();
    QEXPECT_FAIL("http://host/%2525252525252525", "Not supported yet", Continue);
    QEXPECT_FAIL("http://\x01\x80.com/", "Not supported yet", Continue);
    QEXPECT_FAIL("%20leadingspace.com/", "Not supported yet", Continue);
    QEXPECT_FAIL("http://%20leadingspace.com/", "Not supported yet", Continue);
    QEXPECT_FAIL("http://www.google.com.../", "Not supported yet", Continue);

    QEXPECT_FAIL("http://http/host%23.com/%7Ea%21b%40c%23d%24e%f%5E00%2611%2A22%2833%2944_55%2B", "Not supported yet", Continue);
    QEXPECT_FAIL("http://host/%%%25%32%35asd%%", "Not supported yet", Continue);
    QEXPECT_FAIL("http://host/%25252525252525", "Not supported yet", Continue);
    QEXPECT_FAIL("http:// leadingspace.com/", "Not supported yet", Continue);
    QEXPECT_FAIL("test9", "Not supported yet", Continue);
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
    hosts << u"b.c"_s << u"a.b.c"_s;
    paths << u"/"_s << u"/1/"_s << QStringLiteral("/1/2.html") << QStringLiteral("/1/2.html?param=1");
    QTest::newRow("http://a.b.c/1/2.html?param=1") << u"http://a.b.c/1/2.html?param=1"_s << hosts << paths;
    hosts.clear();
    paths.clear();
    hosts << u"f.g"_s << u"e.f.g"_s << QStringLiteral("d.e.f.g") << QStringLiteral("c.d.e.f.g") << QStringLiteral("a.b.c.d.e.f.g");
    paths << u"/"_s << u"/1.html"_s;
    QTest::newRow("http://a.b.c.d.e.f.g/1.html") << u"http://a.b.c.d.e.f.g/1.html"_s << hosts << paths;

    hosts.clear();
    paths.clear();
    hosts << u"a.b"_s;
    paths << u"/"_s << u"/saw-cgi/"_s << QStringLiteral("/saw-cgi/eBayISAPI.dll/");
    QTest::newRow("http://a.b/saw-cgi/eBayISAPI.dll/") << u"http://a.b/saw-cgi/eBayISAPI.dll/"_s << hosts << paths;
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
    QTest::newRow("http://a.b/saw-cgi/eBayISAPI.dll/") << QUrl(u"http://a.b/saw-cgi/eBayISAPI.dll/"_s) << 3;
}

void UrlHashingTest::shouldGenerateHashList()
{
    QFETCH(QUrl, input);
    QFETCH(int, numberItems);
    WebEngineViewer::UrlHashing hashing(input);
    QCOMPARE(hashing.hashList().count(), numberItems);
}

QTEST_GUILESS_MAIN(UrlHashingTest)

#include "moc_urlhashingtest.cpp"
