/*
 *
 *  This file is part of KMail, the KDE mail client.
 *
 *  SPDX-FileCopyrightText: 2002-2003 Carsten Pfeiffer <pfeiffer@kde.org>
 *  SPDX-FileCopyrightText: 2003 Zack Rusin <zack@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <KSyntaxHighlighting/Repository>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <kpimtextedit/kpimtextedit-texttospeech.h>

namespace KPIMTextEdit
{
class SlideContainer;
}

namespace KPIMTextEditTextToSpeech
{
class TextToSpeechWidget;
class TextToSpeechInterface;
}

namespace MessageViewer
{
class FindBarSourceView;

/**
 * A tiny little class to use for displaying raw messages, textual
 * attachments etc.
 *
 * Auto-deletes itself when closed.
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class MailSourceViewTextBrowser;

class MailSourceViewTextBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MailSourceViewTextBrowserWidget(const QString &syntax, QWidget *parent = nullptr);

    void setText(const QString &text);
    void setPlainText(const QString &text);
    void setFixedFont();
    Q_REQUIRED_RESULT MessageViewer::MailSourceViewTextBrowser *textBrowser() const;

private:
    void slotFind();
    KSyntaxHighlighting::Repository mRepo;
    MailSourceViewTextBrowser *mTextBrowser = nullptr;
    FindBarSourceView *mFindBar = nullptr;
    KPIMTextEdit::SlideContainer *const mSliderContainer;
#if KPIMTEXTEDIT_TEXT_TO_SPEECH
    KPIMTextEditTextToSpeech::TextToSpeechWidget *const mTextToSpeechWidget;
#endif
};

class MailSourceViewTextBrowser : public QPlainTextEdit
{
    Q_OBJECT
public:
#if KPIMTEXTEDIT_TEXT_TO_SPEECH
    explicit MailSourceViewTextBrowser(KPIMTextEditTextToSpeech::TextToSpeechInterface *textToSpeechInterface, QWidget *parent = nullptr);
#endif
    explicit MailSourceViewTextBrowser(QWidget *parent);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
Q_SIGNALS:
    void findText();

private:
    void slotSpeakText();
    void slotSaveAs();
#if KPIMTEXTEDIT_TEXT_TO_SPEECH
    KPIMTextEditTextToSpeech::TextToSpeechInterface *mTextToSpeechInterface = nullptr;
#endif
};
}
