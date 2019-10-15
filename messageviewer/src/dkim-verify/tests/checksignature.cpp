/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "checksignature.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

CheckSignature::CheckSignature(const QString &fileName, QObject *parent)
    : QObject(parent)
{
    mQcaInitializer = new QCA::Initializer(QCA::Practical, 64);
    MessageViewer::DKIMCheckSignatureJob *job = new MessageViewer::DKIMCheckSignatureJob(this);
    connect(job, &MessageViewer::DKIMCheckSignatureJob::result, this, &CheckSignature::slotResult);

    KMime::Message *msg = new KMime::Message;
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
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("+[file]"),
                                        QStringLiteral("File containing an email")));

    parser.process(app);

    QString filename;
    if (!parser.positionalArguments().isEmpty()) {
        filename = parser.positionalArguments().at(0);
    }
    CheckSignature *w = new CheckSignature(filename);

    app.exec();
    return 0;
}
