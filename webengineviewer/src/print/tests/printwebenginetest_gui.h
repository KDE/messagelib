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

#ifndef PRINTWEBENGINETEST_GUI_H
#define PRINTWEBENGINETEST_GUI_H

#include <QWidget>
class QWebEngineView;
class PrintWebEngineTest_Gui : public QWidget
{
    Q_OBJECT
public:
    explicit PrintWebEngineTest_Gui(QWidget *parent = Q_NULLPTR);
    ~PrintWebEngineTest_Gui();

private Q_SLOTS:
    void slotPrint();
    void slotPdfFailed();
    void slotPdfCreated(const QString &filename);
private:
    QWebEngineView *mWebEngine;
};

#endif // PRINTWEBENGINETEST_GUI_H
