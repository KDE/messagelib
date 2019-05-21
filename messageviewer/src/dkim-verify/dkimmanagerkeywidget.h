/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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
#ifndef DKIMMANAGERKEYWIDGET_H
#define DKIMMANAGERKEYWIDGET_H

#include <QWidget>
#include "messageviewer_private_export.h"
class QTreeWidget;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMManagerKeyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyWidget(QWidget *parent = nullptr);
    ~DKIMManagerKeyWidget();

    //TODO make it private ?
    void loadKeys();
    void saveKeys();
private:
    QTreeWidget *mTreeWidget = nullptr;
};
}

#endif // DKIMMANAGERKEYWIDGET_H
