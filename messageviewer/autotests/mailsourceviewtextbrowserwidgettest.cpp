/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailsourceviewtextbrowserwidgettest.h"
#include "../src/widgets/mailsourceviewtextbrowserwidget.h"
#include "../src/findbar/findbarsourceview.h"
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

    MessageViewer::MailSourceViewTextBrowser *textbrowser
        = widget.findChild<MessageViewer::MailSourceViewTextBrowser *>(QStringLiteral("textbrowser"));
    QVERIFY(textbrowser);
    QVERIFY(!textbrowser->isHidden());
    MessageViewer::FindBarSourceView *findbar
        = widget.findChild<MessageViewer::FindBarSourceView *>(QStringLiteral("findbar"));
    QVERIFY(findbar);
    QVERIFY(findbar->isHidden());

    KPIMTextEdit::TextToSpeechWidget *texttospeechwidget
        = widget.findChild<KPIMTextEdit::TextToSpeechWidget *>(QStringLiteral("texttospeech"));
    QVERIFY(texttospeechwidget);
    QVERIFY(texttospeechwidget->isHidden());
}

QTEST_MAIN(MailSourceViewTextBrowserWidgetTest)
