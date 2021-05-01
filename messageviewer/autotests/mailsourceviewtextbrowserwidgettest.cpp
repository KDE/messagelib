/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailsourceviewtextbrowserwidgettest.h"
#include "../src/findbar/findbarsourceview.h"
#include "../src/widgets/mailsourceviewtextbrowserwidget.h"
#include <KPIMTextEdit/TextToSpeechWidget>
#include <QTest>

MailSourceViewTextBrowserWidgetTest::MailSourceViewTextBrowserWidgetTest(QObject *parent)
    : QObject(parent)
{
}

MailSourceViewTextBrowserWidgetTest::~MailSourceViewTextBrowserWidgetTest()
{
}

void MailSourceViewTextBrowserWidgetTest::shouldHaveDefaultValue()
{
    MessageViewer::MailSourceViewTextBrowserWidget widget(QStringLiteral("Email"));

    auto textbrowser = widget.findChild<MessageViewer::MailSourceViewTextBrowser *>(QStringLiteral("textbrowser"));
    QVERIFY(textbrowser);
    QVERIFY(!textbrowser->isHidden());
    auto findbar = widget.findChild<MessageViewer::FindBarSourceView *>(QStringLiteral("findbar"));
    QVERIFY(findbar);
    QVERIFY(findbar->isHidden());

    auto texttospeechwidget = widget.findChild<KPIMTextEdit::TextToSpeechWidget *>(QStringLiteral("texttospeech"));
    QVERIFY(texttospeechwidget);
    QVERIFY(texttospeechwidget->isHidden());
}

QTEST_MAIN(MailSourceViewTextBrowserWidgetTest)
