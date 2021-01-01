/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMAUTHENTICATIONSTATUSINFOCONVERTERTEST_H
#define DKIMAUTHENTICATIONSTATUSINFOCONVERTERTEST_H

#include <QObject>

class DKIMAuthenticationStatusInfoConverterTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMAuthenticationStatusInfoConverterTest(QObject *parent = nullptr);
    ~DKIMAuthenticationStatusInfoConverterTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMAUTHENTICATIONSTATUSINFOCONVERTERTEST_H
