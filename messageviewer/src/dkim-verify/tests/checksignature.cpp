/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checksignature.h"
using namespace Qt::Literals::StringLiterals;

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>

CheckSignature::CheckSignature(const QString &fileName, QObject *parent)
    : QObject(parent)
{
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
    job->setMessage(std::shared_ptr<KMime::Message>(msg));
    job->start();
}

CheckSignature::~CheckSignature() = default;

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
    parser.addOption(QCommandLineOption(QStringList() << u"+[file]"_s, u"File containing an email"_s));

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
