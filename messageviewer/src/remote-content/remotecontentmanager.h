/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QObject>
#include <QUrl>
namespace MessageViewer
{
class RemoteContentInfo;
/**
 * @brief The RemoteContentManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentManager : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentManager(QObject *parent = nullptr);
    ~RemoteContentManager() override;
    static RemoteContentManager *self();
    void clear();

    Q_REQUIRED_RESULT bool isAutorized(const QString &url) const;
    Q_REQUIRED_RESULT bool isAutorized(const QUrl &url, bool &contains) const;

    void addRemoteContent(const RemoteContentInfo &info);
    Q_REQUIRED_RESULT QVector<RemoteContentInfo> removeContentInfo() const;

    void setRemoveContentInfo(const QVector<RemoteContentInfo> &removeContentInfo);

    Q_REQUIRED_RESULT bool isUnique(const RemoteContentInfo &newInfo) const;

    Q_REQUIRED_RESULT bool isBlocked(const QUrl &url, bool &contains) const;

private:
    void loadSettings();
    void writeSettings();
    QVector<RemoteContentInfo> mRemoveContentInfo;
};
}

