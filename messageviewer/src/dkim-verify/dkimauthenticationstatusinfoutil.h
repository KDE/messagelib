/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#ifndef DKIMAUTHENTICATIONSTATUSINFOUTIL_H
#define DKIMAUTHENTICATIONSTATUSINFOUTIL_H

#include "messageviewer_private_export.h"
#include <QString>

namespace MessageViewer {
namespace DKIMAuthenticationStatusInfoUtil {
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString wsp_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString vchar_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString letDig_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString ldhStr_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString keyword_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString subDomain_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString obsFws_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString quotedPair_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString fws_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString fws_op();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString ctext_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString ccontent_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString comment_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString cfws_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString cfws_op();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString atext();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString dotAtomText_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString dotAtom_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString qtext_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString qcontent_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString quotedString_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString quotedString_cp();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString localPart_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString token_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString value_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString value_cp();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString domainName_p();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString regexMatchO(const QString &regularExpressionStr);
}
}

#endif // DKIMAUTHENTICATIONSTATUSINFOUTIL_H
