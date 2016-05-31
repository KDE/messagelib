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

#ifndef PRINTCONFIGUREWIDGET_H
#define PRINTCONFIGUREWIDGET_H

#include <QWidget>
#include <QPageLayout>
class QLabel;
class QToolButton;
namespace WebEngineViewer
{
class PrintConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrintConfigureWidget(QWidget *parent = Q_NULLPTR);
    ~PrintConfigureWidget();

    QPageLayout currentPageLayout() const;

private Q_SLOTS:
    void slotSelectPrintLayout();

private:
    void updatePageLayoutLabel();
    QPageLayout mCurrentPageLayout;
    QLabel *mPrintLayoutLabel;
    QToolButton *mSelectPrintLayout;
};
}
#endif // PRINTCONFIGUREWIDGET_H
