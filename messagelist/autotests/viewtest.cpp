/*
 *  SPDX-FileCopyrightText: 2026 Claudio Cambra <claudio.cambra@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "core/widgetbase.h"

#include <QTest>

#include <memory>

class ViewTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void destroysWidgetWithModelAndViewAdapter()
    {
        auto widget = std::make_unique<MessageList::Core::Widget>(nullptr);
        QVERIFY(widget);
        widget.reset();
    }
};

QTEST_MAIN(ViewTest)

#include "viewtest.moc"
