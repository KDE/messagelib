/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 * SPDX-FileContributor: Leo Franchi <lfranchi@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "messagecomposer_export.h"

#include <KJob>

#include <QStringList>

namespace MessageComposer
{
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

