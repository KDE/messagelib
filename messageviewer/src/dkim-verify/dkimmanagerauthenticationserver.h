/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERAUTHENTICATIONSERVER_H
#define DKIMMANAGERAUTHENTICATIONSERVER_H

#include <QObject>
#include "messageviewer_export.h"
namespace MessageViewer {
class MESSAGEVIEWER_EXPORT DKIMManagerAuthenticationServer : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerAuthenticationServer(QObject *parent = nullptr);
    ~DKIMManagerAuthenticationServer();
    static DKIMManagerAuthenticationServer *self();

    Q_REQUIRED_RESULT QStringList serverList() const;
    void setServerList(const QStringList &serverList);

private:
    void load();
    void save();
    QStringList mServerList;
};
}

#endif // DKIMMANAGERAUTHENTICATIONSERVER_H
