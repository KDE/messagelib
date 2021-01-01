/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMAUTHENTICATIONSTATUSINFOTEST_H
#define DKIMAUTHENTICATIONSTATUSINFOTEST_H

#include <QObject>

class DKIMAuthenticationStatusInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMAuthenticationStatusInfoTest(QObject *parent = nullptr);
    ~DKIMAuthenticationStatusInfoTest() = default;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldParseKey();
    void shouldParseKey_data();
};

#endif // DKIMAUTHENTICATIONSTATUSINFOTEST_H
