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

#include "checkphishingurlgui.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>

extern WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson;

CheckPhishingUrlGui::CheckPhishingUrlGui(QWidget *parent)
    : QWidget(parent)
{
    webengineview_useCompactJson = false;
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *checkUrlLayout = new QHBoxLayout;
    layout->addLayout(checkUrlLayout);
    QLabel *lab = new QLabel(QStringLiteral("Url to Check:"), this);
    checkUrlLayout->addWidget(lab);
    mCheckUrlLineEdit = new QLineEdit(this);
    checkUrlLayout->addWidget(mCheckUrlLineEdit);
    QPushButton *button = new QPushButton(QStringLiteral("Check"), this);
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

CheckPhishingUrlGui::~CheckPhishingUrlGui()
{
}

void CheckPhishingUrlGui::slotCheckUrl()
{
    const QString urlStr = mCheckUrlLineEdit->text().trimmed();
    if (urlStr.isEmpty()) {
        return;
    }
    mResult->clear();

    WebEngineViewer::CheckPhishingUrlJob *job = new WebEngineViewer::CheckPhishingUrlJob(this);
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
        resultStr = QStringLiteral("Url Unknow state");
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
    CheckPhishingUrlGui *w = new CheckPhishingUrlGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}
