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
namespace TextAddonsWidgets
{
class SlideContainer;
}

#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
namespace TextEditTextToSpeech
{
class TextToSpeechContainerWidget;
}
#endif

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
    [[nodiscard]] MessageViewer::MailSourceViewTextBrowser *textBrowser() const;

private:
    void slotFind();
    KSyntaxHighlighting::Repository mRepo;
    MailSourceViewTextBrowser *mTextBrowser = nullptr;
    FindBarSourceView *mFindBar = nullptr;
    TextAddonsWidgets::SlideContainer *const mSliderContainer;
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    TextEditTextToSpeech::TextToSpeechContainerWidget *const mTextToSpeechContainerWidget;
#endif
};

class MailSourceViewTextBrowser : public QPlainTextEdit
{
    Q_OBJECT
public:
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    explicit MailSourceViewTextBrowser(TextEditTextToSpeech::TextToSpeechContainerWidget *TextToSpeechContainerWidget, QWidget *parent = nullptr);
#endif
    explicit MailSourceViewTextBrowser(QWidget *parent);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
Q_SIGNALS:
    void findText();

private:
    void slotSpeakText();
    void slotSaveAs();
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    TextEditTextToSpeech::TextToSpeechContainerWidget *mTextToSpeechContainerWidget = nullptr;
#endif
};
}
