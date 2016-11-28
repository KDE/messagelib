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
    QTest::newRow("empty") << QString() << QString();
}

void UrlHashingTest::shouldCanonicalizeUrl()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    WebEngineViewer::UrlHashing handling(QUrl::fromUserInput(input));
    QCOMPARE(handling.canonicalizeUrl(), output);

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
    Canonicalize("http://www.google.com/foo\tbar\rbaz\n2") ="http://www.google.com/foobarbaz2";
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
}


QTEST_MAIN(UrlHashingTest)
