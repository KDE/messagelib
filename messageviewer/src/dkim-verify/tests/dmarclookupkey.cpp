/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarclookupkey.h"
#include "dkim-verify/dmarcrecordjob.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>

DMarcLookUpKey::DMarcLookUpKey(QObject *parent)
    : QObject(parent)
{
}

DMarcLookUpKey::~DMarcLookUpKey() = default;

void DMarcLookUpKey::lookUpDomain(const QString &domain)
{
    auto job = new MessageViewer::DMARCRecordJob(this);
    job->setDomainName(domain);
    connect(job, &MessageViewer::DMARCRecordJob::success, this, [](const QList<QByteArray> &lst, const QString &domainName) {
        qDebug() << "domainName: " << domainName << " lst " << lst;
    });
    connect(job, &MessageViewer::DMARCRecordJob::error, this, [](const QString &err, const QString &domainName) {
        qDebug() << "error: " << err << " domain " << domainName;
    });
    if (!job->start()) {
        qWarning() << " impossible to start job";
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("server address"), QStringLiteral("add specific server address"));
    parser.process(app);

    if (!parser.positionalArguments().isEmpty()) {
        const QStringList lst = parser.positionalArguments();
        for (const QString &str : lst) {
            auto d = new DMarcLookUpKey();
            d->lookUpDomain(str);
        }
    }

    app.exec();
    return 0;
}

#include "moc_dmarclookupkey.cpp"
