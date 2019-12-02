/*
 * This file is part of KMail.
 *
 * Copyright (c) 2010 KDAB
 *
 * Authors: Tobias Koenig <tokoe@kde.org>
 *          Leo Franchi   <lfranchi@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef EMAILADDRESSRESOLVEJOB_H
#define EMAILADDRESSRESOLVEJOB_H

#include "messagecomposer_export.h"

#include <KJob>

#include <QStringList>

namespace MessageComposer {
class Composer;

class EmailAddressResolveJobPrivate;
/**
 * @short A job to resolve nicknames, distribution lists and email addresses for queued emails.
 */
class MESSAGECOMPOSER_EXPORT EmailAddressResolveJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new email address resolve job.
     *
     * @param parent The parent object.
     */
    explicit EmailAddressResolveJob(QObject *parent = nullptr);

    /**
     * Destroys the email address resolve job.
     */
    ~EmailAddressResolveJob() override;

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Sets the from address to expand.
     */
    void setFrom(const QString &from);

    /**
     * Sets the from address to expand.
     */
    void setTo(const QStringList &from);

    /**
     * Sets the from address to expand.
     */
    void setCc(const QStringList &from);

    /**
     * Sets the from address to expand.
     */
    void setBcc(const QStringList &from);
    /**
     * Sets the Reply-To address to expand.
     */
    void setReplyTo(const QStringList &replyTo);

    /**
     * Returns the expanded From field
     */
    Q_REQUIRED_RESULT QString expandedFrom() const;

    /**
     * Returns the expanded To field
     */
    Q_REQUIRED_RESULT QStringList expandedTo() const;

    /**
     * Returns the expanded CC field
     */
    Q_REQUIRED_RESULT QStringList expandedCc() const;

    /**
     * Returns the expanded Bcc field
     */
    Q_REQUIRED_RESULT QStringList expandedBcc() const;

    void setDefaultDomainName(const QString &domainName);
    /**
     * Returns the expanded Reply-To field
     */
    Q_REQUIRED_RESULT QStringList expandedReplyTo() const;
private Q_SLOTS:
    void slotAliasExpansionDone(KJob *);

private:
    EmailAddressResolveJobPrivate *const d;
};
}

#endif
