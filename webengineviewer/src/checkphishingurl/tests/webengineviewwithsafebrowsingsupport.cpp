/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineviewwithsafebrowsingsupport.h"
#include "../localdatabasemanager.h"

#include "webenginepage.h"
#include "webengineview.h"
#include <QApplication>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

WebEngineViewWithSafeBrowsingSupport::WebEngineViewWithSafeBrowsingSupport(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    QStandardPaths::setTestModeEnabled(true);
    pageView = new WebEngineViewer::WebEngineView(this);
    connect(pageView->phishingDatabase(),
            &WebEngineViewer::LocalDataBaseManager::checkUrlFinished,
            this,
            &WebEngineViewWithSafeBrowsingSupport::slotCheckedUrlFinished);
    layout->addWidget(pageView);
    auto mEnginePage = new WebEngineViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);
    // pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
    const QString urlPage = QLatin1StringView(CHECKPHISHINGURL_TEST_DATA_DIR) + QStringLiteral("/test-url.html");
    qDebug() << " urlPage" << urlPage;
    pageView->load(QUrl::fromLocalFile(urlPage));
    connect(mEnginePage, &WebEngineViewer::WebEnginePage::urlClicked, this, &WebEngineViewWithSafeBrowsingSupport::slotUrlClicked);

    mDebug = new QPlainTextEdit(this);
    mDebug->setReadOnly(true);
    layout->addWidget(mDebug);
}

WebEngineViewWithSafeBrowsingSupport::~WebEngineViewWithSafeBrowsingSupport() = default;

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
    mDebug->setPlainText(QStringLiteral("Url: %1 , Status %2").arg(url.toDisplayString(), statusStr));
    if (status != WebEngineViewer::CheckPhishingUrlUtil::MalWare) {
        pageView->load(url);
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    auto w = new WebEngineViewWithSafeBrowsingSupport;

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_webengineviewwithsafebrowsingsupport.cpp"
