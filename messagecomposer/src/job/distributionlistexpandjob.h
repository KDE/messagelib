/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KJob>
#include <QStringList>
namespace MessageComposer
{
/**
 * @short A job to expand a distribution list to its member email addresses.
 */
class DistributionListExpandJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new distribution list expand job.
     *
     * @param name The name of the distribution list to expand.
     * @param parent The parent object.
     */
    explicit DistributionListExpandJob(const QString &name, QObject *parent = nullptr);

    /**
     * Destroys the distribution list expand job.
     */
    ~DistributionListExpandJob() override;

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the email addresses of the list members.
     */
    Q_REQUIRED_RESULT QString addresses() const;

    /**
     * Returns whether the list of email addresses is empty.
     */
    Q_REQUIRED_RESULT bool isEmpty() const;

private Q_SLOTS:
    void slotSearchDone(KJob *);
    void slotExpansionDone(KJob *);

private:
    const QString mListName;
    QStringList mEmailAddresses;
    bool mIsEmpty = false;
};
}
