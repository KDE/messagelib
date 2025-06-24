/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "lookupkey.h"
using namespace Qt::Literals::StringLiterals;

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QStandardPaths>
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addPositionalArgument(u"server address"_s, u"add specific server address"_s);
    parser.process(app);

    if (!parser.positionalArguments().isEmpty()) {
        const QStringList lst = parser.positionalArguments();
        for (const QString &str : lst) {
            auto d = new LookUpKey();
            d->lookUpServer(str);
        }
    }

    app.exec();
    return 0;
}
