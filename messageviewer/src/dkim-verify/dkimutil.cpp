/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimutil.h"
using namespace Qt::Literals::StringLiterals;

#include "messageviewer_dkimcheckerdebug.h"
#include <QRegularExpression>

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

    body.replace(u"\n"_s, u"\r\n"_s);
    static const QRegularExpression reg1(u"[ \t]+\r\n"_s);
    body.replace(reg1, u"\r\n"_s);
    static const QRegularExpression reg2(u"[ \t]+"_s);
    body.replace(reg2, u" "_s);
    static const QRegularExpression reg3(u"((\r\n)+?)$"_s);
    body.replace(QRegularExpression(reg3), u"\r\n"_s);
    if (body == QLatin1StringView("\r\n")) {
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
    body.replace(u"\n"_s, u"\r\n"_s);
    static const QRegularExpression reg(u"((\r\n)+)?$"_s);
    body.replace(reg, u"\r\n"_s);
    if (body.endsWith(QLatin1StringView("\r\n"))) { // Remove it from start
        body.chop(2);
    }
    if (body.isEmpty()) {
        body = u"\r\n"_s;
    }
    return body;
}

QByteArray MessageViewer::DKIMUtil::generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo)
{
    return QCryptographicHash::hash(body, algo).toBase64();
}

QString MessageViewer::DKIMUtil::headerCanonizationSimple(const QString &headerName, const QString &headerValue)
{
    // TODO verify it lower it ?
    return headerName + u':' + headerValue;
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
    static const QRegularExpression reg1(u"\r\n[ \t]+"_s);
    newHeaderValue.replace(reg1, u" "_s);
    static const QRegularExpression reg2(u"[ \t]+"_s);
    newHeaderValue.replace(reg2, u" "_s);
    static const QRegularExpression reg3(u"[ \t]+\r\n"_s);
    newHeaderValue.replace(reg3, u"\r\n"_s);
    // Perhaps remove tab after headername and before value name
    // newHeaderValue.replace(QRegularExpression(u"[ \t]*:[ \t]"_s), u":"_s);
    if (newHeaderName == QLatin1StringView("content-type") && removeQuoteOnContentType) { // Remove quote in charset
        if (newHeaderValue.contains(QLatin1StringView("charset=\""))) {
            newHeaderValue.remove(u'"');
        }
    }
    // Remove extra space.
    newHeaderValue = newHeaderValue.trimmed();
    return newHeaderName + u':' + newHeaderValue;
}

QString MessageViewer::DKIMUtil::cleanString(QString str)
{
    // Move as static ?
    // WSP help pattern as specified in Section 2.8 of RFC 6376
    const QString pattWSP = u"[ \t]"_s;
    // FWS help pattern as specified in Section 2.8 of RFC 6376
    const QString pattFWS = u"(?:"_s + pattWSP + u"*(?:\r\n)?"_s + pattWSP + QStringLiteral("+)");
    str.replace(QRegularExpression(pattFWS), QString());
    return str;
}

QString MessageViewer::DKIMUtil::emailDomain(const QString &emailDomain)
{
    return emailDomain.right(emailDomain.length() - emailDomain.indexOf(u'@') - 1);
}

QString MessageViewer::DKIMUtil::emailSubDomain(const QString &emailDomain)
{
    int dotNumber = 0;
    for (int i = emailDomain.length() - 1; i >= 0; --i) {
        if (emailDomain.at(i) == u'.') {
            dotNumber++;
            if (dotNumber == 2) {
                return emailDomain.right(emailDomain.length() - i - 1);
            }
        }
    }
    return emailDomain;
}

QString MessageViewer::DKIMUtil::defaultConfigFileName()
{
    return u"dkimsettingsrc"_s;
}

QString MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod method)
{
    QString methodStr;
    switch (method) {
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown:
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Undefined type";
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim:
        methodStr = u"dkim"_s;
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf:
        methodStr = u"spf"_s;
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc:
        methodStr = u"dmarc"_s;
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps:
        methodStr = u"dkim-atps"_s;
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth:
        methodStr = u"auth"_s;
        break;
    case MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::XTls:
        methodStr = u"x-tls"_s;
        break;
    }
    return methodStr;
}

MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(const QString &str)
{
    if (str == QLatin1StringView("dkim")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim;
    } else if (str == QLatin1StringView("spf")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf;
    } else if (str == QLatin1StringView("dmarc")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc;
    } else if (str == QLatin1StringView("dkim-atps")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps;
    } else if (str == QLatin1StringView("auth")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth;
    } else if (str == QLatin1StringView("x-tls")) {
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::XTls;
    } else {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Undefined type " << str;
        return MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown;
    }
}
