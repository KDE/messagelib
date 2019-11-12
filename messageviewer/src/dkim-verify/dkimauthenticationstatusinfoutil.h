/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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
MESSAGEVIEWER_TESTS_EXPORT QString wsp_p();
MESSAGEVIEWER_TESTS_EXPORT QString vchar_p();
MESSAGEVIEWER_TESTS_EXPORT QString letDig_p();
MESSAGEVIEWER_TESTS_EXPORT QString ldhStr_p();
MESSAGEVIEWER_TESTS_EXPORT QString keyword_p();
MESSAGEVIEWER_TESTS_EXPORT QString subDomain_p();
MESSAGEVIEWER_TESTS_EXPORT QString obsFws_p();
MESSAGEVIEWER_TESTS_EXPORT QString quotedPair_p();
MESSAGEVIEWER_TESTS_EXPORT QString fws_p();
MESSAGEVIEWER_TESTS_EXPORT QString fws_op();
MESSAGEVIEWER_TESTS_EXPORT QString ctext_p();
MESSAGEVIEWER_TESTS_EXPORT QString ccontent_p();
MESSAGEVIEWER_TESTS_EXPORT QString comment_p();
MESSAGEVIEWER_TESTS_EXPORT QString cfws_p();
MESSAGEVIEWER_TESTS_EXPORT QString cfws_op();
MESSAGEVIEWER_TESTS_EXPORT QString atext();
MESSAGEVIEWER_TESTS_EXPORT QString dotAtomText_p();
MESSAGEVIEWER_TESTS_EXPORT QString dotAtom_p();
MESSAGEVIEWER_TESTS_EXPORT QString qtext_p();
MESSAGEVIEWER_TESTS_EXPORT QString qcontent_p();
MESSAGEVIEWER_TESTS_EXPORT QString quotedString_p();
MESSAGEVIEWER_TESTS_EXPORT QString quotedString_cp();
MESSAGEVIEWER_TESTS_EXPORT QString localPart_p();
MESSAGEVIEWER_TESTS_EXPORT QString token_p();
MESSAGEVIEWER_TESTS_EXPORT QString value_p();
MESSAGEVIEWER_TESTS_EXPORT QString value_cp();
MESSAGEVIEWER_TESTS_EXPORT QString domainName_p();
}
}

#endif // DKIMAUTHENTICATIONSTATUSINFOUTIL_H
