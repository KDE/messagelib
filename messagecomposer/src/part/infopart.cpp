/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "infopart.h"

using namespace MessageComposer;

class InfoPart::InfoPartPrivate
{
public:
    InfoPartPrivate() = default;

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
    , d(new InfoPartPrivate)
{
}

InfoPart::~InfoPart() = default;

QString InfoPart::from() const
{
    return d->from;
}

void InfoPart::setFrom(const QString &from)
{
    if (d->from == from) {
        return;
    }
    d->from = from;
    Q_EMIT fromChanged();
}

QStringList InfoPart::to() const
{
    return d->to;
}

void InfoPart::setTo(const QStringList &to)
{
    if (d->to == to) {
        return;
    }
    d->to = to;
    Q_EMIT toChanged();
}

QStringList InfoPart::cc() const
{
    return d->cc;
}

void InfoPart::setCc(const QStringList &cc)
{
    if (d->cc == cc) {
        return;
    }
    d->cc = cc;
    Q_EMIT ccChanged();
}

QStringList InfoPart::bcc() const
{
    return d->bcc;
}

void InfoPart::setBcc(const QStringList &bcc)
{
    if (d->bcc == bcc) {
        return;
    }
    d->bcc = bcc;
    Q_EMIT bccChanged();
}

QString InfoPart::subject() const
{
    return d->subject;
}

void InfoPart::setSubject(const QString &subject)
{
    if (d->subject == subject) {
        return;
    }
    d->subject = subject;
    Q_EMIT subjectChanged();
}

QStringList InfoPart::replyTo() const
{
    return d->replyTo;
}

void InfoPart::setReplyTo(const QStringList &replyTo)
{
    if (d->replyTo == replyTo) {
        return;
    }
    d->replyTo = replyTo;
    Q_EMIT replyToChanged();
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
    if (d->fcc == fcc) {
        return;
    }
    d->fcc = fcc;
    Q_EMIT fccChanged();
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
    if (d->urgent == urgent) {
        return;
    }
    d->urgent = urgent;
    Q_EMIT urgentChanged();
}

QString InfoPart::inReplyTo() const
{
    return d->inReplyTo;
}

void InfoPart::setInReplyTo(const QString &inReplyTo)
{
    if (d->inReplyTo == inReplyTo) {
        return;
    }
    d->inReplyTo = inReplyTo;
    Q_EMIT inReplyToChanged();
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
    if (d->userAgent == userAgent) {
        return;
    }
    d->userAgent = userAgent;
    Q_EMIT userAgentChanged();
}

#include "moc_infopart.cpp"
