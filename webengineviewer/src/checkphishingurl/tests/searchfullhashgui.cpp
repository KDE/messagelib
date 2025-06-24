/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "searchfullhashgui.h"
using namespace Qt::Literals::StringLiterals;

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
    auto lab = new QLabel(u"Hash from Url to Check:"_s, this);
    checkHashLayout->addWidget(lab);
    mCheckHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mCheckHashLineEdit);

    auto databaseHashLayout = new QHBoxLayout;
    layout->addLayout(databaseHashLayout);
    lab = new QLabel(u"Database hash:"_s, this);
    checkHashLayout->addWidget(lab);
    mDataBaseHashLineEdit = new QLineEdit(this);
    checkHashLayout->addWidget(mDataBaseHashLineEdit);

    auto button = new QPushButton(u"Check"_s, this);
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
