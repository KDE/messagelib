/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailsourceviewtextbrowserwidgettest.h"
#include "../src/findbar/findbarsourceview.h"
#include "../src/widgets/mailsourceviewtextbrowserwidget.h"
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
#include <TextEditTextToSpeech/TextToSpeechContainerWidget>
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

#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    auto TextToSpeechContainerWidget = widget.findChild<TextEditTextToSpeech::TextToSpeechContainerWidget *>(QStringLiteral("texttospeech"));
    QVERIFY(TextToSpeechContainerWidget);
    QVERIFY(TextToSpeechContainerWidget->isHidden());
#endif
}

QTEST_MAIN(MailSourceViewTextBrowserWidgetTest)

#include "moc_mailsourceviewtextbrowserwidgettest.cpp"
