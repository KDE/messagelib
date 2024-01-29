/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "distributionlistexpandjob.h"

#include <Akonadi/ContactGroupExpandJob>
#include <Akonadi/ContactGroupSearchJob>

using namespace MessageComposer;

DistributionListExpandJob::DistributionListExpandJob(const QString &name, QObject *parent)
    : KJob(parent)
    , mListName(name)
{
}

DistributionListExpandJob::~DistributionListExpandJob() = default;

void DistributionListExpandJob::start()
{
    auto job = new Akonadi::ContactGroupSearchJob(this);
    job->setQuery(Akonadi::ContactGroupSearchJob::Name, mListName);
    connect(job, &KJob::result, this, &DistributionListExpandJob::slotSearchDone);
}

QString DistributionListExpandJob::addresses() const
{
    return mEmailAddresses.join(QLatin1StringView(", "));
}

bool DistributionListExpandJob::isEmpty() const
{
    return mIsEmpty;
}

void DistributionListExpandJob::slotSearchDone(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const Akonadi::ContactGroupSearchJob *searchJob = qobject_cast<Akonadi::ContactGroupSearchJob *>(job);

    const KContacts::ContactGroup::List groups = searchJob->contactGroups();
    if (groups.isEmpty()) {
        emitResult();
        return;
    }

    auto expandJob = new Akonadi::ContactGroupExpandJob(groups.first());
    connect(expandJob, &KJob::result, this, &DistributionListExpandJob::slotExpansionDone);
    expandJob->start();
}

void DistributionListExpandJob::slotExpansionDone(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const Akonadi::ContactGroupExpandJob *expandJob = qobject_cast<Akonadi::ContactGroupExpandJob *>(job);

    const KContacts::Addressee::List contacts = expandJob->contacts();
    mEmailAddresses.reserve(contacts.count());
    for (const KContacts::Addressee &contact : contacts) {
        mEmailAddresses << contact.fullEmail();
    }

    mIsEmpty = mEmailAddresses.isEmpty();

    emitResult();
}

#include "moc_distributionlistexpandjob.cpp"
