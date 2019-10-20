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

#include "dmarclookupkey.h"
#include "dkim-verify/dmarcrecordjob.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

DMarcLookUpKey::DMarcLookUpKey(QObject *parent)
    : QObject(parent)
{

}

DMarcLookUpKey::~DMarcLookUpKey()
{

}

void DMarcLookUpKey::lookUpDomain(const QString &domain)
{
    MessageViewer::DMARCRecordJob *job = new MessageViewer::DMARCRecordJob(this);
    job->setDomainName(domain);
    connect(job, &MessageViewer::DMARCRecordJob::success, this, [](const QList<QByteArray> &lst, const QString &domainName) {
       qDebug() << "domainName: " << domainName << " lst " << lst;
    });
    connect(job, &MessageViewer::DMARCRecordJob::error, this, [](const QString &err, const QString &domainName) {
       qDebug() << "error: " << err << " domain " << domainName;
    });
    job->start();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addPositionalArgument(
        QStringLiteral("server address"), QStringLiteral("add specific server address"));
    parser.process(app);

    if (!parser.positionalArguments().isEmpty()) {
        for (const QString &str : parser.positionalArguments()) {
            DMarcLookUpKey *d = new DMarcLookUpKey();
            d->lookUpDomain(str);
        }
    }

    app.exec();
    return 0;
}
