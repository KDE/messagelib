/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoutil.h"

/*
// domain-name as specified in Section 3.5 of RFC 6376 [DKIM].
let domain_name_p = "(?:" + sub_domain_p + "(?:\\." + sub_domain_p + ")+)";
*/
QString MessageViewer::DKIMAuthenticationStatusInfoUtil::wsp_p()
{
    // WSP as specified in Appendix B.1 of RFC 5234
    return QStringLiteral("[ \t]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::vchar_p()
{
    // VCHAR as specified in Appendix B.1 of RFC 5234
    return QStringLiteral("[!-~]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::letDig_p()
{
    // Let-dig  as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return QStringLiteral("[A-Za-z0-9]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ldhStr_p()
{
    // Ldh-str  as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return QStringLiteral("(?:[A-Za-z0-9-]*%1)").arg(DKIMAuthenticationStatusInfoUtil::letDig_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::keyword_p()
{
    // "Keyword" as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return DKIMAuthenticationStatusInfoUtil::ldhStr_p();
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::subDomain_p()
{
    // sub-domain as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return QStringLiteral("(?:%1%2?)").arg(DKIMAuthenticationStatusInfoUtil::letDig_p(), DKIMAuthenticationStatusInfoUtil::ldhStr_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::obsFws_p()
{
    // obs-FWS as specified in Section 4.2 of RFC 5322
    return QStringLiteral("(?:%1+(?:\r\n%1+)*)").arg(DKIMAuthenticationStatusInfoUtil::wsp_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedPair_p()
{
    // quoted-pair as specified in Section 3.2.1 of RFC 5322
    // Note: obs-qp is not included, so this pattern matches less then specified!
    return QStringLiteral("(?:\\\\(?:%1|%2))").arg(vchar_p(), wsp_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::fws_p()
{
    // FWS as specified in Section 3.2.2 of RFC 5322
    return QStringLiteral("(?:(?:(?:%1*\r\n)?%1+)|%2)").arg(wsp_p(), obsFws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::fws_op()
{
    return QStringLiteral("%1?").arg(fws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ctext_p()
{
    // ctext as specified in Section 3.2.2 of RFC 5322
    return QStringLiteral("[!-'*-[\\]-~]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ccontent_p()
{
    // ccontent as specified in Section 3.2.2 of RFC 5322
    // Note: comment is not included, so this pattern matches less then specified!
    return QStringLiteral("(?:%1|%2)").arg(ctext_p(), quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::comment_p()
{
    // comment as specified in Section 3.2.2 of RFC 5322
    return QStringLiteral("\\((?:%1%2)*%1\\)").arg(fws_op(), ccontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::cfws_p()
{
    // CFWS as specified in Section 3.2.2 of RFC 5322 [MAIL]
    return QStringLiteral("(?:(?:(?:%1%2)+%1)|%3)").arg(fws_op(), comment_p(), fws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::cfws_op()
{
    return QStringLiteral("%1?").arg(cfws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::atext()
{
    // atext as specified in Section 3.2.3 of RFC 5322
    return QStringLiteral("[!#-'*-+/-9=?A-Z^-~-]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::dotAtomText_p()
{
    // dot-atom-text as specified in Section 3.2.3 of RFC 5322
    return QStringLiteral("(?:%1+(?:\\.%1+)*)").arg(atext());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::dotAtom_p()
{
    // dot-atom as specified in Section 3.2.3 of RFC 5322
    // dot-atom        =   [CFWS] dot-atom-text [CFWS]
    return QStringLiteral("(?:%1%2%1)").arg(cfws_op(), dotAtomText_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::qtext_p()
{
    // qtext as specified in Section 3.2.4 of RFC 5322
    // Note: obs-qtext is not included, so this pattern matches less then specified!
    return QStringLiteral("[!#-[\\]-~]");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::qcontent_p()
{
    // qcontent as specified in Section 3.2.4 of RFC 5322
    return QStringLiteral("(?:%1|%2)").arg(qtext_p(), quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_p()
{
    // quoted-string as specified in Section 3.2.4 of RFC 5322
    return QStringLiteral("(?:%1\"(?:%2%3)*%2\"%1)").arg(cfws_op(), fws_op(), qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_cp()
{
    return QStringLiteral("(?:%1\"((?:%2%3)*)%2\"%1)").arg(cfws_op(), fws_op(), qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::localPart_p()
{
    // local-part as specified in Section 3.4.1 of RFC 5322
    // Note: obs-local-part is not included, so this pattern matches less then specified!
    return QStringLiteral("(?:%1|%2))").arg(dotAtom_p(), quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::token_p()
{
    // token as specified in Section 5.1 of RFC 2045.
    return QStringLiteral("[^ \\x00-\\x1F\\x7F()<>@,;:\\\\\"/[\\]?=]+");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_p()
{
    // "value" as specified in Section 5.1 of RFC 2045.
    return QStringLiteral("(?:%1|%2)").arg(token_p(), quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_cp()
{
    return QStringLiteral("(?:(%1)|%2)").arg(token_p(), quotedString_cp());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::domainName_p()
{
    // domain-name as specified in Section 3.5 of RFC 6376 [DKIM].
    return QStringLiteral("(?:%1(?:\\.%1)+)").arg(subDomain_p());
}

// Tries to matches a pattern to the beginning of str.
//  Adds CFWS_op to the beginning of pattern.
//  pattern must be followed by string end, ";" or CFWS_p.
// If match is found, removes it from str.
QString MessageViewer::DKIMAuthenticationStatusInfoUtil::regexMatchO(const QString &regularExpressionStr)
{
    const QString regexp = (QLatin1Char('^') + DKIMAuthenticationStatusInfoUtil::cfws_op() + QStringLiteral("(?:") + regularExpressionStr + QLatin1Char(')')
                            + QStringLiteral("(?:(?:") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QStringLiteral("\r\n$)|(?=;)|(?=")
                            + DKIMAuthenticationStatusInfoUtil::cfws_p() + QStringLiteral("))"));
    return regexp;
}
