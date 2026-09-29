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
[[nodiscard]] MESSAGECORE_EXPORT QString wsp_p();
[[nodiscard]] MESSAGECORE_EXPORT QString vchar_p();
[[nodiscard]] MESSAGECORE_EXPORT QString letDig_p();
[[nodiscard]] MESSAGECORE_EXPORT QString ldhStr_p();
[[nodiscard]] MESSAGECORE_EXPORT QString keyword_p();
[[nodiscard]] MESSAGECORE_EXPORT QString subDomain_p();
[[nodiscard]] MESSAGECORE_EXPORT QString obsFws_p();
[[nodiscard]] MESSAGECORE_EXPORT QString quotedPair_p();
[[nodiscard]] MESSAGECORE_EXPORT QString fws_p();
[[nodiscard]] MESSAGECORE_EXPORT QString fws_op();
[[nodiscard]] MESSAGECORE_EXPORT QString ctext_p();
[[nodiscard]] MESSAGECORE_EXPORT QString ccontent_p();
[[nodiscard]] MESSAGECORE_EXPORT QString comment_p();
[[nodiscard]] MESSAGECORE_EXPORT QString cfws_p();
[[nodiscard]] MESSAGECORE_EXPORT QString cfws_op();
[[nodiscard]] MESSAGECORE_EXPORT QString atext();
[[nodiscard]] MESSAGECORE_EXPORT QString dotAtomText_p();
[[nodiscard]] MESSAGECORE_EXPORT QString dotAtom_p();
[[nodiscard]] MESSAGECORE_EXPORT QString qtext_p();
[[nodiscard]] MESSAGECORE_EXPORT QString qcontent_p();
[[nodiscard]] MESSAGECORE_EXPORT QString quotedString_p();
[[nodiscard]] MESSAGECORE_EXPORT QString quotedString_cp();
[[nodiscard]] MESSAGECORE_EXPORT QString localPart_p();
[[nodiscard]] MESSAGECORE_EXPORT QString token_p();
[[nodiscard]] MESSAGECORE_EXPORT QString value_p();
[[nodiscard]] MESSAGECORE_EXPORT QString value_cp();
[[nodiscard]] MESSAGECORE_EXPORT QString domainName_p();
[[nodiscard]] MESSAGECORE_EXPORT QString regexMatchO(const QString &regularExpressionStr);
}
}
