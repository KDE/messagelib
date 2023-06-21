/*
    SPDX-FileCopyrightText: 2011 Martin Bednár <serafean@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "MessageCore/AttachmentFromUrlBaseJob"

using namespace MessageCore;

class Q_DECL_HIDDEN AttachmentFromUrlBaseJob::AttachmentFromUrlBaseJobPrivate
{
public:
    AttachmentFromUrlBaseJobPrivate(const QUrl &url, AttachmentFromUrlBaseJob *qq);

    AttachmentFromUrlBaseJob *const q;
    qint64 mMaxSize = -1;
    QUrl mUrl;
};

AttachmentFromUrlBaseJob::AttachmentFromUrlBaseJobPrivate::AttachmentFromUrlBaseJobPrivate(const QUrl &url, AttachmentFromUrlBaseJob *qq)
    : q(qq)
    , mUrl(url)
{
}

AttachmentFromUrlBaseJob::AttachmentFromUrlBaseJob(const QUrl &url, QObject *parent)
    : AttachmentLoadJob(parent)
    , d(new AttachmentFromUrlBaseJobPrivate(url, this))
{
}

AttachmentFromUrlBaseJob::~AttachmentFromUrlBaseJob() = default;

void AttachmentFromUrlBaseJob::setMaximumAllowedSize(qint64 size)
{
    d->mMaxSize = size;
}

qint64 AttachmentFromUrlBaseJob::maximumAllowedSize() const
{
    return d->mMaxSize;
}

void AttachmentFromUrlBaseJob::setUrl(const QUrl &url)
{
    d->mUrl = url;
}

QUrl AttachmentFromUrlBaseJob::url() const
{
    return d->mUrl;
}

#include "moc_attachmentfromurlbasejob.cpp"
