/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
namespace MessageViewer
{
/**
 * @brief The DKIMManagerAuthenticationServer class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerAuthenticationServer : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerAuthenticationServer(QObject *parent = nullptr);
    ~DKIMManagerAuthenticationServer() override;
    static DKIMManagerAuthenticationServer *self();

    Q_REQUIRED_RESULT QStringList serverList() const;
    void setServerList(const QStringList &serverList);

private:
    void load();
    void save();
    QStringList mServerList;
};
}

