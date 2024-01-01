/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "searchfullhashgui.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

extern WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_SearchFullHashJob;

SearchFullHashGui::SearchFullHashGui(QWidget *parent)
    : QWidget(parent)
{
    webengineview_useCompactJson_SearchFullHashJob = false;
    auto layout = new QVBoxLayout(this);

    auto checkHashLayout = new QHBoxLayout;
    layout->addLayout(checkHashLayout);
    auto lab = new QLabel(QStringLiteral("Hash from Url to Check:"), this);
    checkHashLayout->addWidget(lab);
    mCheckHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mCheckHashLineEdit);

    auto databaseHashLayout = new QHBoxLayout;
    layout->addLayout(databaseHashLayout);
    lab = new QLabel(QStringLiteral("Database hash:"), this);
    checkHashLayout->addWidget(lab);
    mDataBaseHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mDataBaseHashLineEdit);

    auto button = new QPushButton(QStringLiteral("Check"), this);
    checkHashLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &SearchFullHashGui::slotCheckUrl);
    connect(mCheckHashLineEdit, &QLineEdit::returnPressed, this, &SearchFullHashGui::slotCheckUrl);

    mResult = new QPlainTextEdit(this);
    mResult->setReadOnly(true);
    layout->addWidget(mResult);

    mJson = new QPlainTextEdit(this);
    mJson->setReadOnly(true);
    layout->addWidget(mJson);
}

SearchFullHashGui::~SearchFullHashGui() = default;

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
    auto job = new WebEngineViewer::SearchFullHashJob(this);
    connect(job, &WebEngineViewer::SearchFullHashJob::result, this, &SearchFullHashGui::slotGetResult);
    connect(job, &WebEngineViewer::SearchFullHashJob::debugJson, this, &SearchFullHashGui::slotJSonDebug);
    job->setDatabaseState(QStringList() << databaseHashStr);
    QByteArray ba = hashStr.toLatin1();
    QByteArray baShort = ba;
    baShort.truncate(4);
    QHash<QByteArray, QByteArray> lst;
    lst.insert(ba, baShort);
    job->setSearchHashs(lst);

    job->start();
}

void SearchFullHashGui::slotJSonDebug(const QByteArray &debug)
{
    mJson->setPlainText(QString::fromLatin1(debug));
}

void SearchFullHashGui::slotGetResult(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus result)
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
    mResult->setPlainText(resultStr);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    auto w = new SearchFullHashGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_searchfullhashgui.cpp"
