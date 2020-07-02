/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEPARSEREMAILADDRESSREQUESTERLINEEDITTEST_H
#define TEMPLATEPARSEREMAILADDRESSREQUESTERLINEEDITTEST_H

#include <QObject>

class TemplateParserEmailAddressRequesterLineEditTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterLineEditTest(QObject *parent = nullptr);
    ~TemplateParserEmailAddressRequesterLineEditTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAssignValue();
    void shouldClearValue();
    void shouldEmitSignal();
};

#endif // TEMPLATEPARSEREMAILADDRESSREQUESTERLINEEDITTEST_H
