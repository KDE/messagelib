/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "scamdetectionwebengine_gui.h"
using namespace Qt::Literals::StringLiterals;

#include "scamdetection/scamdetectionwarningwidget.h"
#include "scamdetection/scamdetectionwebengine.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineView>

ScamDetectionWebEngineTestWidget::ScamDetectionWebEngineTestWidget(const QString &filename, QWidget *parent)
    : QWidget(parent)
{
    mScamDetection = new MessageViewer::ScamDetectionWebEngine(this);

    auto lay = new QVBoxLayout(this);
    mScamWarningWidget = new MessageViewer::ScamDetectionWarningWidget();
    mScamWarningWidget->setUseInTestApps(true);
    lay->addWidget(mScamWarningWidget);

    mWebEngineView = new QWebEngineView;
    connect(mWebEngineView, &QWebEngineView::loadFinished, this, &ScamDetectionWebEngineTestWidget::slotLoadFinished);
    lay->addWidget(mWebEngineView);

    connect(mScamDetection,
            &MessageViewer::ScamDetectionWebEngine::messageMayBeAScam,
            mScamWarningWidget,
            &MessageViewer::ScamDetectionWarningWidget::slotShowWarning);
    connect(mScamWarningWidget, &MessageViewer::ScamDetectionWarningWidget::showDetails, mScamDetection, &MessageViewer::ScamDetectionWebEngine::showDetails);

    mWebEngineView->load(QUrl::fromLocalFile(filename));

    auto hbox = new QHBoxLayout;
    auto openFile = new QPushButton(u"Open html..."_s);
    connect(openFile, &QPushButton::clicked, this, &ScamDetectionWebEngineTestWidget::slotOpenHtml);
    hbox->addWidget(openFile);
    lay->addLayout(hbox);
}

ScamDetectionWebEngineTestWidget::~ScamDetectionWebEngineTestWidget() = default;

void ScamDetectionWebEngineTestWidget::slotLoadFinished()
{
    mScamDetection->scanPage(mWebEngineView->page());
}

void ScamDetectionWebEngineTestWidget::slotOpenHtml()
{
    const QString fileName = QFileDialog::getOpenFileName(nullptr, QString(), QString(), u"*.html"_s);
    if (!fileName.isEmpty()) {
        mScamWarningWidget->setVisible(false);
        mWebEngineView->load(QUrl::fromLocalFile(fileName));
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << u"+[url]"_s, u"URL of an html file to be opened"_s));
    parser.process(app);

    QString fileName;
    if (!parser.positionalArguments().isEmpty()) {
        fileName = parser.positionalArguments().at(0);
    } else {
        fileName = QFileDialog::getOpenFileName(nullptr, QString(), QString(), u"HTML File (*.html)"_s);
    }
    if (fileName.isEmpty()) {
        return 0;
    }

    auto w = new ScamDetectionWebEngineTestWidget(fileName);

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_scamdetectionwebengine_gui.cpp"
