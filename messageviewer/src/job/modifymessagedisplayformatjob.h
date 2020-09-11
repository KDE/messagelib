/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    Akonadi::Session *const mSession;
    Akonadi::Item mMessageItem;
    Viewer::DisplayFormatMessage mMessageFormat = Viewer::UseGlobalSetting;
    bool mRemoteContent = false;
    bool mResetFormat = false;
};
}

#endif // MODIFYMESSAGEDISPLAYFORMATJOB_H
