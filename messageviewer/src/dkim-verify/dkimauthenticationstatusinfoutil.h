/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QString>

namespace MessageViewer
{
namespace DKIMAuthenticationStatusInfoUtil
{
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString wsp_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString vchar_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString letDig_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString ldhStr_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString keyword_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString subDomain_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString obsFws_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString quotedPair_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString fws_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString fws_op();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString ctext_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString ccontent_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString comment_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString cfws_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString cfws_op();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString atext();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString dotAtomText_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString dotAtom_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString qtext_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString qcontent_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString quotedString_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString quotedString_cp();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString localPart_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString token_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString value_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString value_cp();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString domainName_p();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString regexMatchO(const QString &regularExpressionStr);
}
}
