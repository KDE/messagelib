/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "webengineviewwithsafebrowsingsupport.h"
#include "../localdatabasemanager.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <webenginepage.h>
#include <webengineview.h>

WebEngineViewWithSafeBrowsingSupport::WebEngineViewWithSafeBrowsingSupport(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    WebEngineViewer::LocalDataBaseManager::self()->initialize();
    connect(WebEngineViewer::LocalDataBaseManager::self(), &WebEngineViewer::LocalDataBaseManager::checkUrlFinished, this, &WebEngineViewWithSafeBrowsingSupport::slotCheckedUrlFinished);
    //Make sure to initialize database
    WebEngineViewer::LocalDataBaseManager::self();
    WebEngineViewer::WebEngineView *pageView = new WebEngineViewer::WebEngineView(this);
    layout->addWidget(pageView);
    WebEngineViewer::WebEnginePage *mEnginePage = new WebEngineViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);
    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
    connect(mEnginePage, &WebEngineViewer::WebEnginePage::urlClicked, this, &WebEngineViewWithSafeBrowsingSupport::slotUrlClicked);

    mDebug = new QPlainTextEdit(this);
    layout->addWidget(mDebug);
}

WebEngineViewWithSafeBrowsingSupport::~WebEngineViewWithSafeBrowsingSupport()
{

}

void WebEngineViewWithSafeBrowsingSupport::slotUrlClicked(const QUrl &url)
{
    qDebug() << " url clicked " << url;
    WebEngineViewer::LocalDataBaseManager::self()->checkUrl(url);
}

void WebEngineViewWithSafeBrowsingSupport::slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::LocalDataBaseManager::UrlStatus status)
{
    qDebug() << " checked url: " << url << " result : " << status;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    WebEngineViewWithSafeBrowsingSupport *w = new WebEngineViewWithSafeBrowsingSupport;

    w->show();
    app.exec();
    delete w;
    return 0;
}
