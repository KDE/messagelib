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

#include "checkphishingurlgui.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

CheckPhishingUrlGui::CheckPhishingUrlGui(QWidget *parent)
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
    connect(button, &QPushButton::clicked, this, &CheckPhishingUrlGui::slotCheckUrl);

    mResult = new QTextEdit(this);
    mResult->setReadOnly(true);
    mResult->setAcceptRichText(false);
    layout->addWidget(mResult);
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

    MessageViewer::CheckPhishingUrlJob *job = new MessageViewer::CheckPhishingUrlJob(this);
    connect(job, &MessageViewer::CheckPhishingUrlJob::result, this, &CheckPhishingUrlGui::slotGetResult);
    job->setUrl(QUrl(urlStr));
    job->start();
}

void CheckPhishingUrlGui::slotGetResult(MessageViewer::CheckPhishingUrlJob::UrlStatus result)
{
    QString resultStr;
    switch(result) {
    case MessageViewer::CheckPhishingUrlJob::Ok:
        resultStr = QStringLiteral("Url ok");
        break;
    case MessageViewer::CheckPhishingUrlJob::MalWare:
        resultStr = QStringLiteral("Url MalWare");
        break;
    case MessageViewer::CheckPhishingUrlJob::Unknown:
        resultStr = QStringLiteral("Url Unknow state");
        break;
    }
    mResult->setText(resultStr);
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
