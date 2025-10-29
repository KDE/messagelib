/*
  SPDX-FileCopyrightText: 2014-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailsourceviewtextbrowserwidgettest.h"
#include "config-messageviewer.h"

using namespace Qt::Literals::StringLiterals;

#include "../src/findbar/findbarsourceview.h"
#include "../src/widgets/mailsourceviewtextbrowserwidget.h"
#if HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
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
    MessageViewer::MailSourceViewTextBrowserWidget widget(u"Email"_s);

    auto textbrowser = widget.findChild<MessageViewer::MailSourceViewTextBrowser *>(u"textbrowser"_s);
    QVERIFY(textbrowser);
    QVERIFY(!textbrowser->isHidden());
    auto findbar = widget.findChild<MessageViewer::FindBarSourceView *>(u"findbar"_s);
    QVERIFY(findbar);
    QVERIFY(findbar->isHidden());

#if HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    auto TextToSpeechContainerWidget = widget.findChild<TextEditTextToSpeech::TextToSpeechContainerWidget *>(u"texttospeech"_s);
    QVERIFY(TextToSpeechContainerWidget);
    QVERIFY(TextToSpeechContainerWidget->isHidden());
#endif
}

QTEST_MAIN(MailSourceViewTextBrowserWidgetTest)

#include "moc_mailsourceviewtextbrowserwidgettest.cpp"
