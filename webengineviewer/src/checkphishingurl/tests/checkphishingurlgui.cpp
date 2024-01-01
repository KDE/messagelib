/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurlgui.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

extern WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson;

CheckPhishingUrlGui::CheckPhishingUrlGui(QWidget *parent)
    : QWidget(parent)
{
    webengineview_useCompactJson = false;
    auto layout = new QVBoxLayout(this);

    auto checkUrlLayout = new QHBoxLayout;
    layout->addLayout(checkUrlLayout);
    auto lab = new QLabel(QStringLiteral("Url to Check:"), this);
    checkUrlLayout->addWidget(lab);
    mCheckUrlLineEdit = new QLineEdit(this);
    checkUrlLayout->addWidget(mCheckUrlLineEdit);
    auto button = new QPushButton(QStringLiteral("Check"), this);
    checkUrlLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &CheckPhishingUrlGui::slotCheckUrl);
    connect(mCheckUrlLineEdit, &QLineEdit::returnPressed, this, &CheckPhishingUrlGui::slotCheckUrl);

    mResult = new QPlainTextEdit(this);
    mResult->setReadOnly(true);
    layout->addWidget(mResult);

    mJson = new QPlainTextEdit(this);
    mJson->setReadOnly(true);
    layout->addWidget(mJson);
}

CheckPhishingUrlGui::~CheckPhishingUrlGui() = default;

void CheckPhishingUrlGui::slotCheckUrl()
{
    const QString urlStr = mCheckUrlLineEdit->text().trimmed();
    if (urlStr.isEmpty()) {
        return;
    }
    mResult->clear();

    auto job = new WebEngineViewer::CheckPhishingUrlJob(this);
    connect(job, &WebEngineViewer::CheckPhishingUrlJob::result, this, &CheckPhishingUrlGui::slotGetResult);
    connect(job, &WebEngineViewer::CheckPhishingUrlJob::debugJson, this, &CheckPhishingUrlGui::slotJSonDebug);
    job->setUrl(QUrl::fromUserInput(urlStr));
    job->start();
}

void CheckPhishingUrlGui::slotJSonDebug(const QByteArray &debug)
{
    mJson->setPlainText(QString::fromLatin1(debug));
}

void CheckPhishingUrlGui::slotGetResult(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus result, const QUrl &url, uint verifyCacheAfterThisTime)
{
    QString resultStr;
    switch (result) {
    case WebEngineViewer::CheckPhishingUrlUtil::Ok:
        resultStr = QStringLiteral("Url ok");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::MalWare:
        resultStr = QStringLiteral("Url MalWare");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::Unknown:
        resultStr = QStringLiteral("Url Unknown state");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork:
        resultStr = QStringLiteral("Broken Network");
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl:
        resultStr = QStringLiteral("Invalid Url");
        break;
    }
    const QString str = QStringLiteral("\nurl: %1, verifyCacheAfterThisTime: %2").arg(url.toDisplayString()).arg(verifyCacheAfterThisTime);
    mResult->setPlainText(resultStr + str);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    auto w = new CheckPhishingUrlGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_checkphishingurlgui.cpp"
