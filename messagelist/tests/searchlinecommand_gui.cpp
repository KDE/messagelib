/* SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    const int ret = app.exec();
    return ret;
}
