/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkimutil.h"
#include <QDebug>
#include <QRegularExpression>
#include <QCryptographicHash>

QString MessageViewer::DKIMUtil::bodyCanonizationRelaxed(QString body)
{
    /*
     * canonicalize the body using the relaxed algorithm
     * specified in Section 3.4.4 of RFC 6376
     */
    /*
        a.  Reduce whitespace:

            *  Ignore all whitespace at the end of lines.  Implementations
                MUST NOT remove the CRLF at the end of the line.

            *  Reduce all sequences of WSP within a line to a single SP
                character.

        b.  Ignore all empty lines at the end of the message body.  "Empty
            line" is defined in Section 3.4.3.  If the body is non-empty but
            does not end with a CRLF, a CRLF is added.  (For email, this is
            only possible when using extensions to SMTP or non-SMTP transport
            mechanisms.)
        */

    body.replace(QLatin1String("\n"), QLatin1String("\r\n"));
    body.replace(QRegularExpression(QLatin1String("[ \t]+\r\n")), QLatin1String("\r\n"));
    body.replace(QRegularExpression(QLatin1String("[ \t]+")), QStringLiteral(" "));
    body.replace(QRegularExpression(QLatin1String("((\r\n)+?)$")), QLatin1String("\r\n"));
    if (body == QLatin1String("\r\n")) {
        body.clear();
    }
    return body;
}

QString MessageViewer::DKIMUtil::bodyCanonizationSimple(QString body)
{
    //    The "simple" body canonicalization algorithm ignores all empty lines
    //       at the end of the message body.  An empty line is a line of zero
    //       length after removal of the line terminator.  If there is no body or
    //       no trailing CRLF on the message body, a CRLF is added.  It makes no
    //       other changes to the message body.  In more formal terms, the
    //       "simple" body canonicalization algorithm converts "*CRLF" at the end
    //       of the body to a single "CRLF".

    //       Note that a completely empty or missing body is canonicalized as a
    //       single "CRLF"; that is, the canonicalized length will be 2 octets.
    body.replace(QLatin1String("\n"), QLatin1String("\r\n"));
    body.replace(QRegularExpression(QLatin1String("((\r\n)+)?$")), QLatin1String("\r\n"));
    if (body.endsWith(QLatin1Literal("\r\n"))) { //Remove it from start
        body.chop(2);
    }
    if (body.isEmpty()) {
        body = QLatin1String("\r\n");
    }
    return body;
}

QByteArray MessageViewer::DKIMUtil::generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo)
{
    return QCryptographicHash::hash(body, algo).toBase64();
}

QString MessageViewer::DKIMUtil::headerCanonizationSimple(const QString &headerName, const QString &headerValue)
{
    //TODO verify it lower it ?
    return headerName + QLatin1Char(':') + headerValue;
}

QString MessageViewer::DKIMUtil::headerCanonizationRelaxed(const QString &headerName, const QString &headerValue, bool removeQuoteOnContentType)
{
    //    The "relaxed" header canonicalization algorithm MUST apply the
    //       following steps in order:

    //       o  Convert all header field names (not the header field values) to
    //          lowercase.  For example, convert "SUBJect: AbC" to "subject: AbC".

    //       o  Unfold all header field continuation lines as described in
    //          [RFC5322]; in particular, lines with terminators embedded in
    //          continued header field values (that is, CRLF sequences followed by
    //          WSP) MUST be interpreted without the CRLF.  Implementations MUST
    //          NOT remove the CRLF at the end of the header field value.

    //       o  Convert all sequences of one or more WSP characters to a single SP
    //          character.  WSP characters here include those before and after a
    //          line folding boundary.

    //       o  Delete all WSP characters at the end of each unfolded header field
    //          value.

    //       o  Delete any WSP characters remaining before and after the colon
    //          separating the header field name from the header field value.  The
    //          colon separator MUST be retained.
    QString newHeaderName = headerName.toLower();
    QString newHeaderValue = headerValue;
    newHeaderValue.replace(QRegularExpression(QStringLiteral("\r\n[ \t]+")), QStringLiteral(" "));
    newHeaderValue.replace(QRegularExpression(QStringLiteral("[ \t]+")), QStringLiteral(" "));
    newHeaderValue.replace(QRegularExpression(QStringLiteral("[ \t]+\r\n")), QStringLiteral("\r\n"));
    //Perhaps remove tab after headername and before value name
    //newHeaderValue.replace(QRegularExpression(QStringLiteral("[ \t]*:[ \t]")), QStringLiteral(":"));
    if (newHeaderName == QLatin1String("content-type") && removeQuoteOnContentType) { //Remove quote in charset
        if (newHeaderValue.contains(QLatin1String("charset=\""))) {
            newHeaderValue.remove(QLatin1Char('"'));
        }
    }
    //Remove extra space.
    newHeaderValue = newHeaderValue.trimmed();
    return newHeaderName + QLatin1Char(':') + newHeaderValue;
}

QString MessageViewer::DKIMUtil::cleanString(QString str)
{
    //Move as static ?
    // WSP help pattern as specified in Section 2.8 of RFC 6376
    const QString pattWSP = QStringLiteral("[ \t]");
    // FWS help pattern as specified in Section 2.8 of RFC 6376
    const QString pattFWS = QStringLiteral("(?:") + pattWSP + QStringLiteral("*(?:\r\n)?") + pattWSP + QStringLiteral("+)");
    str.replace(QRegularExpression(pattFWS), QString());
    return str;
}

QString MessageViewer::DKIMUtil::emailDomain(const QString &emailDomain)
{
    return emailDomain.right(emailDomain.length() - emailDomain.indexOf(QLatin1Char('@')) - 1);
}

QString MessageViewer::DKIMUtil::emailSubDomain(const QString &emailDomain)
{
    int dotNumber = 0;
    for (int i = emailDomain.length() - 1; i >= 0; --i) {
        if (emailDomain.at(i) == QLatin1Char('.')) {
            dotNumber++;
            if (dotNumber == 2) {
                return emailDomain.right(emailDomain.length() - i - 1);
            }
        }
    }
    return emailDomain;
}
