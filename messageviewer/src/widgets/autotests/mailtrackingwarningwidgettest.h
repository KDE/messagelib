/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRACKINGWARNINGWIDGETTEST_H
#define MAILTRACKINGWARNINGWIDGETTEST_H

#include <QObject>

class MailTrackingWarningWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit MailTrackingWarningWidgetTest(QObject *parent = nullptr);
    ~MailTrackingWarningWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // MAILTRACKINGWARNINGWIDGETTEST_H
