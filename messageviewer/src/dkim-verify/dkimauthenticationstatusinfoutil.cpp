/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoutil.h"

using namespace Qt::Literals::StringLiterals;

/*
// domain-name as specified in Section 3.5 of RFC 6376 [DKIM].
let domain_name_p = "(?:" + sub_domain_p + "(?:\\." + sub_domain_p + ")+)";
*/
QString MessageViewer::DKIMAuthenticationStatusInfoUtil::wsp_p()
{
    // WSP as specified in Appendix B.1 of RFC 5234
    return u"[ \t]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::vchar_p()
{
    // VCHAR as specified in Appendix B.1 of RFC 5234
    return u"[!-~]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::letDig_p()
{
    // Let-dig  as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return u"[A-Za-z0-9]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ldhStr_p()
{
    // Ldh-str  as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return u"(?:[A-Za-z0-9-]*%1)"_s.arg(DKIMAuthenticationStatusInfoUtil::letDig_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::keyword_p()
{
    // "Keyword" as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return DKIMAuthenticationStatusInfoUtil::ldhStr_p();
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::subDomain_p()
{
    // sub-domain as specified in Section 4.1.2 of RFC 5321 [SMTP].
    return u"(?:%1%2?)"_s.arg(DKIMAuthenticationStatusInfoUtil::letDig_p(), DKIMAuthenticationStatusInfoUtil::ldhStr_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::obsFws_p()
{
    // obs-FWS as specified in Section 4.2 of RFC 5322
    return u"(?:%1+(?:\r\n%1+)*)"_s.arg(DKIMAuthenticationStatusInfoUtil::wsp_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedPair_p()
{
    // quoted-pair as specified in Section 3.2.1 of RFC 5322
    // Note: obs-qp is not included, so this pattern matches less then specified!
    return u"(?:\\\\(?:%1|%2))"_s.arg(vchar_p(), wsp_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::fws_p()
{
    // FWS as specified in Section 3.2.2 of RFC 5322
    return u"(?:(?:(?:%1*\r\n)?%1+)|%2)"_s.arg(wsp_p(), obsFws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::fws_op()
{
    return u"%1?"_s.arg(fws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ctext_p()
{
    // ctext as specified in Section 3.2.2 of RFC 5322
    return u"[!-'*-[\\]-~]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::ccontent_p()
{
    // ccontent as specified in Section 3.2.2 of RFC 5322
    // Note: comment is not included, so this pattern matches less then specified!
    return u"(?:%1|%2)"_s.arg(ctext_p(), quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::comment_p()
{
    // comment as specified in Section 3.2.2 of RFC 5322
    return u"\\((?:%1%2)*%1\\)"_s.arg(fws_op(), ccontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::cfws_p()
{
    // CFWS as specified in Section 3.2.2 of RFC 5322 [MAIL]
    return u"(?:(?:(?:%1%2)+%1)|%3)"_s.arg(fws_op(), comment_p(), fws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::cfws_op()
{
    return u"%1?"_s.arg(cfws_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::atext()
{
    // atext as specified in Section 3.2.3 of RFC 5322
    return u"[!#-'*-+/-9=?A-Z^-~-]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::dotAtomText_p()
{
    // dot-atom-text as specified in Section 3.2.3 of RFC 5322
    return u"(?:%1+(?:\\.%1+)*)"_s.arg(atext());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::dotAtom_p()
{
    // dot-atom as specified in Section 3.2.3 of RFC 5322
    // dot-atom        =   [CFWS] dot-atom-text [CFWS]
    return u"(?:%1%2%1)"_s.arg(cfws_op(), dotAtomText_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::qtext_p()
{
    // qtext as specified in Section 3.2.4 of RFC 5322
    // Note: obs-qtext is not included, so this pattern matches less then specified!
    return u"[!#-[\\]-~]"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::qcontent_p()
{
    // qcontent as specified in Section 3.2.4 of RFC 5322
    return u"(?:%1|%2)"_s.arg(qtext_p(), quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_p()
{
    // quoted-string as specified in Section 3.2.4 of RFC 5322
    return u"(?:%1\"(?:%2%3)*%2\"%1)"_s.arg(cfws_op(), fws_op(), qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_cp()
{
    return u"(?:%1\"((?:%2%3)*)%2\"%1)"_s.arg(cfws_op(), fws_op(), qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::localPart_p()
{
    // local-part as specified in Section 3.4.1 of RFC 5322
    // Note: obs-local-part is not included, so this pattern matches less then specified!
    return u"(?:%1|%2))"_s.arg(dotAtom_p(), quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::token_p()
{
    // token as specified in Section 5.1 of RFC 2045.
    return u"[^ \\x00-\\x1F\\x7F()<>@,;:\\\\\"/[\\]?=]+"_s;
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_p()
{
    // "value" as specified in Section 5.1 of RFC 2045.
    return u"(?:%1|%2)"_s.arg(token_p(), quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_cp()
{
    return u"(?:(%1)|%2)"_s.arg(token_p(), quotedString_cp());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::domainName_p()
{
    // domain-name as specified in Section 3.5 of RFC 6376 [DKIM].
    return u"(?:%1(?:\\.%1)+)"_s.arg(subDomain_p());
}

// Tries to matches a pattern to the beginning of str.
//  Adds CFWS_op to the beginning of pattern.
//  pattern must be followed by string end, ";" or CFWS_p.
// If match is found, removes it from str.
QString MessageViewer::DKIMAuthenticationStatusInfoUtil::regexMatchO(const QString &regularExpressionStr)
{
    const QString regexp = (u'^' + DKIMAuthenticationStatusInfoUtil::cfws_op() + u"(?:"_s + regularExpressionStr + u')' + u"(?:(?:"_s
                            + DKIMAuthenticationStatusInfoUtil::cfws_op() + u"\r\n$)|(?=;)|(?="_s + DKIMAuthenticationStatusInfoUtil::cfws_p() + u"))"_s);
    return regexp;
}
