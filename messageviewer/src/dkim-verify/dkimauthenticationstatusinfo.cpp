/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfo.h"
#include "dkimauthenticationstatusinfoutil.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QRegularExpressionMatch>
using namespace Qt::Literals::StringLiterals;

using namespace MessageViewer;
// see https://tools.ietf.org/html/rfc7601
DKIMAuthenticationStatusInfo::DKIMAuthenticationStatusInfo() = default;

bool DKIMAuthenticationStatusInfo::parseAuthenticationStatus(const QString &key, bool relaxingParsing)
{
    QString valueKey = key;
    // kmime remove extra \r\n but we need it for regexp at the end.
    if (!valueKey.endsWith(QLatin1StringView("\r\n"))) {
        valueKey += QLatin1StringView("\r\n");
    }
    // https://tools.ietf.org/html/rfc7601#section-2.2
    // authres-header = "Authentication-Results:" [CFWS] authserv-id
    //                                            [ CFWS authres-version ]
    //                                            ( no-result / 1*resinfo ) [CFWS] CRLF

    // 1) extract AuthservId and AuthVersion
    QRegularExpressionMatch match;
    const QString regStr = DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1StringView("(?:") + DKIMAuthenticationStatusInfoUtil::cfws_p()
        + QLatin1StringView("([0-9]+)") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1StringView(" )?");
    // qDebug() << " regStr" << regStr;
    static const QRegularExpression regular1(regStr);
    int index = valueKey.indexOf(regular1, 0, &match);
    if (index != -1) {
        mAuthservId = match.captured(1);
        const QString authVersionStr = match.captured(2);
        if (!authVersionStr.isEmpty()) {
            mAuthVersion = authVersionStr.toInt();
        } else {
            mAuthVersion = 1;
        }
        valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0)));
        // qDebug() << " match.captured(0)"<<match.captured(0)<<"match.captured(1)" <<match.captured(1) << authVersionStr;
        // qDebug() << " valueKey" << valueKey;
    } else {
        return false;
    }
    // check if message authentication was performed
    const QString authResultStr = DKIMAuthenticationStatusInfoUtil::regexMatchO(DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1StringView(";")
                                                                                + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1StringView("?none"));
    // qDebug() << "authResultStr "<<authResultStr;
    static const QRegularExpression regular2(authResultStr);
    index = valueKey.indexOf(regular2, 0, &match);
    if (index != -1) {
        // no result
        return false;
    }
    while (!valueKey.isEmpty()) {
        // qDebug() << "valueKey LOOP" << valueKey;
        const AuthStatusInfo resultInfo = parseAuthResultInfo(valueKey, relaxingParsing);
        if (resultInfo.isValid()) {
            mListAuthStatusInfo.append(resultInfo);
        }
    }
    return true;
}

bool DKIMAuthenticationStatusInfo::checkResultKeyword(const QString &method, const QString &resultKeyword) const
{
    QStringList allowedKeywords;

    // DKIM and DomainKeys (RFC 8601 section 2.7.1.)
    if (method == u"dkim"_s || method == u"domainkeys"_s) {
        allowedKeywords = {u"none"_s, u"pass"_s, u"fail"_s, u"policy"_s, u"neutral"_s, u"temperror"_s, u"permerror"_s};
    }

    // SPF and Sender ID (RFC 8601 section 2.7.2.)
    if (method == u"spf"_s || method == u"sender-id"_s) {
        allowedKeywords = {u"none"_s,
                           u"pass"_s,
                           u"fail"_s,
                           u"softfail"_s,
                           u"policy"_s,
                           u"neutral"_s,
                           u"temperror"_s,
                           u"permerror"_s
                           // Deprecated from older ARH RFC 5451.
                           ,
                           u"hardfail"_s
                           // Older SPF specs (e.g. RFC 4408) used mixed case.
                           ,
                           u"None"_s,
                           u"Pass"_s,
                           u"Fail"_s,
                           u"SoftFail"_s,
                           u"Neutral"_s,
                           u"TempError"_s,
                           u"PermError"_s};
    }

    // DMARC (RFC 7489 section 11.2.)
    if (method == u"dmarc"_s) {
        allowedKeywords = {u"none"_s, u"pass"_s, QStringLiteral("fail"), QStringLiteral("temperror"), QStringLiteral("permerror")};
    }

    // BIMI (https://datatracker.ietf.org/doc/draft-brand-indicators-for-message-identification/04/ section 7.7.)
    if (method == u"bimi"_s) {
        allowedKeywords = {u"pass"_s, u"none"_s, u"fail"_s, u"temperror"_s, u"declined"_s, u"skipped"_s};
    }

    // Note: Both the ARH RFC and the IANA registry contain keywords for more than the above methods.
    // As we don't really care about them, for simplicity we treat them the same as unknown methods,
    // And don't restrict the keyword.

    if (!allowedKeywords.contains(resultKeyword)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Result keyword " << resultKeyword << " is not allowed for method " << method;
        return false;
    }
    return true;
}

