/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "aliasesexpandjob.h"

#include "distributionlistexpandjob.h"

#include <Akonadi/ContactGroupExpandJob>
#include <Akonadi/ContactSearchJob>
#include <KEmailAddress>

#include <MessageCore/StringUtil>

using namespace MessageComposer;

AliasesExpandJob::AliasesExpandJob(const QString &recipients, const QString &defaultDomain, QObject *parent)
    : KJob(parent)
    , mRecipients(KEmailAddress::splitAddressList(recipients))
    , mDefaultDomain(defaultDomain)
{
}

AliasesExpandJob::~AliasesExpandJob() = default;

void AliasesExpandJob::start()
{
    // At first we try to expand the recipient to a distribution list
    // or nick name and save the results in a map for later lookup
    for (const QString &recipient : std::as_const(mRecipients)) {
        // speedup: assume aliases and list names don't contain '@'
        if (recipient.isEmpty() || recipient.contains(QLatin1Char('@'))) {
            continue;
        }

        // check for distribution list
        auto expandJob = new DistributionListExpandJob(recipient, this);
        expandJob->setProperty("recipient", recipient);
        connect(expandJob, &Akonadi::ContactGroupExpandJob::result, this, &AliasesExpandJob::slotDistributionListExpansionDone);
        mDistributionListExpansionJobs++;
        expandJob->start();

        // check for nick name
        auto searchJob = new Akonadi::ContactSearchJob(this);
        searchJob->setProperty("recipient", recipient);
        searchJob->setQuery(Akonadi::ContactSearchJob::NickName, recipient.toLower());
        connect(searchJob, &Akonadi::ContactSearchJob::result, this, &AliasesExpandJob::slotNicknameExpansionDone);
        mNicknameExpansionJobs++;
        searchJob->start();
    }

    if (mDistributionListExpansionJobs == 0 && mNicknameExpansionJobs == 0) {
        emitResult();
    }
}

QString AliasesExpandJob::addresses() const
{
    return mEmailAddresses;
}

QStringList AliasesExpandJob::emptyDistributionLists() const
{
    return mEmptyDistributionLists;
}

void AliasesExpandJob::slotDistributionListExpansionDone(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const DistributionListExpandJob *expandJob = qobject_cast<DistributionListExpandJob *>(job);
    const QString recipient = expandJob->property("recipient").toString();

    DistributionListExpansionResult result;
    result.addresses = expandJob->addresses();
    result.isEmpty = expandJob->isEmpty();

    mDistListExpansionResults.insert(recipient, result);

    mDistributionListExpansionJobs--;
    if (mDistributionListExpansionJobs == 0 && mNicknameExpansionJobs == 0) {
        finishExpansion();
    }
}

void AliasesExpandJob::slotNicknameExpansionDone(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const Akonadi::ContactSearchJob *searchJob = qobject_cast<Akonadi::ContactSearchJob *>(job);
    const KContacts::Addressee::List contacts = searchJob->contacts();
    const QString recipient = searchJob->property("recipient").toString();

    for (const KContacts::Addressee &contact : contacts) {
        if (contact.nickName().toLower() == recipient.toLower()) {
            mNicknameExpansionResults.insert(recipient, contact.fullEmail());
            break;
        }
    }

    mNicknameExpansionJobs--;
    if (mDistributionListExpansionJobs == 0 && mNicknameExpansionJobs == 0) {
        finishExpansion();
    }
}

void AliasesExpandJob::finishExpansion()
{
    for (const QString &recipient : std::as_const(mRecipients)) {
        if (recipient.isEmpty()) {
            continue;
        }
        if (!mEmailAddresses.isEmpty()) {
            mEmailAddresses += QLatin1StringView(", ");
        }

        const QString receiver = recipient.trimmed();

        // take prefetched expand distribution list results
        const DistributionListExpansionResult result = mDistListExpansionResults.value(recipient);
        QString displayName;
        QString addrSpec;
        QString comment;

        if (result.isEmpty) {
            KEmailAddress::splitAddress(receiver, displayName, addrSpec, comment);
            mEmailAddressOnly.append(addrSpec);
            mEmailAddresses += receiver;
            mEmptyDistributionLists << receiver;
            continue;
        }

        if (!result.addresses.isEmpty()) {
            KEmailAddress::splitAddress(result.addresses, displayName, addrSpec, comment);
            mEmailAddressOnly.append(addrSpec);

            mEmailAddresses += result.addresses;
            continue;
        }

        // take prefetched expand nick name results
        const QString recipientValue = mNicknameExpansionResults.value(recipient);
        if (!recipientValue.isEmpty()) {
            mEmailAddresses += recipientValue;
            KEmailAddress::splitAddress(recipientValue, displayName, addrSpec, comment);
            mEmailAddressOnly.append(addrSpec);

            continue;
        }

        // check whether the address is missing the domain part
        KEmailAddress::splitAddress(receiver, displayName, addrSpec, comment);
        if (!addrSpec.contains(QLatin1Char('@'))) {
            if (!mDefaultDomain.isEmpty()) {
                mEmailAddresses += KEmailAddress::normalizedAddress(displayName, addrSpec + QLatin1Char('@') + mDefaultDomain, comment);
            } else {
                mEmailAddresses += MessageCore::StringUtil::guessEmailAddressFromLoginName(addrSpec);
            }
        } else {
            mEmailAddresses += receiver;
        }
        mEmailAddressOnly.append(addrSpec);
    }

    emitResult();
}

QStringList AliasesExpandJob::emailAddressOnly() const
{
    return mEmailAddressOnly;
}

#include "moc_aliasesexpandjob.cpp"
