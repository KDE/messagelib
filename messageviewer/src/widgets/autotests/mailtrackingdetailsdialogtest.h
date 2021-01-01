/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRACKINGDETAILSDIALOGTEST_H
#define MAILTRACKINGDETAILSDIALOGTEST_H

#include <QObject>

class MailTrackingDetailsDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit MailTrackingDetailsDialogTest(QObject *parent = nullptr);
    ~MailTrackingDetailsDialogTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // MAILTRACKINGDETAILSDIALOGTEST_H
