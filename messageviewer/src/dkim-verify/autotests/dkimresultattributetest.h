/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRESULTATTRIBUTETEST_H
#define DKIMRESULTATTRIBUTETEST_H

#include <QObject>

class DKIMResultAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMResultAttributeTest(QObject *parent = nullptr);
    ~DKIMResultAttributeTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldCloneAttribute();
    void shouldHaveType();
    void shouldDeserializeValue();
};

#endif // DKIMRESULTATTRIBUTETEST_H
