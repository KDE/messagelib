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

#include "scamdetectionwebengine_gui.h"
#include "scamdetection/scamdetectionwarningwidget.h"
#include "scamdetection/scamdetectionwebengine.h"

#include <KLocalizedString>

#include <QUrl>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <KAboutData>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QWebEngineView>

ScamDetectionWebEngineTestWidget::ScamDetectionWebEngineTestWidget(const QString &filename,
                                                                   QWidget *parent)
    : QWidget(parent)
{
    mScamDetection = new MessageViewer::ScamDetectionWebEngine(this);

    QVBoxLayout *lay = new QVBoxLayout(this);
    mScamWarningWidget = new MessageViewer::ScamDetectionWarningWidget();
    mScamWarningWidget->setUseInTestApps(true);
    lay->addWidget(mScamWarningWidget);

    mWebEngineView = new QWebEngineView;
    connect(mWebEngineView, &QWebEngineView::loadFinished, this,
            &ScamDetectionWebEngineTestWidget::slotLoadFinished);
    lay->addWidget(mWebEngineView);

    connect(mScamDetection, &MessageViewer::ScamDetectionWebEngine::messageMayBeAScam,
            mScamWarningWidget, &MessageViewer::ScamDetectionWarningWidget::slotShowWarning);
    connect(mScamWarningWidget, &MessageViewer::ScamDetectionWarningWidget::showDetails,
            mScamDetection, &MessageViewer::ScamDetectionWebEngine::showDetails);

    mWebEngineView->load(QUrl::fromLocalFile(filename));

    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *openFile = new QPushButton(i18n("Open html..."));
    connect(openFile, &QPushButton::clicked, this, &ScamDetectionWebEngineTestWidget::slotOpenHtml);
    hbox->addWidget(openFile);
    lay->addLayout(hbox);
}

ScamDetectionWebEngineTestWidget::~ScamDetectionWebEngineTestWidget()
{
}

void ScamDetectionWebEngineTestWidget::slotLoadFinished()
{
    mScamDetection->scanPage(mWebEngineView->page());
}

void ScamDetectionWebEngineTestWidget::slotOpenHtml()
{
    const QString fileName = QFileDialog::getOpenFileName(nullptr, QString(),
                                                          QString(), QStringLiteral("*.html"));
    if (!fileName.isEmpty()) {
        mScamWarningWidget->setVisible(false);
        mWebEngineView->load(QUrl::fromLocalFile(fileName));
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);

    KAboutData aboutData(QStringLiteral("scamdetection_gui"), i18n(
                             "ScamDetectionTest_Gui"), QStringLiteral("1.0"));
    aboutData.setShortDescription(i18n("Test for scamdetection widget"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("+[url]"),
        i18n("URL of an html file to be opened")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QString fileName;
    if (parser.positionalArguments().count()) {
        fileName = parser.positionalArguments().at(0);
    } else {
        fileName
            = QFileDialog::getOpenFileName(nullptr, QString(), QString(),
                                           QStringLiteral("HTML File (*.html)"));
    }
    if (fileName.isEmpty()) {
        return 0;
    }

    ScamDetectionWebEngineTestWidget *w = new ScamDetectionWebEngineTestWidget(fileName);

    w->show();
    app.exec();
    delete w;
    return 0;
}
