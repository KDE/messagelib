/*
 * This file is part of KMail.
 *
 * SPDX-FileCopyrightText: 2010 KDAB
 * SPDX-FileContributor: Tobias Koenig <tokoe@kde.org>
 * SPDX-FileContributor: Leo Franchi <lfranchi@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "job/emailaddressresolvejob.h"

#include "aliasesexpandjob.h"
#include "settings/messagecomposersettings.h"

#include "MessageComposer/Composer"
#include "MessageComposer/InfoPart"

#include <KEmailAddress>
#include <QVariant>

using namespace MessageComposer;

class MessageComposer::EmailAddressResolveJobPrivate
{
public:
    EmailAddressResolveJobPrivate() = default;

    QVariantMap mResultMap;
    QString mFrom;
    QStringList mTo;
    QStringList mCc;
    QStringList mBcc;
    QStringList mReplyTo;
    QString mDefaultDomainName;
    int mJobCount = 0;
};

EmailAddressResolveJob::EmailAddressResolveJob(QObject *parent)
    : KJob(parent)
    , d(new MessageComposer::EmailAddressResolveJobPrivate)
{
}

EmailAddressResolveJob::~EmailAddressResolveJob() = default;

static inline bool containsAliases(const QString &address)
{
    // an valid email is defined as foo@foo.extension
    return !(address.contains(QLatin1Char('@')) && address.contains(QLatin1Char('.')));
}

static bool containsAliases(const QStringList &addresses)
{
    for (const QString &address : addresses) {
        if (containsAliases(address)) {
            return true;
        }
    }

    return false;
}

void EmailAddressResolveJob::setDefaultDomainName(const QString &domainName)
{
    d->mDefaultDomainName = domainName;
}

void EmailAddressResolveJob::start()
{
    QList<AliasesExpandJob *> jobs;

    if (containsAliases(d->mFrom)) {
        auto job = new AliasesExpandJob(d->mFrom, d->mDefaultDomainName, this);
        job->setProperty("id", QStringLiteral("infoPartFrom"));
        connect(job, &AliasesExpandJob::result, this, &EmailAddressResolveJob::slotAliasExpansionDone);
        jobs << job;
    }
    if (containsAliases(d->mTo)) {
        auto job = new AliasesExpandJob(d->mTo.join(QLatin1StringView(", ")), d->mDefaultDomainName, this);
        job->setProperty("id", QStringLiteral("infoPartTo"));
        connect(job, &AliasesExpandJob::result, this, &EmailAddressResolveJob::slotAliasExpansionDone);
        jobs << job;
    }

    if (containsAliases(d->mCc)) {
        auto job = new AliasesExpandJob(d->mCc.join(QLatin1StringView(", ")), d->mDefaultDomainName, this);
        job->setProperty("id", QStringLiteral("infoPartCc"));
        connect(job, &AliasesExpandJob::result, this, &EmailAddressResolveJob::slotAliasExpansionDone);
        jobs << job;
    }

    if (containsAliases(d->mBcc)) {
        auto job = new AliasesExpandJob(d->mBcc.join(QLatin1StringView(", ")), d->mDefaultDomainName, this);
        job->setProperty("id", QStringLiteral("infoPartBcc"));
        connect(job, &AliasesExpandJob::result, this, &EmailAddressResolveJob::slotAliasExpansionDone);
        jobs << job;
    }
    if (containsAliases(d->mReplyTo)) {
        auto job = new AliasesExpandJob(d->mReplyTo.join(QLatin1StringView(", ")), d->mDefaultDomainName, this);
        job->setProperty("id", QStringLiteral("infoPartReplyTo"));
        connect(job, &AliasesExpandJob::result, this, &EmailAddressResolveJob::slotAliasExpansionDone);
        jobs << job;
    }

    d->mJobCount = jobs.count();

    if (d->mJobCount == 0) {
        emitResult();
    } else {
        for (AliasesExpandJob *job : std::as_const(jobs)) {
            job->start();
        }
    }
}

void EmailAddressResolveJob::slotAliasExpansionDone(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const AliasesExpandJob *expandJob = qobject_cast<AliasesExpandJob *>(job);
    d->mResultMap.insert(expandJob->property("id").toString(), expandJob->addresses());

    d->mJobCount--;
    if (d->mJobCount == 0) {
        emitResult();
    }
}

void EmailAddressResolveJob::setFrom(const QString &from)
{
    d->mFrom = from;
    d->mResultMap.insert(QStringLiteral("infoPartFrom"), from);
}

void EmailAddressResolveJob::setTo(const QStringList &to)
{
    d->mTo = to;
    d->mResultMap.insert(QStringLiteral("infoPartTo"), to.join(QLatin1StringView(", ")));
}

void EmailAddressResolveJob::setCc(const QStringList &cc)
{
    d->mCc = cc;
    d->mResultMap.insert(QStringLiteral("infoPartCc"), cc.join(QLatin1StringView(", ")));
}

void EmailAddressResolveJob::setBcc(const QStringList &bcc)
{
    d->mBcc = bcc;
    d->mResultMap.insert(QStringLiteral("infoPartBcc"), bcc.join(QLatin1StringView(", ")));
}

void EmailAddressResolveJob::setReplyTo(const QStringList &replyTo)
{
    d->mReplyTo = replyTo;
    d->mResultMap.insert(QStringLiteral("infoPartReplyTo"), replyTo.join(QLatin1StringView(", ")));
}

QString EmailAddressResolveJob::expandedFrom() const
{
    return d->mResultMap.value(QStringLiteral("infoPartFrom")).toString();
}

QStringList EmailAddressResolveJob::expandedTo() const
{
    return KEmailAddress::splitAddressList(d->mResultMap.value(QStringLiteral("infoPartTo")).toString());
}

QStringList EmailAddressResolveJob::expandedCc() const
{
    return KEmailAddress::splitAddressList(d->mResultMap.value(QStringLiteral("infoPartCc")).toString());
}

QStringList EmailAddressResolveJob::expandedBcc() const
{
    return KEmailAddress::splitAddressList(d->mResultMap.value(QStringLiteral("infoPartBcc")).toString());
}

QStringList EmailAddressResolveJob::expandedReplyTo() const
{
    return KEmailAddress::splitAddressList(d->mResultMap.value(QStringLiteral("infoPartReplyTo")).toString());
}

#include "moc_emailaddressresolvejob.cpp"
