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
#include <KPIMTextEdit/SlideContainer>
#include <KPIMTextEdit/TextToSpeechInterface>
#include <KPIMTextEdit/TextToSpeechWidget>
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
    , mSliderContainer(new KPIMTextEdit::SlideContainer(this))
    , mTextToSpeechWidget(new KPIMTextEdit::TextToSpeechWidget(this))
{
    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins({});
    mTextToSpeechWidget->setObjectName(QStringLiteral("texttospeech"));
    lay->addWidget(mTextToSpeechWidget);

    auto textToSpeechInterface = new KPIMTextEdit::TextToSpeechInterface(mTextToSpeechWidget, this);

    mTextBrowser = new MailSourceViewTextBrowser(textToSpeechInterface);
    mTextBrowser->setObjectName(QStringLiteral("textbrowser"));
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
    mFindBar->setObjectName(QStringLiteral("findbar"));
    connect(mFindBar, &FindBarSourceView::hideFindBar, mSliderContainer, &KPIMTextEdit::SlideContainer::slideOut);
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

MailSourceViewTextBrowser::MailSourceViewTextBrowser(KPIMTextEdit::TextToSpeechInterface *textToSpeechInterface, QWidget *parent)
    : QPlainTextEdit(parent)
    , mTextToSpeechInterface(textToSpeechInterface)
{
}

void MailSourceViewTextBrowser::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *popup = createStandardContextMenu();
    if (popup) {
        popup->addSeparator();
        popup->addAction(KStandardAction::find(this, &MailSourceViewTextBrowser::findText, this));
        // Code from KTextBrowser
        if (mTextToSpeechInterface->isReady()) {
            popup->addSeparator();
            popup->addAction(QIcon::fromTheme(QStringLiteral("preferences-desktop-text-to-speech")),
                             i18n("Speak Text"),
                             this,
                             &MailSourceViewTextBrowser::slotSpeakText);
        }
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
    mTextToSpeechInterface->say(text);
}
