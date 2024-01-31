/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testselectedtextwebengine.h"
#include "webengineview.h"
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <WebEngineViewer/WebEnginePage>

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

TestSelectedTextWebEngine::~TestSelectedTextWebEngine() = default;

void TestSelectedTextWebEngine::slotSlowSelectedText()
{
    QMessageBox::information(this, QStringLiteral("selected text"), pageView->selectedText());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto testWebEngine = new TestSelectedTextWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

#include "moc_testselectedtextwebengine.cpp"
