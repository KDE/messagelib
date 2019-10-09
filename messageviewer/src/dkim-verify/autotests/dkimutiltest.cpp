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

#include "dkimutiltest.h"
#include "dkim-verify/dkimutil.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMUtilTest)
DKIMUtilTest::DKIMUtilTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMUtilTest::shouldTestBodyCanonizationRelaxed()
{
    QString ba = QStringLiteral("-- \nLaurent Montel | laurent.montel@kdab.com | KDE/Qt Senior Software Engineer \nKDAB (France) S.A.S., a KDAB Group company\nTel: France +33 (0)4 90 84 08 53, http://www.kdab.fr\nKDAB - The Qt, C++ and OpenGL Experts\n\n\n");
    QString result = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(ba);

    QCOMPARE(MessageViewer::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "jnEyWN7LwPIBgES0mElYDek3lmyrRtSwUjD R2Ge08Xw=");
}
