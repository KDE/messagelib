/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testselectedtextwebengine.h"
using namespace Qt::Literals::StringLiterals;

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
    auto showSelectedText = new QPushButton(u"Show Selected Text"_s, this);
    connect(showSelectedText, &QPushButton::clicked, this, &TestSelectedTextWebEngine::slotSlowSelectedText);
    hboxLayout->addWidget(showSelectedText);

    mEnginePage = new WebEngineViewer::WebEnginePage(this);
    // pageView->setPage(mEnginePage);
    pageView->load(QUrl(u"http://www.kde.org"_s));
}

TestSelectedTextWebEngine::~TestSelectedTextWebEngine() = default;

void TestSelectedTextWebEngine::slotSlowSelectedText()
{
    QMessageBox::information(this, u"selected text"_s, pageView->selectedText());
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
