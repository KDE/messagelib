/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_private_export.h"
#include <QString>

namespace MessageCore
{
namespace DKIMAuthenticationStatusInfoUtil
{
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString wsp_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString vchar_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString letDig_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString ldhStr_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString keyword_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString subDomain_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString obsFws_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString quotedPair_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString fws_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString fws_op();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString ctext_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString ccontent_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString comment_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString cfws_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString cfws_op();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString atext();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString dotAtomText_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString dotAtom_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString qtext_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString qcontent_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString quotedString_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString quotedString_cp();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString localPart_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString token_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString value_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString value_cp();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString domainName_p();
[[nodiscard]] MESSAGECORE_TESTS_EXPORT QString regexMatchO(const QString &regularExpressionStr);
}
}
