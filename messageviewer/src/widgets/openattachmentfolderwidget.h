/*
   Copyright (C) 2014-2016 Laurent Montel <montel@kde.org>

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

#ifndef OPENATTACHMENTFOLDERWIDGET_H
#define OPENATTACHMENTFOLDERWIDGET_H

#include <KMessageWidget>
#include <QUrl>
class QTimer;
namespace MessageViewer
{
class OpenAttachmentFolderWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit OpenAttachmentFolderWidget(QWidget *parent = nullptr);
    ~OpenAttachmentFolderWidget();

    void setFolder(const QUrl &url);

public Q_SLOTS:
    void slotShowWarning();
    void slotHideWarning();

private:
    void slotOpenAttachmentFolder();
    void slotTimeOut();
    void slotExplicitlyClosed();
    QUrl mUrl;
    QTimer *mTimer;
};
}

#endif // OPENATTACHMENTFOLDERWIDGET_H
