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

#include "createphishingurldatabasegui.h"
#include "../createphishingurldatabasejob.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QInputDialog>

CreatePhisingUrlDataBaseGui::CreatePhisingUrlDataBaseGui(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    mResult = new QTextEdit(this);
    mResult->setReadOnly(true);
    mResult->setAcceptRichText(false);
    layout->addWidget(mResult);

    mJson = new QTextEdit(this);
    mJson->setReadOnly(true);
    mJson->setAcceptRichText(false);
    layout->addWidget(mJson);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    QPushButton *button = new QPushButton(QStringLiteral("DownLoad full database"), this);
    connect(button, &QPushButton::clicked, this, &CreatePhisingUrlDataBaseGui::slotDownloadFullDatabase);
    buttonLayout->addWidget(button);

    QPushButton *button2 = new QPushButton(QStringLiteral("DownLoad partial database"), this);
    connect(button2, &QPushButton::clicked, this, &CreatePhisingUrlDataBaseGui::slotDownloadPartialDatabase);
    buttonLayout->addWidget(button2);
}

CreatePhisingUrlDataBaseGui::~CreatePhisingUrlDataBaseGui()
{

}

void CreatePhisingUrlDataBaseGui::clear()
{
    mJson->clear();
    mResult->clear();
}

void CreatePhisingUrlDataBaseGui::slotDownloadPartialDatabase()
{
    const QString newValue = QInputDialog::getText(this, QStringLiteral("Define database newClientState"), QStringLiteral("newClientState:"));
    if (!newValue.isEmpty()) {
        clear();
        WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
        job->setUseCompactJson(false);
        job->setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
        job->setDataBaseState(newValue);
        connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJsonResult, this, &CreatePhisingUrlDataBaseGui::slotResult);
        connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJson, this, &CreatePhisingUrlDataBaseGui::slotDebugJSon);
        job->start();
    }
}

void CreatePhisingUrlDataBaseGui::slotDownloadFullDatabase()
{
    clear();
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setUseCompactJson(false);
    connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJsonResult, this, &CreatePhisingUrlDataBaseGui::slotResult);
    connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJson, this, &CreatePhisingUrlDataBaseGui::slotDebugJSon);
    job->start();
}

void CreatePhisingUrlDataBaseGui::slotDebugJSon(const QByteArray &data)
{
    mJson->setText(QString::fromLatin1(data));
}

void CreatePhisingUrlDataBaseGui::slotResult(const QByteArray &data)
{
    mResult->setText(QString::fromLatin1(data));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    CreatePhisingUrlDataBaseGui *w = new CreatePhisingUrlDataBaseGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}
