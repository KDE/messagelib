/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#include <QStandardPaths>
#include <QCoreApplication>
#include <QCommandLineParser>
#include "lookupkey.h"
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
            LookUpKey *d = new LookUpKey();
            d->lookUpServer(str);
        }
    }

    app.exec();
    return 0;
}
