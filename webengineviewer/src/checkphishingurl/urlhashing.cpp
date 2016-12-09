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
#include <QCryptographicHash>

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
                pathToCheck << str.left(i+1);
            }
        }
    }
    if (!pathToCheck.isEmpty() && pathToCheck.at(pathToCheck.count()-1) != str) {
        pathToCheck << str;
    }
    if (!query.isEmpty()) {
        pathToCheck << str + QLatin1Char('?') + query;
    }
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

QList<QByteArray> UrlHashing::hashList()
{
    QList<QByteArray> lst;
    if (mUrl.isValid()) {
        const QString result = WebEngineViewer::UrlHashing::canonicalizeUrl(mUrl);
        const QUrl url(result);
        const QStringList hosts = WebEngineViewer::UrlHashing::generateHostsToCheck(url.host());
        const QStringList paths = WebEngineViewer::UrlHashing::generatePathsToCheck(url.path(), url.query());

        Q_FOREACH(const QString &host, hosts) {
            Q_FOREACH(const QString &path, paths) {
                const QString str = host + path;
                QByteArray ba = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Sha256);
                qDebug() << " ba " << ba.toBase64();
                //We need to keep 4
                ba.truncate(4);
                lst << ba;
            }
        }
    }
    return lst;
}

