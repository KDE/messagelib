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

    QHBoxLayout *checkHashLayout = new QHBoxLayout;
    layout->addLayout(checkHashLayout);
    QLabel *lab = new QLabel(QStringLiteral("Hash from Url to Check:"), this);
    checkHashLayout->addWidget(lab);
    mCheckHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mCheckHashLineEdit);


    QHBoxLayout *databaseHashLayout = new QHBoxLayout;
    layout->addLayout(databaseHashLayout);
    lab = new QLabel(QStringLiteral("Database hash:"), this);
    checkHashLayout->addWidget(lab);
    mDataBaseHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mDataBaseHashLineEdit);


    QPushButton *button = new QPushButton(QStringLiteral("Check"), this);
    checkHashLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &SearchFullHashGui::slotCheckUrl);
    connect(mCheckHashLineEdit, &QLineEdit::returnPressed, this, &SearchFullHashGui::slotCheckUrl);

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
    const QString hashStr = mCheckHashLineEdit->text().trimmed();
    if (hashStr.isEmpty()) {
        return;
    }
    const QString databaseHashStr = mDataBaseHashLineEdit->text().trimmed();
    if (databaseHashStr.isEmpty()) {
        return;
    }

    mResult->clear();
    WebEngineViewer::SearchFullHashJob *job = new WebEngineViewer::SearchFullHashJob(this);
    job->setUseCompactJson(false);
    connect(job, &WebEngineViewer::SearchFullHashJob::result, this, &SearchFullHashGui::slotGetResult);
    connect(job, &WebEngineViewer::SearchFullHashJob::debugJson, this, &SearchFullHashGui::slotJSonDebug);
    job->setDatabaseState(QStringList() << databaseHashStr);
    job->setSearchHash(hashStr.toLatin1());

    job->start();
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
