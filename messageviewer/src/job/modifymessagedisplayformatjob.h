/*
   Copyright (C) 2014-2017 Laurent Montel <montel@kde.org>

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

#ifndef MODIFYMESSAGEDISPLAYFORMATJOB_H
#define MODIFYMESSAGEDISPLAYFORMATJOB_H

#include <QObject>
#include "viewer/viewer_p.h"
namespace Akonadi {
class Session;
}
namespace MessageViewer {
class ModifyMessageDisplayFormatJob : public QObject
{
    Q_OBJECT
public:
    explicit ModifyMessageDisplayFormatJob(Akonadi::Session *session, QObject *parent = nullptr);
    ~ModifyMessageDisplayFormatJob();

    void setRemoteContent(bool remote);
    void setMessageFormat(Viewer::DisplayFormatMessage format);
    void setResetFormat(bool resetFormat);

    void start();
    void setMessageItem(const Akonadi::Item &messageItem);

private:
    void slotModifyItemDone(KJob *job);
    void resetDisplayFormat();
    void modifyDisplayFormat();
    Akonadi::Session *mSession;
    Akonadi::Item mMessageItem;
    Viewer::DisplayFormatMessage mMessageFormat;
    bool mRemoteContent;
    bool mResetFormat;
};
}

#endif // MODIFYMESSAGEDISPLAYFORMATJOB_H
