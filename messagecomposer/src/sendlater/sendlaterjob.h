/*
   Copyright (C) 2020 Daniel Vrátil <dvratil@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MESSAGECOMPOSER_SENDLATERJOB_H
#define MESSAGECOMPOSER_SENDLATERJOB_H

#include <KJob>
#include <QDBusPendingReply>

#include "messagecomposer_export.h"

class OrgFreedesktopAkonadiSendLaterAgentInterface;

namespace MessageComposer {
class MESSAGECOMPOSER_EXPORT SendLaterJob : public KJob
{
    Q_OBJECT
public:
    enum Error {
        AgentNotAvailable = KJob::UserDefinedError,
        CallFailed
    };

    explicit SendLaterJob(QObject *parent = nullptr);

    void start() final;

protected:
    virtual QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) = 0;
    virtual QString getErrorString(Error error, const QString &desc) const = 0;
};
}

#endif