/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEPARSEREMAILADDRESSREQUESTERINTERFACEWIDGETTEST_H
#define TEMPLATEPARSEREMAILADDRESSREQUESTERINTERFACEWIDGETTEST_H

#include <QObject>

class TemplateParserEmailAddressRequesterInterfaceWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterInterfaceWidgetTest(QObject *parent = nullptr);
    ~TemplateParserEmailAddressRequesterInterfaceWidgetTest() = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAssignValues();
};

#endif // TEMPLATEPARSEREMAILADDRESSREQUESTERINTERFACEWIDGETTEST_H
