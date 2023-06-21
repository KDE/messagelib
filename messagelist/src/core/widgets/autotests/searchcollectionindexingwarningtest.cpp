/*
  SPDX-FileCopyrightText: 2016-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "searchcollectionindexingwarningtest.h"
#include "../src/core/widgets/searchcollectionindexingwarning.h"
#include <QTest>

SearchCollectionIndexingWarningTest::SearchCollectionIndexingWarningTest(QObject *parent)
    : QObject(parent)
{
}

SearchCollectionIndexingWarningTest::~SearchCollectionIndexingWarningTest() = default;

void SearchCollectionIndexingWarningTest::shouldHaveDefaultValue()
{
    MessageList::Core::SearchCollectionIndexingWarning w;
    QVERIFY(!w.isVisible());
    QVERIFY(w.wordWrap());
}

QTEST_MAIN(SearchCollectionIndexingWarningTest)

#include "moc_searchcollectionindexingwarningtest.cpp"
