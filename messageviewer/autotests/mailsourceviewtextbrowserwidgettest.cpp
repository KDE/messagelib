/*
  SPDX-FileCopyrightText: 2014-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailsourceviewtextbrowserwidgettest.h"
#include "../src/findbar/findbarsourceview.h"
#include "../src/widgets/mailsourceviewtextbrowserwidget.h"
#if KPIMTEXTEDIT_TEXT_TO_SPEECH
#include <KPIMTextEditTextToSpeech/TextToSpeechContainerWidget>
#endif
#include <QTest>

MailSourceViewTextBrowserWidgetTest::MailSourceViewTextBrowserWidgetTest(QObject *parent)
    : QObject(parent)
{
}

MailSourceViewTextBrowserWidgetTest::~MailSourceViewTextBrowserWidgetTest() = default;

void MailSourceViewTextBrowserWidgetTest::shouldHaveDefaultValue()
{
    MessageViewer::MailSourceViewTextBrowserWidget widget(QStringLiteral("Email"));

    auto textbrowser = widget.findChild<MessageViewer::MailSourceViewTextBrowser *>(QStringLiteral("textbrowser"));
    QVERIFY(textbrowser);
    QVERIFY(!textbrowser->isHidden());
    auto findbar = widget.findChild<MessageViewer::FindBarSourceView *>(QStringLiteral("findbar"));
    QVERIFY(findbar);
    QVERIFY(findbar->isHidden());

#if KPIMTEXTEDIT_TEXT_TO_SPEECH
    auto TextToSpeechContainerWidget = widget.findChild<KPIMTextEditTextToSpeech::TextToSpeechContainerWidget *>(QStringLiteral("texttospeech"));
    QVERIFY(TextToSpeechContainerWidget);
    QVERIFY(TextToSpeechContainerWidget->isHidden());
#endif
}

QTEST_MAIN(MailSourceViewTextBrowserWidgetTest)
