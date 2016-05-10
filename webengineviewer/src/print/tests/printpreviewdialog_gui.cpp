/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "printpreviewdialog_gui.h"
#include "../printpreviewdialog.h"

#include <QApplication>
#include <QStandardPaths>
#include <QFileDialog>

PrintPreviewDialog_gui::PrintPreviewDialog_gui()
{

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    const QString filename = QFileDialog::getOpenFileName(0, QStringLiteral("Open PDF"), QString(), QStringLiteral("*.pdf"));
    WebEngineViewer::PrintPreviewDialog dlg;
    dlg.loadFile(filename);
    dlg.show();
    const int ret = app.exec();
    return ret;
}
