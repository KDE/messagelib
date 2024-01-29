/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "createphishingurldatabasegui.h"

#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

extern WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_CreatePhishingUrlDataBaseJob;

CreatePhisingUrlDataBaseGui::CreatePhisingUrlDataBaseGui(QWidget *parent)
    : QWidget(parent)
{
    webengineview_useCompactJson_CreatePhishingUrlDataBaseJob = false;
    auto layout = new QVBoxLayout(this);

    mCompressionType = new QComboBox(this);
    mCompressionType->addItem(QStringLiteral("RAW"));
    mCompressionType->addItem(QStringLiteral("RICE"));
    layout->addWidget(mCompressionType);

    mResult = new QPlainTextEdit(this);
    mResult->setReadOnly(true);
    layout->addWidget(mResult);

    mJson = new QPlainTextEdit(this);
    mJson->setReadOnly(true);
    layout->addWidget(mJson);

    auto buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    auto button = new QPushButton(QStringLiteral("DownLoad full database"), this);
    connect(button, &QPushButton::clicked, this, &CreatePhisingUrlDataBaseGui::slotDownloadFullDatabase);
    buttonLayout->addWidget(button);

    auto button2 = new QPushButton(QStringLiteral("DownLoad partial database"), this);
    connect(button2, &QPushButton::clicked, this, &CreatePhisingUrlDataBaseGui::slotDownloadPartialDatabase);
    buttonLayout->addWidget(button2);

    auto save = new QPushButton(QStringLiteral("Save result to disk"), this);
    connect(save, &QPushButton::clicked, this, &CreatePhisingUrlDataBaseGui::slotSaveResultToDisk);
    buttonLayout->addWidget(save);
}

CreatePhisingUrlDataBaseGui::~CreatePhisingUrlDataBaseGui() = default;

void CreatePhisingUrlDataBaseGui::clear()
{
    mJson->clear();
    mResult->clear();
}

WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType CreatePhisingUrlDataBaseGui::compressionType()
{
    WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType type = WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression;
    if (mCompressionType->currentText() == QLatin1StringView("RICE")) {
        type = WebEngineViewer::CreatePhishingUrlDataBaseJob::RiceCompression;
    } else if (mCompressionType->currentText() == QLatin1StringView("RAW")) {
        type = WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression;
    }
    return type;
}

void CreatePhisingUrlDataBaseGui::slotDownloadPartialDatabase()
{
    const QString newValue = QInputDialog::getText(this, QStringLiteral("Define database newClientState"), QStringLiteral("newClientState:"));
    if (!newValue.isEmpty()) {
        clear();
        auto job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
        job->setContraintsCompressionType(compressionType());
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
    auto job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setContraintsCompressionType(compressionType());
    connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJsonResult, this, &CreatePhisingUrlDataBaseGui::slotResult);
    connect(job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::debugJson, this, &CreatePhisingUrlDataBaseGui::slotDebugJSon);
    job->start();
}

void CreatePhisingUrlDataBaseGui::slotSaveResultToDisk()
{
    if (!mResult->document()->isEmpty()) {
        const QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("save result to disk"));
        QTextStream ds;
        QFile file;
        file.setFileName(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << " impossible to open file " << filename;
            return;
        }
        ds.setDevice(&file);
        ds << mResult->toPlainText();
        file.close();
    }
}

void CreatePhisingUrlDataBaseGui::slotDebugJSon(const QByteArray &data)
{
    mJson->setPlainText(QString::fromLatin1(data));
}

void CreatePhisingUrlDataBaseGui::slotResult(const QByteArray &data)
{
    mResult->setPlainText(QString::fromLatin1(data));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    auto w = new CreatePhisingUrlDataBaseGui;

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_createphishingurldatabasegui.cpp"
