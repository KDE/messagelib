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

#include "searchfullhashgui.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

SearchFullHashGui::SearchFullHashGui(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *checkUrlLayout = new QHBoxLayout;
    layout->addLayout(checkUrlLayout);
    QLabel *lab = new QLabel(QStringLiteral("Url to Check:"), this);
    checkUrlLayout->addWidget(lab);
    mCheckUrlLineEdit = new QLineEdit(this);
    checkUrlLayout->addWidget(mCheckUrlLineEdit);
    QPushButton *button = new QPushButton(QStringLiteral("Check"), this);
    checkUrlLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &SearchFullHashGui::slotCheckUrl);
    connect(mCheckUrlLineEdit, &QLineEdit::returnPressed, this, &SearchFullHashGui::slotCheckUrl);

    mResult = new QTextEdit(this);
    mResult->setReadOnly(true);
    mResult->setAcceptRichText(false);
    layout->addWidget(mResult);

    mJson = new QTextEdit(this);
    mJson->setReadOnly(true);
    mJson->setAcceptRichText(false);
    layout->addWidget(mJson);

}

SearchFullHashGui::~SearchFullHashGui()
{

}

void SearchFullHashGui::slotCheckUrl()
{
    const QString urlStr = mCheckUrlLineEdit->text().trimmed();
    if (urlStr.isEmpty()) {
        return;
    }
    mResult->clear();
/*
    WebEngineViewer::CheckPhishingUrlJob *job = new WebEngineViewer::CheckPhishingUrlJob(this);
    job->setUseCompactJson(false);
    connect(job, &WebEngineViewer::CheckPhishingUrlJob::result, this, &SearchFullHashGui::slotGetResult);
    connect(job, &WebEngineViewer::CheckPhishingUrlJob::debugJson, this, &SearchFullHashGui::slotJSonDebug);
    job->setUrl(QUrl::fromUserInput(urlStr));
    job->start();
    */
}

void SearchFullHashGui::slotJSonDebug(const QByteArray &debug)
{
    mJson->setText(QString::fromLatin1(debug));
}

void SearchFullHashGui::slotGetResult(WebEngineViewer::SearchFullHashJob::UrlStatus result)
{
    QString resultStr;
    switch (result) {
    case WebEngineViewer::SearchFullHashJob::Ok:
        resultStr = QStringLiteral("Url ok");
        break;
    case WebEngineViewer::SearchFullHashJob::MalWare:
        resultStr = QStringLiteral("Url MalWare");
        break;
    case WebEngineViewer::SearchFullHashJob::Unknown:
        resultStr = QStringLiteral("Url Unknow state");
        break;
    case WebEngineViewer::SearchFullHashJob::BrokenNetwork:
        resultStr = QStringLiteral("Broken Network");
        break;
    case WebEngineViewer::SearchFullHashJob::InvalidUrl:
        resultStr = QStringLiteral("Invalid Url");
        break;
    }
    mResult->setText(resultStr);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    SearchFullHashGui *w = new SearchFullHashGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}
