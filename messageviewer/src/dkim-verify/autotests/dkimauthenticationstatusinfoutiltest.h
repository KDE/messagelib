/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMAUTHENTICATIONSTATUSINFOUTILTEST_H
#define DKIMAUTHENTICATIONSTATUSINFOUTILTEST_H

#include <QObject>

class DKIMAuthenticationStatusInfoUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMAuthenticationStatusInfoUtilTest(QObject *parent = nullptr);
    ~DKIMAuthenticationStatusInfoUtilTest() = default;
};

#endif // DKIMAUTHENTICATIONSTATUSINFOUTILTEST_H
