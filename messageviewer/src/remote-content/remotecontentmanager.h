/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] bool isAutorized(const QString &url) const;
    [[nodiscard]] bool isAutorized(const QUrl &url, bool &contains) const;

    void addRemoteContent(const RemoteContentInfo &info);
    [[nodiscard]] QList<RemoteContentInfo> removeContentInfo() const;

    void setRemoveContentInfo(const QList<RemoteContentInfo> &removeContentInfo);

    [[nodiscard]] bool isUnique(const RemoteContentInfo &newInfo) const;

    [[nodiscard]] bool isBlocked(const QUrl &url, bool &contains) const;

private:
    MESSAGEVIEWER_NO_EXPORT void loadSettings();
    MESSAGEVIEWER_NO_EXPORT void writeSettings();
    QList<RemoteContentInfo> mRemoveContentInfo;
};
}
