/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testselectedtextwebengine.h"
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <WebEngineViewer/WebEnginePage>
#include <webengineview.h>

TestSelectedTextWebEngine::TestSelectedTextWebEngine(QWidget *parent)
    : QWidget(parent)
{
    auto hboxLayout = new QVBoxLayout(this);
    pageView = new WebEngineViewer::WebEngineView(this);
    hboxLayout->addWidget(pageView);
    auto showSelectedText = new QPushButton(QStringLiteral("Show Selected Text"), this);
    connect(showSelectedText, &QPushButton::clicked, this, &TestSelectedTextWebEngine::slotSlowSelectedText);
    hboxLayout->addWidget(showSelectedText);

    mEnginePage = new WebEngineViewer::WebEnginePage(this);
    // pageView->setPage(mEnginePage);
    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestSelectedTextWebEngine::~TestSelectedTextWebEngine()
{
}

void TestSelectedTextWebEngine::slotSlowSelectedText()
{
    QMessageBox::information(this, QStringLiteral("selected text"), pageView->selectedText());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    auto testWebEngine = new TestSelectedTextWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
