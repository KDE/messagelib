/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "createphishingurldatabasegui.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

CreatePhisingUrlDataBaseGuioi::CreatePhisingUrlDataBaseGuioi(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    mResult = new QTextEdit(this);
    mResult->setReadOnly(true);
    mResult->setAcceptRichText(false);
    layout->addWidget(mResult);

}

CreatePhisingUrlDataBaseGuioi::~CreatePhisingUrlDataBaseGuioi()
{

}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    CreatePhisingUrlDataBaseGuioi *w = new CreatePhisingUrlDataBaseGuioi;

    w->show();
    app.exec();
    delete w;
    return 0;
}
