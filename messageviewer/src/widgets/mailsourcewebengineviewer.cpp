/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailsourcewebengineviewer.h"

using namespace MessageViewer;

#include "mailsourceviewtextbrowserwidget.h"
#include "messageviewer/messageviewerutil.h"
#include "findbar/findbarsourceview.h"
#include <KPIMTextEdit/SlideContainer>

#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <PimCommon/PimUtil>
#include <KLocalizedString>
#include <QTabWidget>

#include <KConfigGroup>
#include <QShortcut>
#include <QVBoxLayout>

#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QWebEnginePage>

using namespace MessageViewer;

MailSourceWebEngineViewer::MailSourceWebEngineViewer(QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mRawBrowser = new MailSourceViewTextBrowserWidget(QStringLiteral("Email"), this);
#ifndef NDEBUG
    mShowHtmlSource = true;
#endif
    mShowHtmlSource = mShowHtmlSource || !qEnvironmentVariableIsEmpty("KDEPIM_DEBUGGING");
    if (mShowHtmlSource) {
        mTabWidget = new QTabWidget(this);
        mainLayout->addWidget(mTabWidget);

        mTabWidget->addTab(mRawBrowser, i18nc("Unchanged mail message", "Raw Source"));
        mTabWidget->setTabToolTip(0,
                                  i18n(
                                      "Raw, unmodified mail as it is stored on the filesystem or on the server"));

        mHtmlBrowser = new MailSourceViewTextBrowserWidget(QStringLiteral("HTML"), this);
        mTabWidget->addTab(mHtmlBrowser, i18nc("Mail message as shown, in HTML format", "HTML Source"));
        mTabWidget->setTabToolTip(1, i18n("HTML code for displaying the message to the user"));

        mTabWidget->setCurrentIndex(0);
    } else {
        mainLayout->addWidget(mRawBrowser);
    }

    // combining the shortcuts in one qkeysequence() did not work...
    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_Escape);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);
    shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_W + Qt::CTRL);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);

    mRawBrowser->textBrowser()->setFocus();
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailSourceWebEngineViewer::reject);
    connect(buttonBox->button(
                QDialogButtonBox::Close), &QPushButton::clicked, this,
            &MailSourceWebEngineViewer::close);

    mainLayout->addWidget(buttonBox);
    readConfig();
}

MailSourceWebEngineViewer::~MailSourceWebEngineViewer()
{
    writeConfig();
}

void MailSourceWebEngineViewer::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "MailSourceWebEngineViewer");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void MailSourceWebEngineViewer::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "MailSourceWebEngineViewer");
    group.writeEntry("Size", size());
    group.sync();
}

void MailSourceWebEngineViewer::setRawSource(const QString &source)
{
    mRawBrowser->setText(source);
}

void MailSourceWebEngineViewer::setDisplayedSource(QWebEnginePage *page)
{
    if (mShowHtmlSource) {
        if (page) {
            MailSourceViewTextBrowserWidget *browser = mHtmlBrowser;
            page->toHtml([browser](const QString &result) {
                browser->setPlainText(result);
            });
        }
    }
}

void MailSourceWebEngineViewer::setFixedFont()
{
    mRawBrowser->setFixedFont();
    if (mShowHtmlSource) {
        mHtmlBrowser->setFixedFont();
    }
}
