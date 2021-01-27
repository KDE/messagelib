/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkrsapublickey.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <qca_publickey.h>

CheckRSAPublicKey::CheckRSAPublicKey(QObject *parent)
    : QObject(parent)
{
    QCA::Initializer initializer;
    // qDebug() << " support : "<< QCA::isSupported("rsa");
    QCA::ConvertResult conversionResult;
    // QByteArray ba =
    // "TUlHZk1BMEdDU3FHU0liM0RRRUJBUVVBQTRHTkFEQ0JpUUtCZ1FES3NSL3ZEVS9kNitnRnZhbW42c2c4M1Q5KzVPdkI4MFFvbXR5Y09yeG1UdHhGT0FMVkxWVWpWN1J6OExOcVhKZDh1TnROVXFzdU5iK3JSc3pnNE5HS09HRG5OQjJEMWd5M3NWUFNtZkYvZmxycEN3QkEyOHJ5Q3N2MVRoNG9aaGNlVkN1cmNIdFlGbXE1OHhta1ZhQXpSSllmbnBzNmVKQU9xZFJBcGpHRUdRSURBUUFC";
    // QByteArray ba =
    // "TUlJQklqQU5CZ2txaGtpRzl3MEJBUUVGQUFPQ0FROEFNSUlCQ2dLQ0FRRUF2V3lrdHJJTDhETy8rVUd2TWJ2N2NQZC9Yb2dwYnM3cGdWdzh5OWxkTzZBQU1tZzgraWpFTmwvYzdGYjFNZktNN3VHM0xNd0FyMGRWVkt5TSttYmtvWDJrNUw3bHNST1FyMFo5Z0dTcHU3eHJuWk9hNTgrL3BJaGQyWGsvREZQcGE1K1RLYldvZGJzU1pQUk44ejBSWTV4NTlqZHpTY2xYbEV5TjltRVpkbU9pS1RzT1A2QTd2UXhmU3lhOWpnNU44MWRmTk52UDdIbldlak1Nc0t5SU1yWHB0eE9oSUJ1RVlINjdKRGU5OFFnWDE0b0h2R00yVXo1M2lmL1NXOE1GMDlyWWg5c3A0WnNhV0xJZzZUMzQzSnpsYnRyc0dSR0NESjlKUHB4UldaaW10eitVcC9CbEt6VDZzQ0NyQmloYi9CaTNwWmlFQkI0VWkvdnJ1TDVSQ1FJREFRQUI=";
    const QByteArray ba =
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKsR/vDU/d6+gFvamn6sg83T9+5OvB80QomtycOrxmTtxFOALVLVUjV7Rz8LNqXJd8uNtNUqsuNb+rRszg4NGKOGDnNB2D1gy3sVPSmfF/"
        "flrpCwBA28ryCsv1Th4oZhceVCurcHtYFmq58xmkVaAzRJYfnps6eJAOqdRApjGEGQIDAQAB";
    qDebug() << " ba before: " << ba;

    QCA::PublicKey publicKey = QCA::RSAPublicKey::fromDER(QByteArray::fromBase64(ba), &conversionResult);
    if (conversionResult != QCA::ConvertGood) {
        qDebug() << "Public key read failed" << conversionResult;
    } else {
        qDebug() << "Public key read success";
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    //    parser.addPositionalArgument(
    //                QStringLiteral("server address"), QStringLiteral("add specific server address"));
    parser.process(app);

    (void)new CheckRSAPublicKey;

    app.exec();
    return 0;
}
