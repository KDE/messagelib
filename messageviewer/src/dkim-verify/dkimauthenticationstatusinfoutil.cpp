/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

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
    return QStringLiteral("(?:%1%2?)").arg(DKIMAuthenticationStatusInfoUtil::letDig_p()).arg(DKIMAuthenticationStatusInfoUtil::ldhStr_p());
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
    return QStringLiteral("(?:\\\\(?:%1|%2))").arg(vchar_p()).arg(wsp_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::fws_p()
{
    // FWS as specified in Section 3.2.2 of RFC 5322
    return QStringLiteral("(?:(?:(?:%1*\r\n)?%1+)|%2)").arg(wsp_p()).arg(obsFws_p());
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
    return QStringLiteral("(?:%1|%2)").arg(ctext_p()).arg(quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::comment_p()
{
    // comment as specified in Section 3.2.2 of RFC 5322
    return QStringLiteral("\\((?:%1%2)*%1\\)").arg(fws_op()).arg(ccontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::cfws_p()
{
    // CFWS as specified in Section 3.2.2 of RFC 5322 [MAIL]
    return QStringLiteral("(?:(?:(?:%1%2)+%1)|%3)").arg(fws_op()).arg(comment_p()).arg(fws_p());
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
    return QStringLiteral("(?:%1%2%1)").arg(cfws_op()).arg(dotAtomText_p());
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
    return QStringLiteral("(?:%1|%2)").arg(qtext_p()).arg(quotedPair_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_p()
{
    // quoted-string as specified in Section 3.2.4 of RFC 5322
    return QStringLiteral("(?:%1\"(?:%2%3)*%2\"%1)").arg(cfws_op()).arg(fws_op()).arg(qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::quotedString_cp()
{
    return QStringLiteral("(?:%1\"((?:%2%3)*)%2\"%1)").arg(cfws_op()).arg(fws_op()).arg(qcontent_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::localPart_p()
{
    // local-part as specified in Section 3.4.1 of RFC 5322
    // Note: obs-local-part is not included, so this pattern matches less then specified!
    return QStringLiteral("(?:%1|%2))").arg(dotAtom_p()).arg(quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::token_p()
{
    // token as specified in Section 5.1 of RFC 2045.
    return QStringLiteral("[^ \\x00-\\x1F\\x7F()<>@,;:\\\\\"/[\\]?=]+");
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_p()
{
    // "value" as specified in Section 5.1 of RFC 2045.
    return QStringLiteral("(?:%1|%2)").arg(token_p()).arg(quotedString_p());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::value_cp()
{
    return QStringLiteral("(?:(%1)|%2)").arg(token_p()).arg(quotedString_cp());
}

QString MessageViewer::DKIMAuthenticationStatusInfoUtil::domainName_p()
{
    // domain-name as specified in Section 3.5 of RFC 6376 [DKIM].
    return QStringLiteral("(?:%1(?:\\.%1)+)").arg(subDomain_p());
}
