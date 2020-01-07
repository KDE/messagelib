/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

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

#include "dkimauthenticationstatusinfo.h"
#include "dkimauthenticationstatusinfoutil.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QRegularExpressionMatch>
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7601
DKIMAuthenticationStatusInfo::DKIMAuthenticationStatusInfo()
{
}

bool DKIMAuthenticationStatusInfo::parseAuthenticationStatus(const QString &key, bool relaxingParsing)
{
    QString valueKey = key;
    //kmime remove extra \r\n but we need it for regexp at the end.
    if (!valueKey.endsWith(QLatin1String("\r\n"))) {
        valueKey += QLatin1String("\r\n");
    }
    // https://tools.ietf.org/html/rfc7601#section-2.2
    // authres-header = "Authentication-Results:" [CFWS] authserv-id
    //                                            [ CFWS authres-version ]
    //                                            ( no-result / 1*resinfo ) [CFWS] CRLF

    // 1) extract AuthservId and AuthVersion
    QRegularExpressionMatch match;
    const QString regStr = DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1String("(?:") + DKIMAuthenticationStatusInfoUtil::cfws_p() + QLatin1String("([0-9]+)") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String(" )?");
    //qDebug() << " regStr" << regStr;
    int index = valueKey.indexOf(QRegularExpression(regStr), 0, &match);
    if (index != -1) {
        mAuthservId = match.captured(1);
        if (!match.captured(2).isEmpty()) {
            mAuthVersion = match.captured(2).toInt();
        } else {
            mAuthVersion = 1;
        }
        valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0)));
        //qDebug() << " match.captured(0)"<<match.captured(0)<<"match.captured(1)" <<match.captured(1) << match.captured(2);
        //qDebug() << " valueKey" << valueKey;
    } else {
        return false;
    }
    // check if message authentication was performed
    const QString authResultStr = DKIMAuthenticationStatusInfoUtil::regexMatchO(DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1String(";") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String("?none"));
    //qDebug() << "authResultStr "<<authResultStr;
    index = valueKey.indexOf(QRegularExpression(authResultStr), 0, &match);
    if (index != -1) {
        //no result
        return false;
    }
    while (!valueKey.isEmpty()) {
        //qDebug() << "valueKey LOOP" << valueKey;
        const AuthStatusInfo resultInfo = parseAuthResultInfo(valueKey, relaxingParsing);
        if (resultInfo.isValid()) {
            mListAuthStatusInfo.append(resultInfo);
        }
    }
    return true;
}

DKIMAuthenticationStatusInfo::AuthStatusInfo DKIMAuthenticationStatusInfo::parseAuthResultInfo(QString &valueKey, bool relaxingParsing)
{
    DKIMAuthenticationStatusInfo::AuthStatusInfo authStatusInfo;
    // 2) extract methodspec
    const QString methodVersionp = DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1Char('/') + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String("([0-9]+)");
    const QString method_p = QLatin1Char('(') + DKIMAuthenticationStatusInfoUtil::keyword_p() + QLatin1String(")(?:") + methodVersionp + QLatin1String(")?");
    QString Keyword_result_p = QLatin1String("none|pass|fail|softfail|policy|neutral|temperror|permerror");
    // older SPF specs (e.g. RFC 4408) use mixed case
    Keyword_result_p += QLatin1String("|None|Pass|Fail|SoftFail|Neutral|TempError|PermError");
    const QString result_p = QLatin1Char('=') + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1Char('(') + Keyword_result_p + QLatin1Char(')');
    const QString methodspec_p = QLatin1Char(';') + DKIMAuthenticationStatusInfoUtil::cfws_op() + method_p + DKIMAuthenticationStatusInfoUtil::cfws_op() + result_p;

    //qDebug() << "methodspec_p " << methodspec_p;
    QRegularExpressionMatch match;
    int index = valueKey.indexOf(QRegularExpression(methodspec_p), 0, &match);
    if (index == -1) {
        valueKey = QString(); //remove it !
        qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "methodspec not found ";
        //no result
        return authStatusInfo;
    }
    //qDebug() << " match" << match.captured(0) << match.captured(1) << match.capturedTexts();
    authStatusInfo.method = match.captured(1);

    if (!match.captured(2).isEmpty()) {
        authStatusInfo.methodVersion = match.captured(2).toInt();
    } else {
        authStatusInfo.methodVersion = 1;
    }
    authStatusInfo.result = match.captured(3);

    valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!

    // 3) extract reasonspec (optional)
    const QString reasonspec_p = DKIMAuthenticationStatusInfoUtil::regexMatchO(QLatin1String("reason") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1Char('=') + DKIMAuthenticationStatusInfoUtil::cfws_op() + DKIMAuthenticationStatusInfoUtil::value_cp());
    index = valueKey.indexOf(QRegularExpression(reasonspec_p), 0, &match);
    if (index != -1) {
        //qDebug() << " reason " << match.capturedTexts();
        authStatusInfo.reason = match.captured(2);
        valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!
    }
    // 4) extract propspec (optional)
    QString pvalue_p = DKIMAuthenticationStatusInfoUtil::value_p() + QLatin1String("|(?:(?:") + DKIMAuthenticationStatusInfoUtil::localPart_p() + QLatin1String("?@)?") + DKIMAuthenticationStatusInfoUtil::domainName_p() + QLatin1Char(')');
    if (relaxingParsing) {
        //Allow "/" in the header.b (or other) property, even if it is not in a quoted-string
        pvalue_p += QStringLiteral("|[^ \\x00-\\x1F\\x7F()<>@,;:\\\\\"[\\]?=]+");
    }

    const QString property_p = QLatin1String("mailfrom|rcptto") + QLatin1Char('|') + DKIMAuthenticationStatusInfoUtil::keyword_p();
    const QString propspec_p = QLatin1Char('(') + DKIMAuthenticationStatusInfoUtil::keyword_p() + QLatin1Char(')')
                               + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String("\\.") + DKIMAuthenticationStatusInfoUtil::cfws_op()
                               + QLatin1Char('(') + property_p + QLatin1Char(')') + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1Char('=')
                               + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1Char('(') + pvalue_p /*+ QLatin1Char(')')*/;

    //qDebug() << "propspec_p " << propspec_p;

    const QString regexp = DKIMAuthenticationStatusInfoUtil::regexMatchO(propspec_p);
    const QRegularExpression reg(regexp);
    if (!reg.isValid()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " reg error : " << reg.errorString();
    } else {
        index = valueKey.indexOf(reg, 0, &match);
        while (index != -1) {
            //qDebug() << " propspec " << match.capturedTexts();
            valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!
            //qDebug() << " value KEy " << valueKey;
            const QString &captured1 = match.captured(1);
            //qDebug() << " captured1 " << captured1;
            if (captured1 == QLatin1String("header")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.header.append(prop);
            } else if (captured1 == QLatin1String("smtp")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.smtp.append(prop);
            } else if (captured1 == QLatin1String("body")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.body.append(prop);
            } else if (captured1 == QLatin1String("policy")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.policy.append(prop);
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Unknow type found " << captured1;
            }
            index = valueKey.indexOf(reg, 0, &match);
        }
    }
    return authStatusInfo;
}

