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
    QStandardPaths::setTestModeEnabled(true);
    pageView = new WebEngineViewer::WebEngineView(this);
    connect(pageView->phishingDatabase(), &WebEngineViewer::LocalDataBaseManager::checkUrlFinished,
            this, &WebEngineViewWithSafeBrowsingSupport::slotCheckedUrlFinished);
    layout->addWidget(pageView);
    WebEngineViewer::WebEnginePage *mEnginePage = new WebEngineViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);
    //pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
    const QString urlPage = QLatin1String(CHECKPHISHINGURL_TEST_DATA_DIR) + QStringLiteral("/test-url.html");
    qDebug() << " urlPage" << urlPage;
    pageView->load(QUrl::fromLocalFile(urlPage));
    connect(mEnginePage, &WebEngineViewer::WebEnginePage::urlClicked, this, &WebEngineViewWithSafeBrowsingSupport::slotUrlClicked);

    mDebug = new QPlainTextEdit(this);
    mDebug->setReadOnly(true);
    layout->addWidget(mDebug);
}

WebEngineViewWithSafeBrowsingSupport::~WebEngineViewWithSafeBrowsingSupport()
{

}

void WebEngineViewWithSafeBrowsingSupport::slotUrlClicked(const QUrl &url)
{
    qDebug() << " url clicked " << url;
    pageView->phishingDatabase()->checkUrl(url);
}

void WebEngineViewWithSafeBrowsingSupport::slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status)
{
    QString statusStr;
    switch (status) {
    case WebEngineViewer::CheckPhishingUrlUtil::Unknown:
        statusStr = QStringLiteral("Unknown Status");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::Ok:
        statusStr = QStringLiteral("Url Ok");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::MalWare:
        statusStr = QStringLiteral("MalWare");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl:
        statusStr = QStringLiteral("Invalid Url");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork:
        statusStr = QStringLiteral("Broken Network");
        break;
    }

    qDebug() << " checked url: " << url << " result : " << statusStr;
    mDebug->setPlainText(QStringLiteral("Url: %1 , Status %2").arg(url.toDisplayString()).arg(statusStr));
    if (status != WebEngineViewer::CheckPhishingUrlUtil::MalWare) {
        pageView->load(url);
    }
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
