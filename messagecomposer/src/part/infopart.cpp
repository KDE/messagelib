/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "infopart.h"

using namespace MessageComposer;

class Q_DECL_HIDDEN InfoPart::Private
{
public:
    Private()
    {
    }

    KMime::Headers::Base::List extraHeaders;
    QStringList to;
    QStringList cc;
    QStringList bcc;
    QStringList replyTo;
    QString subject;
    QString from;
    QString fcc;
    QString userAgent;
    QString inReplyTo;
    QString references;
    int transportId = 0;
    bool urgent = false;
};

InfoPart::InfoPart(QObject *parent)
    : MessagePart(parent)
    , d(new Private)
{
}

InfoPart::~InfoPart()
{
    delete d;
}

QString InfoPart::from() const
{
    return d->from;
}

void InfoPart::setFrom(const QString &from)
{
    d->from = from;
}

QStringList InfoPart::to() const
{
    return d->to;
}

void InfoPart::setTo(const QStringList &to)
{
    d->to = to;
}

QStringList InfoPart::cc() const
{
    return d->cc;
}

void InfoPart::setCc(const QStringList &cc)
{
    d->cc = cc;
}

QStringList InfoPart::bcc() const
{
    return d->bcc;
}

void InfoPart::setBcc(const QStringList &bcc)
{
    d->bcc = bcc;
}

QString InfoPart::subject() const
{
    return d->subject;
}

void InfoPart::setSubject(const QString &subject)
{
    d->subject = subject;
}

QStringList InfoPart::replyTo() const
{
    return d->replyTo;
}

void InfoPart::setReplyTo(const QStringList &replyTo)
{
    d->replyTo = replyTo;
}

int InfoPart::transportId() const
{
    return d->transportId;
}

void InfoPart::setTransportId(int tid)
{
    d->transportId = tid;
}

void InfoPart::setFcc(const QString &fcc)
{
    d->fcc = fcc;
}

QString InfoPart::fcc() const
{
    return d->fcc;
}

bool InfoPart::urgent() const
{
    return d->urgent;
}

void InfoPart::setUrgent(bool urgent)
{
    d->urgent = urgent;
}

QString InfoPart::inReplyTo() const
{
    return d->inReplyTo;
}

void InfoPart::setInReplyTo(const QString &inReplyTo)
{
    d->inReplyTo = inReplyTo;
}

QString InfoPart::references() const
{
    return d->references;
}

void InfoPart::setReferences(const QString &references)
{
    d->references = references;
}

void InfoPart::setExtraHeaders(const KMime::Headers::Base::List &headers)
{
    d->extraHeaders = headers;
}

KMime::Headers::Base::List InfoPart::extraHeaders() const
{
    return d->extraHeaders;
}

QString InfoPart::userAgent() const
{
    return d->userAgent;
}

void InfoPart::setUserAgent(const QString &userAgent)
{
    d->userAgent = userAgent;
}
