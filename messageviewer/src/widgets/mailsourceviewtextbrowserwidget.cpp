/*
 *
 *  This file is part of KMail, the KDE mail client.
 *
 *  SPDX-FileCopyrightText: 2002-2003 Carsten Pfeiffer <pfeiffer@kde.org>
 *  SPDX-FileCopyrightText: 2003 Zack Rusin <zack@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "mailsourceviewtextbrowserwidget.h"
#include "findbar/findbarsourceview.h"
#include "messageviewer/messageviewerutil.h"
#include "messageviewer_debug.h"
#include <TextAddonsWidgets/SlideContainer>
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
#include <TextEditTextToSpeech/TextToSpeechContainerWidget>
#endif
#include <PimCommon/PimUtil>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

#include <KLocalizedString>
#include <KStandardAction>
#include <QAction>
#include <QIcon>

#include <QContextMenuEvent>
#include <QShortcut>
#include <QVBoxLayout>

#include <QFontDatabase>
#include <QMenu>
#include <QPushButton>

using namespace MessageViewer;
MailSourceViewTextBrowserWidget::MailSourceViewTextBrowserWidget(const QString &syntax, QWidget *parent)
    : QWidget(parent)
    , mSliderContainer(new TextAddonsWidgets::SlideContainer(this))
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    , mTextToSpeechContainerWidget(new TextEditTextToSpeech::TextToSpeechContainerWidget(this))
#endif
{
    auto lay = new QVBoxLayout(this);
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    lay->setContentsMargins({});
    lay->setSpacing(0);
    mTextToSpeechContainerWidget->setObjectName(QLatin1StringView("texttospeech"));
    lay->addWidget(mTextToSpeechContainerWidget);
    mTextBrowser = new MailSourceViewTextBrowser(mTextToSpeechContainerWidget);
#else
    mTextBrowser = new MailSourceViewTextBrowser(this);
#endif
    mTextBrowser->setObjectName(QLatin1StringView("textbrowser"));
    mTextBrowser->setLineWrapMode(QPlainTextEdit::NoWrap);
    mTextBrowser->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    const KSyntaxHighlighting::Definition def = mRepo.definitionForName(syntax);
    if (!def.isValid()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Invalid definition name";
    }

    auto hl = new KSyntaxHighlighting::SyntaxHighlighter(mTextBrowser->document());
    hl->setTheme((palette().color(QPalette::Base).lightness() < 128) ? mRepo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                                                                     : mRepo.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
    hl->setDefinition(def);

    connect(mTextBrowser, &MailSourceViewTextBrowser::findText, this, &MailSourceViewTextBrowserWidget::slotFind);
    lay->addWidget(mTextBrowser);

    mFindBar = new FindBarSourceView(mTextBrowser, this);
    mFindBar->setObjectName(QLatin1StringView("findbar"));
    connect(mFindBar, &FindBarSourceView::hideFindBar, mSliderContainer, &TextAddonsWidgets::SlideContainer::slideOut);
    mSliderContainer->setContent(mFindBar);

    lay->addWidget(mSliderContainer);
    auto shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_F | Qt::CTRL);
    connect(shortcut, &QShortcut::activated, this, &MailSourceViewTextBrowserWidget::slotFind);
}

void MailSourceViewTextBrowserWidget::slotFind()
{
    if (mTextBrowser->textCursor().hasSelection()) {
        mFindBar->setText(mTextBrowser->textCursor().selectedText());
    }
    mSliderContainer->slideIn();
    mFindBar->focusAndSetCursor();
}

void MailSourceViewTextBrowserWidget::setText(const QString &text)
{
    mTextBrowser->setPlainText(text);
}

void MailSourceViewTextBrowserWidget::setPlainText(const QString &text)
{
    mTextBrowser->setPlainText(text);
}

void MailSourceViewTextBrowserWidget::setFixedFont()
{
    mTextBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

MessageViewer::MailSourceViewTextBrowser *MailSourceViewTextBrowserWidget::textBrowser() const
{
    return mTextBrowser;
}
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
MailSourceViewTextBrowser::MailSourceViewTextBrowser(TextEditTextToSpeech::TextToSpeechContainerWidget *TextToSpeechContainerWidget, QWidget *parent)
    : QPlainTextEdit(parent)
    , mTextToSpeechContainerWidget(TextToSpeechContainerWidget)
{
}
#endif
MailSourceViewTextBrowser::MailSourceViewTextBrowser(QWidget *parent)
    : QPlainTextEdit(parent)
{
}

void MailSourceViewTextBrowser::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *popup = createStandardContextMenu();
    if (popup) {
        popup->addSeparator();
        popup->addAction(KStandardAction::find(this, &MailSourceViewTextBrowser::findText, this));
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
        popup->addSeparator();
        popup->addAction(QIcon::fromTheme(QStringLiteral("preferences-desktop-text-to-speech")),
                         i18n("Speak Text"),
                         this,
                         &MailSourceViewTextBrowser::slotSpeakText);
#endif
        popup->addSeparator();
        popup->addAction(KStandardAction::saveAs(this, &MailSourceViewTextBrowser::slotSaveAs, this));

        popup->exec(event->globalPos());
        delete popup;
    }
}

void MailSourceViewTextBrowser::slotSaveAs()
{
    PimCommon::Util::saveTextAs(toPlainText(), QString(), this);
}

void MailSourceViewTextBrowser::slotSpeakText()
{
    QString text;
    if (textCursor().hasSelection()) {
        text = textCursor().selectedText();
    } else {
        text = toPlainText();
    }
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    mTextToSpeechContainerWidget->say(text);
#endif
}

#include "moc_mailsourceviewtextbrowserwidget.cpp"
