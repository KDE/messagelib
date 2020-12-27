/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTINFO_H
#define REMOTECONTENTINFO_H


#include "messageviewer_private_export.h"
#include <QObject>
#include <QDebug>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentInfo
{
     Q_GADGET
public:
    enum class RemoteContentInfoStatus {
        Unknown,
        Blocked,
        Authorized,
    };
    Q_ENUM(RemoteContentInfoStatus)

    RemoteContentInfo();
    ~RemoteContentInfo();

    Q_REQUIRED_RESULT QString url() const;
    void setUrl(const QString &url);

    Q_REQUIRED_RESULT RemoteContentInfoStatus status() const;
    void setStatus(const RemoteContentInfoStatus &status);

    Q_REQUIRED_RESULT bool isValid() const;

private:
    RemoteContentInfoStatus mStatus = RemoteContentInfoStatus::Unknown;
    QString mUrl;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::RemoteContentInfo, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator <<(QDebug d, const MessageViewer::RemoteContentInfo &t);
#endif // REMOTECONTENTINFO_H
