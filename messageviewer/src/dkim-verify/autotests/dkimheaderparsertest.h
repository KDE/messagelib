/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMHEADERPARSERTEST_H
#define DKIMHEADERPARSERTEST_H

#include <QObject>

class DKIMHeaderParserTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMHeaderParserTest(QObject *parent = nullptr);
    ~DKIMHeaderParserTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldChangeWasAlreadyParsedBoolean();
    void shouldAssignElement();
};

#endif // DKIMHEADERPARSERTEST_H
