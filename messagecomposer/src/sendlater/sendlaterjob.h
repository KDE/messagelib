/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KJob>
#include <QDBusPendingReply>

#include "messagecomposer_export.h"

class OrgFreedesktopAkonadiSendLaterAgentInterface;

namespace MessageComposer
{
/*!
 * \class MessageComposer::SendLaterJob
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/SendLaterJob
 * \brief Base class for send later operations
 */
class MESSAGECOMPOSER_EXPORT SendLaterJob : public KJob
{
    Q_OBJECT
public:
    enum Error {
        AgentNotAvailable = KJob::UserDefinedError,
        CallFailed,
    };

    explicit SendLaterJob(QObject *parent = nullptr);

    void start() final;

protected:
    /*! \brief Performs the actual D-Bus call to the send later agent.
        \param iface The D-Bus interface to the send later agent.
        \return A pending reply from the D-Bus call.
    */
    virtual QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) = 0;
    /*! \brief Returns a human-readable error message for the given error.
        \param error The error code that occurred.
        \param desc A description of the error.
        \return The formatted error string.
    */
    virtual QString getErrorString(Error error, const QString &desc) const = 0;
};
}
