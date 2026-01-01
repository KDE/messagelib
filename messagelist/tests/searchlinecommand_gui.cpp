/* SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidgetgui.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    SearchLineCommandWidgetGui *w = new SearchLineCommandWidgetGui;
    w->resize(600, 400);
    w->show();
    const int ret = app.exec();
    return ret;
}
