/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "statusbarlabeltoggledstatetest.h"
#include "MessageComposer/StatusBarLabelToggledState"
#include <QSignalSpy>
#include <QTest>
#include <qtestmouse.h>

QTEST_MAIN(StatusBarLabelToggledStateTest)

StatusBarLabelToggledStateTest::StatusBarLabelToggledStateTest(QObject *parent)
    : QObject(parent)
{
}

StatusBarLabelToggledStateTest::~StatusBarLabelToggledStateTest() = default;

void StatusBarLabelToggledStateTest::shouldHasDefaultValue()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    QVERIFY(!widget.toggleMode());
}

void StatusBarLabelToggledStateTest::shouldChangeState()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    widget.setToggleMode(true);
    QVERIFY(widget.toggleMode());
    QVERIFY(!widget.text().isEmpty());

    widget.setToggleMode(true);
    QVERIFY(widget.toggleMode());

    widget.setToggleMode(false);
    QVERIFY(!widget.toggleMode());
}

void StatusBarLabelToggledStateTest::shouldEmitSignalWhenChangeState()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    QSignalSpy spy(&widget, &MessageComposer::StatusBarLabelToggledState::toggleModeChanged);
    widget.setToggleMode(true);
    QCOMPARE(spy.count(), 1);

    widget.setToggleMode(false);
    QCOMPARE(spy.count(), 2);
}

void StatusBarLabelToggledStateTest::shouldNotEmitSignalWhenWeDontChangeState()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    QSignalSpy spy(&widget, &MessageComposer::StatusBarLabelToggledState::toggleModeChanged);
    widget.setToggleMode(false);
    QCOMPARE(spy.count(), 0);

    widget.setToggleMode(true);
    QCOMPARE(spy.count(), 1);

    widget.setToggleMode(true);
    QCOMPARE(spy.count(), 1);
}

void StatusBarLabelToggledStateTest::shouldEmitSignalWhenClickOnLabel()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    QSignalSpy spy(&widget, &MessageComposer::StatusBarLabelToggledState::toggleModeChanged);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    QTest::mouseClick(&widget, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);

    QTest::mouseClick(&widget, Qt::LeftButton);
    QCOMPARE(spy.count(), 2);
}

void StatusBarLabelToggledStateTest::shouldChangeTestWhenStateChanged()
{
    MessageComposer::StatusBarLabelToggledState widget;
    widget.setStateString(QStringLiteral("toggle"), QStringLiteral("untoggle"));
    const QString initialText = widget.text();
    widget.setToggleMode(true);
    const QString newText = widget.text();
    QVERIFY(initialText != newText);

    widget.setToggleMode(false);
    QCOMPARE(widget.text(), initialText);

    widget.setToggleMode(true);
    QCOMPARE(widget.text(), newText);
}

#include "moc_statusbarlabeltoggledstatetest.cpp"
