/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurlgui.h"
using namespace Qt::Literals::StringLiterals;

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
    auto lab = new QLabel(u"Url to Check:"_s, this);
    checkUrlLayout->addWidget(lab);
    mCheckUrlLineEdit = new QLineEdit(this);
    checkUrlLayout->addWidget(mCheckUrlLineEdit);
    auto button = new QPushButton(u"Check"_s, this);
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
        resultStr = u"Url ok"_s;
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::MalWare:
        resultStr = u"Url MalWare"_s;
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::Unknown:
        resultStr = u"Url Unknown state"_s;
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork:
        resultStr = u"Broken Network"_s;
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl:
        resultStr = u"Invalid Url"_s;
        break;
    }
    const QString str = u"\nurl: %1, verifyCacheAfterThisTime: %2"_s.arg(url.toDisplayString()).arg(verifyCacheAfterThisTime);
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
