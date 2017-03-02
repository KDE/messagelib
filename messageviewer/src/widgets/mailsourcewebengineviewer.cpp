/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mailsourcewebengineviewer.h"

using namespace MessageViewer;

#include "mailsourceviewtextbrowserwidget.h"
#include "messageviewer/messageviewerutil.h"
#include "findbar/findbarsourceview.h"
#include "kpimtextedit/slidecontainer.h"

#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include "PimCommon/PimUtil"
#include <kiconloader.h>
#include <KLocalizedString>
#include <kwindowsystem.h>
#include <QTabWidget>
#include <QApplication>

#include <QRegExp>
#include <QShortcut>
#include <QVBoxLayout>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QWebEnginePage>

using namespace MessageViewer;

MailSourceWebEngineViewer::MailSourceWebEngineViewer(QWidget *parent)
    : QDialog(parent),
      mFindBar(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mRawBrowser = new MailSourceViewTextBrowserWidget(this);

#ifndef NDEBUG
    mTabWidget = new QTabWidget(this);
    mainLayout->addWidget(mTabWidget);

    mTabWidget->addTab(mRawBrowser, i18nc("Unchanged mail message", "Raw Source"));
    mTabWidget->setTabToolTip(0, i18n("Raw, unmodified mail as it is stored on the filesystem or on the server"));

    mHtmlBrowser = new MailSourceViewTextBrowserWidget(this);
    mTabWidget->addTab(mHtmlBrowser, i18nc("Mail message as shown, in HTML format", "HTML Source"));
    mTabWidget->setTabToolTip(1, i18n("HTML code for displaying the message to the user"));
    KSyntaxHighlighting::Definition def;
    def = mRepo.definitionForName(QStringLiteral("HTML"));

    KSyntaxHighlighting::SyntaxHighlighter *hl = new KSyntaxHighlighting::SyntaxHighlighter(mHtmlBrowser->textBrowser()->document());
    hl->setTheme((palette().color(QPalette::Base).lightness() < 128)
                 ? mRepo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                 : mRepo.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
    hl->setDefinition(def);

    mTabWidget->setCurrentIndex(0);
#else
    mainLayout->addWidget(mRawBrowser);
#endif

    // combining the shortcuts in one qkeysequence() did not work...
    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_Escape);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);
    shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_W + Qt::CTRL);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);

    KWindowSystem::setIcons(winId(),
                            qApp->windowIcon().pixmap(IconSize(KIconLoader::Desktop),
                                    IconSize(KIconLoader::Desktop)),
                            qApp->windowIcon().pixmap(IconSize(KIconLoader::Small),
                                    IconSize(KIconLoader::Small)));
    mRawBrowser->textBrowser()->setFocus();
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailSourceWebEngineViewer::reject);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &MailSourceWebEngineViewer::close);

    mainLayout->addWidget(buttonBox);
}

MailSourceWebEngineViewer::~MailSourceWebEngineViewer()
{
}

void MailSourceWebEngineViewer::setRawSource(const QString &source)
{
    mRawBrowser->setText(source);
}

void MailSourceWebEngineViewer::setDisplayedSource(QWebEnginePage *page)
{
#ifndef NDEBUG
    if (page) {
        MailSourceViewTextBrowserWidget *browser = mHtmlBrowser;
        page->toHtml([browser](const QString & result) {
            browser->setPlainText(result);
        });
    }
#else
    Q_UNUSED(page);
#endif
}

void MailSourceWebEngineViewer::setFixedFont()
{
    mRawBrowser->setFixedFont();
#ifndef NDEBUG
    mHtmlBrowser->setFixedFont();
#endif
}

