/* SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#ifndef QUOTEHTMLTEST_H
#define QUOTEHTMLTEST_H

#include <QObject>

class QuoteHtmlTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testQuoteHtml();
    void testQuoteHtml_data();
private:
    QString mCollapseIcon;
    QString mExpandIcon;
};

#endif
