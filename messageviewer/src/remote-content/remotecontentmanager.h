/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#ifndef REMOTECONTENTMANAGER_H
#define REMOTECONTENTMANAGER_H

#include "messageviewer_private_export.h"
#include <QObject>
namespace MessageViewer {
class RemoteContentInfo;
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentManager : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentManager(QObject *parent = nullptr);
    ~RemoteContentManager() override;
    static RemoteContentManager *self();

    Q_REQUIRED_RESULT bool isAutorized(const QString &url) const;

private:
    void loadSettings();
    void writeSettings();
    QVector<RemoteContentInfo> mRemoveContentInfo;
};
}

#endif
