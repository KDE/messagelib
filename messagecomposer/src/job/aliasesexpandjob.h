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
    explicit AliasesExpandJob(const QString &recipients, const QString &defaultDomain, QObject *parent = nullptr);

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
    [[nodiscard]] QString addresses() const;

    /**
     * Returns the list of distribution lists that resolved to an empty member list.
     */
    [[nodiscard]] QStringList emptyDistributionLists() const;

    [[nodiscard]] QStringList emailAddressOnly() const;

private:
    MESSAGECOMPOSER_NO_EXPORT void slotDistributionListExpansionDone(KJob *);
    MESSAGECOMPOSER_NO_EXPORT void slotNicknameExpansionDone(KJob *);
    MESSAGECOMPOSER_NO_EXPORT void finishExpansion();

    QStringList mEmailAddressOnly;

    const QStringList mRecipients;
    const QString mDefaultDomain;

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
