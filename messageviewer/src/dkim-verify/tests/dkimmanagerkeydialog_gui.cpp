/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkim-verify/dkimmanagerkeydialog.h"
#include <QApplication>
#include <QCommandLineParser>
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MessageViewer::DKIMManagerKeyDialog dlg;
    dlg.exec();
    app.exec();
    return 0;
}
