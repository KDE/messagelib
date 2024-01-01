/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoutiltest.h"
#include "dkim-verify/dkimauthenticationstatusinfoutil.h"
#include <QTest>
QTEST_GUILESS_MAIN(DKIMAuthenticationStatusInfoUtilTest)
DKIMAuthenticationStatusInfoUtilTest::DKIMAuthenticationStatusInfoUtilTest(QObject *parent)
    : QObject(parent)
{
}

#include "moc_dkimauthenticationstatusinfoutiltest.cpp"
