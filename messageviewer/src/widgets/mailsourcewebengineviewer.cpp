/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mailsourcewebengineviewer.h"

using namespace MessageViewer;

#include "mailsourceviewtextbrowserwidget.h"
#include "messageviewer/messageviewerutil.h"
#include "findbar/findbarsourceview.h"
#include <kpimtextedit/htmlhighlighter.h>
#include "kpimtextedit/slidecontainer.h"
#include "PimCommon/PimUtil"
#include <kiconloader.h>
#include <KLocalizedString>
#include <KStandardAction>
#include <kwindowsystem.h>
#include <QTabWidget>
#include <KMessageBox>
#include <QAction>
#include <QIcon>
#include <QApplication>

#include <QRegExp>
#include <QShortcut>
#include <QVBoxLayout>

#include <QMenu>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace MessageViewer;

MailSourceWebEngineViewer::MailSourceWebEngineViewer(QWidget *parent)
    : QDialog(parent),
      mFindBar(Q_NULLPTR)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailSourceWebEngineViewer::reject);

    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->setMargin(0);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &MailSourceWebEngineViewer::close);

    mRawBrowser = new MailSourceViewTextBrowserWidget();

#ifndef NDEBUG
    mTabWidget = new QTabWidget;
    layout->addWidget(mTabWidget);

    mTabWidget->addTab(mRawBrowser, i18nc("Unchanged mail message", "Raw Source"));
    mTabWidget->setTabToolTip(0, i18n("Raw, unmodified mail as it is stored on the filesystem or on the server"));

    mHtmlBrowser = new MailSourceViewTextBrowserWidget();
    mTabWidget->addTab(mHtmlBrowser, i18nc("Mail message as shown, in HTML format", "HTML Source"));
    mTabWidget->setTabToolTip(1, i18n("HTML code for displaying the message to the user"));
    new KPIMTextEdit::HtmlHighlighter(mHtmlBrowser->textBrowser()->document());

    mTabWidget->setCurrentIndex(0);
#else
    layout->addWidget(mRawBrowser);
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
    new MailSourceHighlighter(mRawBrowser->textBrowser()->document());
    mRawBrowser->textBrowser()->setFocus();
    mainLayout->addWidget(buttonBox);
}

MailSourceWebEngineViewer::~MailSourceWebEngineViewer()
{
}

void MailSourceWebEngineViewer::setRawSource(const QString &source)
{
    mRawBrowser->setText(source);
}

void MailSourceWebEngineViewer::setDisplayedSource(const QString &source)
{
#ifndef NDEBUG
    mHtmlBrowser->setPlainText(reformat(source));
#else
    Q_UNUSED(source);
#endif
}

void MailSourceWebEngineViewer::setFixedFont()
{
    mRawBrowser->setFixedFont();
#ifndef NDEBUG
    mHtmlBrowser->setFixedFont();
#endif
}

QString MailSourceWebEngineViewer::reformat(const QString &src)
{
    const QRegExp cleanLeadingWhitespace(QStringLiteral("(?:\\n)+\\w*"));
    QStringList tmpSource;
    QString source(src);
    int pos = 0;
    QString indent;
    // Best to be really verbose about this one...
    const static QRegExp htmlTagRegExp(QStringLiteral("<"
                                       "(/)?"    //Captures the / if this is an end tag.
                                       "(\\w+)"    //Captures TagName
                                       "(?:"                //Groups tag contents
                                       "(?:\\s+"            //Groups attributes
                                       "(?:\\w+)"  //Attribute name
                                       "(?:"                //groups =value portion.
                                       "\\s*=\\s*"            // =
                                       "(?:"        //Groups attribute "value" portion.
                                       "\\\"(?:[^\\\"]*)\\\""    // attVal='double quoted'
                                       "|'(?:[^']*)'"        // attVal='single quoted'
                                       "|(?:[^'"">\\s]+)"    // attVal=urlnospaces
                                       ")"
                                       ")?"        //end optional att value portion.
                                       ")+\\s*"        //One or more attribute pairs
                                       "|\\s*"            //Some white space.
                                       ")"
                                       "(/)?>" //Captures the "/" if this is a complete tag.
                                                     ));

    //First make sure that each tag is surrounded by newlines
    while ((pos = htmlTagRegExp.indexIn(source, pos)) != -1) {
        source.insert(pos, QLatin1Char('\n'));
        pos += htmlTagRegExp.matchedLength() + 1;
        source.insert(pos, QLatin1Char('\n'));
        ++pos;
    }

    // Then split the source on newlines skiping empty parts.
    // Now a line is either a tag or pure data.
    tmpSource = source.split(QLatin1Char('\n'), QString::SkipEmptyParts);

    // Then clean any leading whitespace
    for (int i = 0; i != tmpSource.length(); ++i) {
        tmpSource[i] = tmpSource[i].remove(cleanLeadingWhitespace);
    }

    // Then indent as appropriate
    for (int i = 0; i != tmpSource.length(); ++i)  {
        if (htmlTagRegExp.indexIn(tmpSource.at(i)) != -1) {  // A tag
            if (htmlTagRegExp.cap(3) == QLatin1String("/") ||
                    htmlTagRegExp.cap(2) == QLatin1String("img") ||
                    htmlTagRegExp.cap(2) == QLatin1String("br")) {
                //Self closing tag or no closure needed
                continue;
            }
            if (htmlTagRegExp.cap(1) == QLatin1String("/")) {
                // End tag
                indent.chop(2);
                tmpSource[i].prepend(indent);
                continue;
            }
            // start tag
            tmpSource[i].prepend(indent);
            indent.append(QStringLiteral("  "));
            continue;
        }
        // Data
        tmpSource[i].prepend(indent);
    }

    // Finally reassemble and return :)
    return tmpSource.join(QStringLiteral("\n"));
}
