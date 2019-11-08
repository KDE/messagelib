/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#ifndef DKIMCHECKAUTHENTICATIONSTATUSJOB_H
#define DKIMCHECKAUTHENTICATIONSTATUSJOB_H

#include <QObject>
#include "messageviewer_private_export.h"
#include <KMime/Message>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMCheckAuthenticationStatusJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJob(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJob();

    void start();

    Q_REQUIRED_RESULT bool canStart() const;
    Q_REQUIRED_RESULT KMime::Message::Ptr message() const;
    void setMessage(const KMime::Message::Ptr &message);

    Q_REQUIRED_RESULT QString authenticationResult() const;
    void setAuthenticationResult(const QString &authenticationResult);

Q_SIGNALS:
    //TODO implement
    void result();

private:
    KMime::Message::Ptr mMessage;
    QString mAuthenticationResult;
};
}

#endif // DKIMCHECKAUTHENTICATIONSTATUSJOB_H
