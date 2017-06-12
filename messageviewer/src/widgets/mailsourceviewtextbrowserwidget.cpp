/*
 *
 *  This file is part of KMail, the KDE mail client.
 *
 *  Copyright (c) 2002-2003 Carsten Pfeiffer <pfeiffer@kde.org>
 *  Copyright (c) 2003      Zack Rusin <zack@kde.org>
 *
 *  KMail is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License, version 2, as
 *  published by the Free Software Foundation.
 *
 *  KMail is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of this program with any edition of
 *  the Qt library by Trolltech AS, Norway (or with modified versions
 *  of Qt that use the same license as Qt), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt.  If you modify this file, you may extend this exception to
 *  your version of the file, but you are not obligated to do so.  If
 *  you do not wish to do so, delete this exception statement from
 *  your version.
 */

#include "mailsourceviewtextbrowserwidget.h"
#include "messageviewer/messageviewerutil.h"
#include "findbar/findbarsourceview.h"
#include "kpimtextedit/slidecontainer.h"
#include "PimCommon/PimUtil"
#include "kpimtextedit/texttospeechwidget.h"
#include "kpimtextedit/texttospeechinterface.h"

#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <KLocalizedString>
#include <KStandardAction>
#include <QAction>
#include <QIcon>
#include <KIconTheme>

#include <QShortcut>
#include <QVBoxLayout>
#include <QContextMenuEvent>

#include <QMenu>
#include <QFontDatabase>
#include <QPushButton>

using namespace MessageViewer;
MailSourceViewTextBrowserWidget::MailSourceViewTextBrowserWidget(const QString &syntax,
                                                                 QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setMargin(0);
    mTextToSpeechWidget = new KPIMTextEdit::TextToSpeechWidget;
    mTextToSpeechWidget->setObjectName(QStringLiteral("texttospeech"));
    lay->addWidget(mTextToSpeechWidget);

    KPIMTextEdit::TextToSpeechInterface *textToSpeechInterface
        = new KPIMTextEdit::TextToSpeechInterface(mTextToSpeechWidget, this);

    mTextBrowser = new MailSourceViewTextBrowser(textToSpeechInterface);
    mTextBrowser->setObjectName(QStringLiteral("textbrowser"));
    mTextBrowser->setLineWrapMode(QPlainTextEdit::NoWrap);
    mTextBrowser->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    KSyntaxHighlighting::Definition def;
    def = mRepo.definitionForName(syntax);

    KSyntaxHighlighting::SyntaxHighlighter *hl = new KSyntaxHighlighting::SyntaxHighlighter(
        mTextBrowser->document());
    hl->setTheme((palette().color(QPalette::Base).lightness() < 128)
                 ? mRepo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                 : mRepo.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
    hl->setDefinition(def);

    connect(mTextBrowser, &MailSourceViewTextBrowser::findText, this,
            &MailSourceViewTextBrowserWidget::slotFind);
    lay->addWidget(mTextBrowser);
    mSliderContainer = new KPIMTextEdit::SlideContainer(this);

    mFindBar = new FindBarSourceView(mTextBrowser, this);
    mFindBar->setObjectName(QStringLiteral("findbar"));
    connect(mFindBar, &FindBarSourceView::hideFindBar, mSliderContainer,
            &KPIMTextEdit::SlideContainer::slideOut);
    mSliderContainer->setContent(mFindBar);

    lay->addWidget(mSliderContainer);
    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_F + Qt::CTRL);
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

MailSourceViewTextBrowser::MailSourceViewTextBrowser(
    KPIMTextEdit::TextToSpeechInterface *textToSpeechInterface, QWidget *parent)
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
        //Code from KTextBrowser
        KIconTheme::assignIconsToContextMenu(isReadOnly() ? KIconTheme::ReadOnlyText
                                             : KIconTheme::TextEditor,
                                             popup->actions());
        if (mTextToSpeechInterface->isReady()) {
            popup->addSeparator();
            popup->addAction(QIcon::fromTheme(QStringLiteral(
                                                  "preferences-desktop-text-to-speech")), i18n(
                                 "Speak Text"), this, &MailSourceViewTextBrowser::slotSpeakText);
        }
        popup->addSeparator();
        popup->addAction(KStandardAction::saveAs(this, &MailSourceViewTextBrowser::slotSaveAs,
                                                 this));

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