int DKIMAuthenticationStatusInfo::authVersion() const
{
    return mAuthVersion;
}

void DKIMAuthenticationStatusInfo::setAuthVersion(int authVersion)
{
    mAuthVersion = authVersion;
}

QString DKIMAuthenticationStatusInfo::reasonSpec() const
{
    return mReasonSpec;
}

void DKIMAuthenticationStatusInfo::setReasonSpec(const QString &reasonSpec)
{
    mReasonSpec = reasonSpec;
}

bool DKIMAuthenticationStatusInfo::operator==(const DKIMAuthenticationStatusInfo &other) const
{
    return mAuthservId == other.authservId()
           && mAuthVersion == other.authVersion()
           && mReasonSpec == other.reasonSpec()
           && mListAuthStatusInfo == other.listAuthStatusInfo();
}

QVector<DKIMAuthenticationStatusInfo::AuthStatusInfo> DKIMAuthenticationStatusInfo::listAuthStatusInfo() const
{
    return mListAuthStatusInfo;
}

void DKIMAuthenticationStatusInfo::setListAuthStatusInfo(const QVector<AuthStatusInfo> &listAuthStatusInfo)
{
    mListAuthStatusInfo = listAuthStatusInfo;
}

QString DKIMAuthenticationStatusInfo::authservId() const
{
    return mAuthservId;
}

void DKIMAuthenticationStatusInfo::setAuthservId(const QString &authservId)
{
    mAuthservId = authservId;
}

QDebug operator <<(QDebug d, const DKIMAuthenticationStatusInfo &t)
{
    d << "mAuthservId: " << t.authservId();
    d << "mReasonSpec: " << t.reasonSpec();
    d << "mAuthVersion: " << t.authVersion() << '\n';
    for (const DKIMAuthenticationStatusInfo::AuthStatusInfo &info : t.listAuthStatusInfo()) {
        d << "mListAuthStatusInfo: " << info.method << " : " << info.result << " : " << info.methodVersion << " : " << info.reason << '\n';
        d << "Property:" << '\n';
        if (!info.smtp.isEmpty()) {
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.smtp) {
                d << "    smtp " << prop.type << " : " << prop.value << '\n';
            }
        }
        if (!info.header.isEmpty()) {
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.header) {
                d << "    header " << prop.type << " : " << prop.value << '\n';
            }
        }
        if (!info.body.isEmpty()) {
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.body) {
                d << "    body " << prop.type << " : " << prop.value << '\n';
            }
        }
        if (!info.policy.isEmpty()) {
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.policy) {
                d << "    policy " << prop.type << " : " << prop.value << '\n';
            }
        }
    }
    return d;
}

bool DKIMAuthenticationStatusInfo::AuthStatusInfo::operator==(const DKIMAuthenticationStatusInfo::AuthStatusInfo &other) const
{
    return other.method == method
           && other.result == result
           && other.methodVersion == methodVersion
           && other.reason == reason
           && other.policy == policy
           && other.smtp == smtp
           && other.header == header
           && other.body == body;
}

bool DKIMAuthenticationStatusInfo::AuthStatusInfo::isValid() const
{
    return !method.isEmpty();
}
