/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "messagecomposer_export.h"

#include <KJob>

#include <QMap>
#include <QStringList>

namespace MessageComposer
{
/**
 * @short A job to expand aliases to email addresses.
 *
 * Expands aliases (distribution lists and nick names) and appends a
 * domain part to all email addresses which are missing the domain part.
 */
class MESSAGECOMPOSER_EXPORT AliasesExpandJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new aliases expand job.
     *
     * @param recipients The comma separated list of recipient.
     * @param defaultDomain The default domain that shall be used when expanding aliases.
     * @param parent The parent object.
     */
    AliasesExpandJob(const QString &recipients, const QString &defaultDomain, QObject *parent = nullptr);

    /**
     * Destroys the aliases expand job.
     */
    ~AliasesExpandJob() override;

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the expanded email addresses.
     */
    Q_REQUIRED_RESULT QString addresses() const;

    /**
     * Returns the list of distribution lists that resolved to an empty member list.
     */
    Q_REQUIRED_RESULT QStringList emptyDistributionLists() const;

    Q_REQUIRED_RESULT QStringList emailAddressOnly() const;

private Q_SLOTS:
    void slotDistributionListExpansionDone(KJob *);
    void slotNicknameExpansionDone(KJob *);

private:
    void finishExpansion();

    QStringList mEmailAddressOnly;

    QStringList mRecipients;
    QString mDefaultDomain;

    QString mEmailAddresses;
    QStringList mEmptyDistributionLists;

    uint mDistributionListExpansionJobs = 0;
    uint mNicknameExpansionJobs = 0;

    struct DistributionListExpansionResult {
        QString addresses;
        bool isEmpty = false;
    };
    QMap<QString, DistributionListExpansionResult> mDistListExpansionResults;
    QMap<QString, QString> mNicknameExpansionResults;
};
}

