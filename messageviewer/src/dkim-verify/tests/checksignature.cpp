/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checksignature.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>

CheckSignature::CheckSignature(const QString &fileName, QObject *parent)
    : QObject(parent)
{
    mQcaInitializer = new QCA::Initializer(QCA::Practical, 64);
    auto job = new MessageViewer::DKIMCheckSignatureJob(this);
    connect(job, &MessageViewer::DKIMCheckSignatureJob::result, this, &CheckSignature::slotResult);

    auto msg = new KMime::Message;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        msg->setContent(file.readAll());
    } else {
        qWarning() << "Couldn't read" << fileName;
    }
    msg->parse();
    job->setMessage(KMime::Message::Ptr(msg));
    job->start();
}

CheckSignature::~CheckSignature()
{
    delete mQcaInitializer;
}

void CheckSignature::slotResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    qDebug() << "result : status " << checkResult.status << " error : " << checkResult.error << " warning " << checkResult.warning;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("+[file]"), QStringLiteral("File containing an email")));

    parser.process(app);

    QString filename;
    if (!parser.positionalArguments().isEmpty()) {
        filename = parser.positionalArguments().at(0);
    }
    (void)new CheckSignature(filename);

    app.exec();
    return 0;
}

#include "moc_checksignature.cpp"