DKIMAuthenticationStatusInfo::AuthStatusInfo DKIMAuthenticationStatusInfo::parseAuthResultInfo(QString &valueKey, bool relaxingParsing)
{
    // qDebug() << " valueKey *****************" << valueKey;
    DKIMAuthenticationStatusInfo::AuthStatusInfo authStatusInfo;
    // 2) extract methodspec
    const QString methodVersionp =
        DKIMAuthenticationStatusInfoUtil::cfws_op() + u'/' + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1StringView("([0-9]+)");
    const QString method_p = u'(' + DKIMAuthenticationStatusInfoUtil::keyword_p() + QLatin1StringView(")(?:") + methodVersionp + QLatin1StringView(")?");
    QString Keyword_result_p = u"none|pass|fail|softfail|policy|neutral|temperror|permerror"_s;
    // older SPF specs (e.g. RFC 4408) use mixed case
    Keyword_result_p += QLatin1StringView("|None|Pass|Fail|SoftFail|Neutral|TempError|PermError");
    const QString result_p = QLatin1Char('=') + DKIMAuthenticationStatusInfoUtil::cfws_op() + u'(' + Keyword_result_p + u')';
    const QString methodspec_p = u';' + DKIMAuthenticationStatusInfoUtil::cfws_op() + method_p + DKIMAuthenticationStatusInfoUtil::cfws_op() + result_p;

    // qDebug() << "methodspec_p " << methodspec_p;
    QRegularExpressionMatch match;
    static const QRegularExpression reg2(methodspec_p);
    int index = valueKey.indexOf(reg2, 0, &match);
    if (index == -1) {
        valueKey = QString(); // remove it !
        qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "methodspec not found ";
        // no result
        return authStatusInfo;
    }
    // qDebug() << " match" << match.captured(0) << match.captured(1) << match.capturedTexts();
    authStatusInfo.method = match.captured(1);
    const QString authVersionStr = match.captured(2);
    if (!authVersionStr.isEmpty()) {
        authStatusInfo.methodVersion = authVersionStr.toInt();
    } else {
        authStatusInfo.methodVersion = 1;
    }
    authStatusInfo.result = match.captured(3);

    valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!

    // 3) extract reasonspec (optional)
    const QString reasonspec_p =
        DKIMAuthenticationStatusInfoUtil::regexMatchO(QLatin1StringView("reason") + DKIMAuthenticationStatusInfoUtil::cfws_op() + u'='
                                                      + DKIMAuthenticationStatusInfoUtil::cfws_op() + DKIMAuthenticationStatusInfoUtil::value_cp());
    static const QRegularExpression reg31(reasonspec_p);
    index = valueKey.indexOf(reg31, 0, &match);
    if (index != -1) {
        // qDebug() << " reason " << match.capturedTexts();
        authStatusInfo.reason = match.captured(2);
        valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!
    }
    // 4) extract propspec (optional)
    QString pvalue_p = DKIMAuthenticationStatusInfoUtil::value_p() + QLatin1StringView("|(?:(?:") + DKIMAuthenticationStatusInfoUtil::localPart_p()
        + QLatin1StringView("?@)?") + DKIMAuthenticationStatusInfoUtil::domainName_p() + u')';
    if (relaxingParsing) {
        // Allow "/" in the header.b (or other) property, even if it is not in a quoted-string
        pvalue_p += u"|[^ \\x00-\\x1F\\x7F()<>@,;:\\\\\"[\\]?=]+"_s;
    }

    const QString property_p = QLatin1StringView("mailfrom|rcptto") + u'|' + DKIMAuthenticationStatusInfoUtil::keyword_p();
    const QString propspec_p = u'(' + DKIMAuthenticationStatusInfoUtil::keyword_p() + u')' + DKIMAuthenticationStatusInfoUtil::cfws_op()
        + QLatin1StringView("\\.") + DKIMAuthenticationStatusInfoUtil::cfws_op() + u'(' + property_p + u')' + DKIMAuthenticationStatusInfoUtil::cfws_op() + u'='
        + DKIMAuthenticationStatusInfoUtil::cfws_op() + u'(' + pvalue_p /*+ u')'*/;

    // qDebug() << "propspec_p " << propspec_p;

    const QString regexp = DKIMAuthenticationStatusInfoUtil::regexMatchO(propspec_p);
    static const QRegularExpression reg(regexp);
    if (!reg.isValid()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " reg error : " << reg.errorString();
    } else {
        index = valueKey.indexOf(reg, 0, &match);
        while (index != -1) {
            // qDebug() << " propspec " << match.capturedTexts();
            valueKey = valueKey.right(valueKey.length() - (index + match.capturedLength(0))); // Improve it!
            // qDebug() << " value KEy " << valueKey;
            const QString &captured1 = match.captured(1);
            // qDebug() << " captured1 " << captured1;
            if (captured1 == QLatin1StringView("header")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.header.append(prop);
            } else if (captured1 == QLatin1StringView("smtp")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.smtp.append(prop);
            } else if (captured1 == QLatin1StringView("body")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.body.append(prop);
            } else if (captured1 == QLatin1StringView("policy")) {
                AuthStatusInfo::Property prop;
                prop.type = match.captured(2);
                prop.value = match.captured(3);
                authStatusInfo.policy.append(prop);
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Unknown type found " << captured1;
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
    return mAuthservId == other.authservId() && mAuthVersion == other.authVersion() && mReasonSpec == other.reasonSpec()
        && mListAuthStatusInfo == other.listAuthStatusInfo();
}

QList<DKIMAuthenticationStatusInfo::AuthStatusInfo> DKIMAuthenticationStatusInfo::listAuthStatusInfo() const
{
    return mListAuthStatusInfo;
}

void DKIMAuthenticationStatusInfo::setListAuthStatusInfo(const QList<AuthStatusInfo> &listAuthStatusInfo)
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

QDebug operator<<(QDebug d, const DKIMAuthenticationStatusInfo &t)
{
    d << "mAuthservId: " << t.authservId();
    d << "mReasonSpec: " << t.reasonSpec();
    d << "mAuthVersion: " << t.authVersion() << '\n';
    const auto listAuthStatusInfo = t.listAuthStatusInfo();
    for (const DKIMAuthenticationStatusInfo::AuthStatusInfo &info : listAuthStatusInfo) {
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
    return other.method == method && other.result == result && other.methodVersion == methodVersion && other.reason == reason && other.policy == policy
        && other.smtp == smtp && other.header == header && other.body == body;
}

bool DKIMAuthenticationStatusInfo::AuthStatusInfo::isValid() const
{
    return !method.isEmpty();
}
