/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "viewer/viewer_p.h"
#include <QObject>
namespace Akonadi
{
class Session;
}
namespace MessageViewer
{
class ModifyMessageDisplayFormatJob : public QObject
{
    Q_OBJECT
public:
    explicit ModifyMessageDisplayFormatJob(Akonadi::Session *session, QObject *parent = nullptr);
    ~ModifyMessageDisplayFormatJob() override;

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
