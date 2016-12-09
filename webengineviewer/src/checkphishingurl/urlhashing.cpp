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

#include "urlhashing.h"
#include <QDebug>

using namespace WebEngineViewer;

UrlHashing::UrlHashing(const QUrl &url)
    : mUrl(url)
{

}

UrlHashing::~UrlHashing()
{

}

QString UrlHashing::canonicalizeUrl(QUrl url)
{
    if (url.isEmpty()) {
        return {};
    }
    QString path = url.path();
    if (url.path().isEmpty()) {
        url.setPath(QStringLiteral("/"));
    } else {
        // First, remove tab (0x09), CR (0x0d), and LF (0x0a) characters from the URL. Do not remove escape sequences for these characters (e.g. '%0a').
        path.remove(QLatin1Char('\t'));
        path.remove(QLatin1Char('\r'));
        path.remove(QLatin1Char('\n'));

        //In the URL, percent-escape all characters that are <= ASCII 32, >= 127, "#", or "%". The escapes should use uppercase hex characters.
        //TODO

        url.setPath(path);
    }
    // Remove all leading and trailing dots.
#if 0
    QString hostname = url.host();
    qDebug() << " hostname" << hostname;
    while (!hostname.isEmpty() && hostname.at(0) == QLatin1Char('.')) {
        hostname.remove(0, 1);
    }
    qDebug() << "111111 hostname" << hostname;
    for (int i = hostname.length(); i >= 0; --i) {
        if (hostname.at(i) == QLatin1Char('.')) {
            hostname.remove(i);
        } else {
            break;
        }
    }
    qDebug() << "2222222 hostname" << hostname;
    mUrl.setHost(hostname);
#endif
    QByteArray urlEncoded = url.toEncoded(QUrl::RemoveFragment | QUrl::NormalizePathSegments | QUrl::EncodeUnicode | QUrl::RemoveUserInfo | QUrl::RemovePort | QUrl::RemovePassword);
    //qDebug() << "BEFORE  urlEncoded" <<urlEncoded;
    urlEncoded.replace(QByteArrayLiteral("%25"), QByteArrayLiteral("%"));
    //qDebug() << "AFTER  urlEncoded" <<urlEncoded;
    return QString::fromLatin1(urlEncoded);
}

QStringList UrlHashing::generatePathsToCheck(const QString &str, const QString &query)
{
    QStringList pathToCheck;
    if (str.isEmpty()) {
        return pathToCheck;
    }
    const int strLenght(str.length());
    for (int i = 0; i < strLenght; ++i) {
        //We check 5 element => 4 here and host if necessary
        if (pathToCheck.count() == 4) {
            break;
        }
        if (str.at(i) == QLatin1Char('/')) {
            if (i == 0) {
                pathToCheck << QStringLiteral("/");
            } else {
                pathToCheck << str.left(i);
            }
        }
    }
    if (!pathToCheck.isEmpty() && pathToCheck.at(pathToCheck.count()-1) != str) {
        pathToCheck << str;
    }
    if (!query.isEmpty()) {
        pathToCheck << str + QLatin1Char('?') + query;
    }
    qDebug() << "ssssssssssss :" << pathToCheck;
    return pathToCheck;
}

QStringList UrlHashing::generateHostsToCheck(const QString &str)
{
    QStringList hostToCheck;
    if (str.isEmpty()) {
        return hostToCheck;
    }
    const int strLenght(str.length());
    bool lastElement = true;
    for (int i = strLenght; i > 0; --i) {
        //We need to check just 5 element => 4 splits hosts + current host
        if (hostToCheck.count() == 4) {
            break;
        }
        if (str.at(i) == QLatin1Char('.')) {
            if (lastElement) {
                lastElement = false;
            } else {
                hostToCheck << str.right(strLenght - i - 1);
            }
        }
    }
    hostToCheck << str;
    return hostToCheck;
}

QByteArray UrlHashing::hashComputation()
{
#if 0
    Unit Test(in pseudo - C)

    // Example B1 from FIPS-180-2
    string input1 = "abc";
    string output1 = TruncatedSha256Prefix(input1, 32);
    int expected1[] = { 0xba, 0x78, 0x16, 0xbf };
    assert(output1.size() == 4);  // 4 bytes == 32 bits
    for (int i = 0; i < output1.size(); i++) {
        assert(output1[i] == expected1[i]);
    }

    // Example B2 from FIPS-180-2
    string input2 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    string output2 = TruncatedSha256Prefix(input2, 48);
    int expected2[] = { 0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06 };
    assert(output2.size() == 6);
    for (int i = 0; i < output2.size(); i++) {
        assert(output2[i] == expected2[i]);
    }

    // Example B3 from FIPS-180-2
    string input3(1000000, 'a');  // 'a' repeated a million times
    string output3 = TruncatedSha256Prefix(input3, 96);
    int expected3[] = { 0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92,
                        0x81, 0xa1, 0xc7, 0xe2
                      };
    assert(output3.size() == 12);
    for (int i = 0; i < output3.size(); i++) {
        assert(output3[i] == expected3[i]);
    }
#endif
    return {};
}

QByteArray UrlHashing::hashPrefixComputation()
{
    return {};
}
