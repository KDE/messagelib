/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailsourcewebengineviewer.h"

using namespace MessageViewer;

#include "mailsourceviewtextbrowserwidget.h"
#include "messageviewer/messageviewerutil.h"
// #include <TextAddonsWidgets/SlideContainer>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

#include <KLocalizedString>
#include <PimCommon/PimUtil>
#include <QTabWidget>

#include <KConfigGroup>
#include <KWindowConfig>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWindow>

#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QWebEnginePage>

using namespace MessageViewer;
namespace
{
static const char myMailSourceWebEngineViewerConfigGroupName[] = "MailSourceWebEngineViewer";
}
MailSourceWebEngineViewer::MailSourceWebEngineViewer(QWidget *parent)
    : QDialog(parent)
    , mRawBrowser(new MailSourceViewTextBrowserWidget(QStringLiteral("Email"), this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    auto mainLayout = new QVBoxLayout(this);
#ifndef NDEBUG
    mShowHtmlSource = true;
#endif
    mShowHtmlSource = mShowHtmlSource || !qEnvironmentVariableIsEmpty("KDEPIM_DEBUGGING");
    if (mShowHtmlSource) {
        mTabWidget = new QTabWidget(this);
        mainLayout->addWidget(mTabWidget);

        mTabWidget->addTab(mRawBrowser, i18nc("Unchanged mail message", "Raw Source"));
        mTabWidget->setTabToolTip(0, i18n("Raw, unmodified mail as it is stored on the filesystem or on the server"));

        mHtmlBrowser = new MailSourceViewTextBrowserWidget(QStringLiteral("HTML"), this);
        mTabWidget->addTab(mHtmlBrowser, i18nc("Mail message as shown, in HTML format", "HTML Source"));
        mTabWidget->setTabToolTip(1, i18n("HTML code for displaying the message to the user"));

        mTabWidget->setCurrentIndex(0);
    } else {
        mainLayout->addWidget(mRawBrowser);
    }

    // combining the shortcuts in one qkeysequence() did not work...
    auto shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_Escape);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);
    shortcut = new QShortcut(this);
    shortcut->setKey(Qt::Key_W | Qt::CTRL);
    connect(shortcut, &QShortcut::activated, this, &MailSourceWebEngineViewer::close);

    mRawBrowser->textBrowser()->setFocus();
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailSourceWebEngineViewer::reject);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &MailSourceWebEngineViewer::close);

    mainLayout->addWidget(buttonBox);
    readConfig();
}

MailSourceWebEngineViewer::~MailSourceWebEngineViewer()
{
    writeConfig();
}

void MailSourceWebEngineViewer::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(600, 400));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myMailSourceWebEngineViewerConfigGroupName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void MailSourceWebEngineViewer::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myMailSourceWebEngineViewerConfigGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
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

#include "moc_mailsourcewebengineviewer.cpp"
